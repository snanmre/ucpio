#ifndef __uCPIO_H__
#define __uCPIO_H__

#include "ucpio_entry.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define uCPIO_EOF  1
#define uCPIO_OK   0
#define uCPIO_FAIL -1

    struct ucpio;
    struct ucpio_entry;

    struct ucpio *ucpio_read_new();

    int ucpio_read_open_filename(struct ucpio *cpio, const char *filename, size_t block_size);

    int ucpio_read_next_header(struct ucpio *cpio, struct ucpio_entry **entry);

    int ucpio_read_data(struct ucpio *cpio, void *, size_t);
    int ucpio_read_data_skip(struct ucpio *cpio);

    int ucpio_read_close(struct ucpio *cpio);
    int ucpio_read_free(struct ucpio *cpio);

#ifdef __cplusplus
}
#endif

#endif  // __uCPIO_H__