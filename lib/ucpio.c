#include "ucpio.h"

#include "ucpio-priv.h"
#include "ucpio_entry.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define CHECK_CPIO(cpio) \
    if (!(cpio)) {       \
        return -1;       \
    }

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static uint8_t to_uint8_t(char hex)
{
    if (hex >= 'A') {
        return hex - 'A' + 10;
    } else if (hex >= 'a') {
        return hex - 'a' + 10;
    }

    return hex - '0';
}

static uint32_t to_uint32_t(const char *hexstr)
{
    return ((to_uint8_t(hexstr[0]) << 4) | (to_uint8_t(hexstr[1]))) << 24 |
           ((to_uint8_t(hexstr[2]) << 4) | (to_uint8_t(hexstr[3]))) << 16 |
           ((to_uint8_t(hexstr[4]) << 4) | (to_uint8_t(hexstr[5]))) << 8 |
           ((to_uint8_t(hexstr[6]) << 4) | (to_uint8_t(hexstr[7])));
}

static struct ucpio_entry *to_cpio_entry(struct ucpio_entry *e, struct cpio_newc_header *h)
{
    if (!e || !h) {
        return NULL;
    }

    e->ino = to_uint32_t(h->c_ino);
    e->mode = to_uint32_t(h->c_mode);
    e->uid = to_uint32_t(h->c_uid);
    e->gid = to_uint32_t(h->c_gid);
    e->nlink = to_uint32_t(h->c_nlink);
    e->mtime = to_uint32_t(h->c_mtime);
    e->filesize = to_uint32_t(h->c_filesize);
    e->devmajor = to_uint32_t(h->c_devmajor);
    e->devminor = to_uint32_t(h->c_devminor);
    e->rdevmajor = to_uint32_t(h->c_rdevmajor);
    e->rdevminor = to_uint32_t(h->c_rdevminor);
    e->namesize = to_uint32_t(h->c_namesize);
    e->check = to_uint32_t(h->c_check);

    return e;
}

struct ucpio *ucpio_read_new()
{
    struct ucpio *cpio = calloc(sizeof(struct ucpio), 1);
    cpio->fd = -1;
    cpio->block_size = 0;

    cpio->entry = ucpio_entry_new();
    if (!cpio->entry) {
        free(cpio);
        return NULL;
    }

    return cpio;
}

int ucpio_read_open_filename(struct ucpio *cpio, const char *filename, size_t block_size)
{
    CHECK_CPIO(cpio);

    if (!filename) {
        return uCPIO_FAIL;
    }

    cpio->fd = open(filename, O_RDONLY);
    if (cpio->fd == -1) {
        return -1;
    }
    cpio->block_size = block_size;

    return uCPIO_OK;
}

int ucpio_read_next_header(struct ucpio *cpio, struct ucpio_entry **entry)
{
    CHECK_CPIO(cpio);

    if (cpio->entry->remaining) {
        errno = EFAULT;
        return uCPIO_FAIL;
    }

    if (cpio->entry->data_padding) {
        off_t rc = lseek(cpio->fd, cpio->entry->data_padding, SEEK_CUR);
        if (rc == -1) {
            return uCPIO_FAIL;
        }
    }

    struct cpio_newc_header header;

    *entry = NULL;
    int rc = read(cpio->fd, &header, sizeof(header));
    if (rc != sizeof(header)) {
        return uCPIO_FAIL;
    }

    if (strncmp(header.c_magic, "070701", 6)) {
        errno = EINVAL;
        return uCPIO_FAIL;
    }

    ucpio_entry_clear(cpio->entry);

    if (!to_cpio_entry(cpio->entry, &header)) {
        return uCPIO_FAIL;
    }

    if (cpio->entry->namesize == 0) {
        return uCPIO_FAIL;
    }

    cpio->entry->filename = malloc(cpio->entry->namesize);
    if (!cpio->entry->filename) {
        return uCPIO_FAIL;
    }

    rc = read(cpio->fd, cpio->entry->filename, cpio->entry->namesize);
    if (rc == -1) {
        return uCPIO_FAIL;
    }

    // end indicator
    if (cpio->entry->namesize == 11 && !strncmp(cpio->entry->filename, "TRAILER!!!", 10)) {
        return uCPIO_EOF;
    }

    const int total_size = sizeof(header) + cpio->entry->namesize;

    if (total_size & 0x3) {
        const int padding = 4 - (total_size & 0x3);
        off_t rc = lseek(cpio->fd, padding, SEEK_CUR);
        if (rc == -1) {
            free(cpio->entry->filename);
            cpio->entry->filename = NULL;
            return uCPIO_FAIL;
        }
    }

    cpio->entry->remaining = cpio->entry->filesize;

    if (S_ISLNK(cpio->entry->mode)) {
        cpio->entry->symlink = malloc(cpio->entry->filesize);
        rc = read(cpio->fd, cpio->entry->symlink, cpio->entry->filesize);
        if (rc == -1) {
            return uCPIO_FAIL;
        }
        cpio->entry->remaining = 0;
    }

    if (cpio->entry->filesize & 0x3) {
        cpio->entry->data_padding = 4 - (cpio->entry->filesize & 0x3);
    } else {
        cpio->entry->data_padding = 0;
    }

    *entry = cpio->entry;
    return uCPIO_OK;
}

int ucpio_read_data(struct ucpio *cpio, void *buf, size_t size)
{
    CHECK_CPIO(cpio);

    int rc = read(cpio->fd, buf, MIN(size, cpio->entry->remaining));
    if (rc == -1) {
        return -1;
    }
    cpio->entry->remaining -= rc;

    return rc;
}

int ucpio_read_data_skip(struct ucpio *cpio)
{
    const off_t delta = cpio->entry->remaining + cpio->entry->data_padding;
    if (delta) {
        off_t rc = lseek(cpio->fd, delta, SEEK_CUR);
        if (rc == -1) {
            return uCPIO_FAIL;
        }
    }

    cpio->entry->remaining = 0;
    cpio->entry->data_padding = 0;
    return uCPIO_OK;
}

int ucpio_read_close(struct ucpio *cpio)
{
    if (!cpio) {
        return uCPIO_FAIL;
    }

    if (cpio->fd >= 0) {
        close(cpio->fd);
        cpio->fd = -1;
    }

    return uCPIO_OK;
}

int ucpio_read_free(struct ucpio *cpio)
{
    if (!cpio) {
        return uCPIO_FAIL;
    }

    ucpio_read_close(cpio);

    if (cpio->entry) {
        ucpio_entry_free(cpio->entry);
        cpio->entry = NULL;
    }

    free(cpio);
    return uCPIO_OK;
}
