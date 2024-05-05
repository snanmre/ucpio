#ifndef __uCPIO_PRIV_H__
#define __uCPIO_PRIV_H__

#include <sys/types.h>

struct cpio_newc_header
{
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
};

struct ucpio_entry
{
    ino_t ino;
    mode_t mode;
    uid_t uid;
    gid_t gid;
    nlink_t nlink;
    time_t mtime;
    int64_t filesize;
    dev_t devmajor;
    dev_t devminor;
    dev_t rdevmajor;
    dev_t rdevminor;
    u_int32_t namesize;
    u_int32_t check;

    char *filename;
    char *symlink;

    size_t remaining;
    size_t data_padding;
};

struct ucpio
{
    int fd;
    int block_size;

    struct ucpio_entry *entry;
};

#endif  // __uCPIO_PRIV_H__