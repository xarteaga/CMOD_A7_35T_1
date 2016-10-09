
#ifndef SW_SYSTEM_BUFFER_H
#define SW_SYSTEM_BUFFER_H

#include <inttypes.h>

typedef enum {
    BUFFER_RETURN_NOT_OK = -1,
    BUFFER_RETURN_FAIL_ALLOCATION = -2,
    BUFFER_RETURN_READ_EMPTY_BUFFER = -3,
    BUFFER_RETURN_WRITE_FULL_BUFFER = -3,
    BUFFER_RETURN_NULL_POINTER = -4,
    BUFFER_RETURN_OK = 0
} t_buffer_return;

typedef struct {
    uint8_t *ptr;
    uint32_t write_ptr;
    uint32_t read_ptr;
    uint32_t size;
    t_buffer_return error;
} t_buffer;

inline t_buffer_return buffer_create(t_buffer *buffer, size_t size);

inline size_t buffer_available(t_buffer *buffer);

inline t_buffer_return buffer_read_byte(t_buffer *buffer, uint8_t *byte);

inline t_buffer_return buffer_write_byte(t_buffer *buffer, uint8_t *byte);

t_buffer_return buffer_write_string(t_buffer *buffer, uint8_t *str);

inline t_buffer_return buffer_free(t_buffer *buffer);

#endif //SW_SYSTEM_BUFFER_H
