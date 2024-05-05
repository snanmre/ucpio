#ifndef __uCPIO_ENTRY_H__
#define __uCPIO_ENTRY_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    struct ucpio_entry;

    struct ucpio_entry *ucpio_entry_new(void);
    struct ucpio_entry *ucpio_entry_clear(struct ucpio_entry *e);
    void ucpio_entry_free(struct ucpio_entry *e);

    gid_t ucpio_entry_gid(struct ucpio_entry *e);
    uid_t ucpio_entry_uid(struct ucpio_entry *e);
    mode_t ucpio_entry_mode(struct ucpio_entry *e);
    const char *ucpio_entry_hardlink(struct ucpio_entry *e);
    const char *ucpio_entry_pathname(struct ucpio_entry *e);
    /** @brief Destination of the symbolic link */
    const char *ucpio_entry_symlink(struct ucpio_entry *e);
    int64_t ucpio_entry_size(struct ucpio_entry *a);
    dev_t ucpio_entry_devmajor(struct ucpio_entry *a);
    dev_t ucpio_entry_devminor(struct ucpio_entry *a);
    ino_t ucpio_entry_ino(struct ucpio_entry *a);
    dev_t ucpio_entry_rdevmajor(struct ucpio_entry *a);
    dev_t ucpio_entry_rdevminor(struct ucpio_entry *a);
    nlink_t ucpio_entry_nlink(struct ucpio_entry *a);
    time_t ucpio_entry_mtime(struct ucpio_entry *e);

#ifdef __cplusplus
}
#endif

#endif  // __uCPIO_ENTRY_H__
