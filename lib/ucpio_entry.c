#include "ucpio_entry.h"

#include "ucpio-priv.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct ucpio_entry *ucpio_entry_new(void)
{
    struct ucpio_entry *e = calloc(sizeof(struct ucpio_entry), 1);
    return e;
}

struct ucpio_entry *ucpio_entry_clear(struct ucpio_entry *e)
{
    if (!e) {
        return e;
    }

    if (e->filename) {
        free(e->filename);
        e->filename = NULL;
    }

    if (e->symlink) {
        free(e->symlink);
        e->symlink = NULL;
    }

    memset(e, 0, sizeof(struct ucpio_entry));
    return e;
}

void ucpio_entry_free(struct ucpio_entry *e)
{
    ucpio_entry_clear(e);
    free(e);
}

gid_t ucpio_entry_gid(struct ucpio_entry *e)
{
    return e->gid;
}

uid_t ucpio_entry_uid(struct ucpio_entry *e)
{
    return e->uid;
}

mode_t ucpio_entry_mode(struct ucpio_entry *e)
{
    return e->mode;
}

const char *ucpio_entry_hardlink(struct ucpio_entry *e)
{
    // XXX not-supported
    (void)e;
    return NULL;
}

const char *ucpio_entry_pathname(struct ucpio_entry *e)
{
    return e->filename;
}

const char *ucpio_entry_symlink(struct ucpio_entry *e)
{
    return e->symlink;
}

int64_t ucpio_entry_size(struct ucpio_entry *e)
{
    return e->filesize;
}

dev_t ucpio_entry_devmajor(struct ucpio_entry *e)
{
    return e->devmajor;
}

dev_t ucpio_entry_devminor(struct ucpio_entry *e)
{
    return e->devminor;
}

ino_t ucpio_entry_ino(struct ucpio_entry *e)
{
    return e->ino;
}

dev_t ucpio_entry_rdevmajor(struct ucpio_entry *e)
{
    return e->rdevmajor;
}

dev_t ucpio_entry_rdevminor(struct ucpio_entry *e)
{
    return e->rdevminor;
}

nlink_t ucpio_entry_nlink(struct ucpio_entry *e)
{
    return e->nlink;
}

time_t ucpio_entry_mtime(struct ucpio_entry *e)
{
    return e->mtime;
}
