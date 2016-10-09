
#ifndef SW_SYSTEM_BUFFER_H
#define SW_SYSTEM_BUFFER_H

#include <inttypes.h>

typedef enum {
    BUFFER_ERROR_NOT_OK = -1,
    BUFFER_ERROR_FAIL_ALLOCATION = -2,
    BUFFER_ERROR_READ_EMPTY_BUFFER = -4,
    BUFFER_ERROR_WRITE_FULL_BUFFER = -5,
    BUFFER_ERROR_NULL_POINTER = -6,
    BUFFER_ERROR_ALREADY_ALLOCATED = -7,
    BUFFER_ERROR_FUNCTION_NOT_SUPPORTED = -8,
    BUFFER_ERROR_OK = 0
} t_buffer_return;

typedef struct {
    uint8_t *ptr;
    uint32_t write_ptr;
    uint32_t read_ptr;
    uint32_t size;
    t_buffer_return error;
} t_buffer;

void buffer_print_error (const char *func_name, t_buffer *buffer);

inline t_buffer_return buffer_create(t_buffer *buffer, size_t size);

inline size_t buffer_available(t_buffer *buffer);

inline t_buffer_return buffer_read_byte(t_buffer *buffer, uint8_t *byte);

size_t buffer_read(t_buffer *buffer, uint8_t *data, size_t max_size);

inline t_buffer_return buffer_write_byte(t_buffer *buffer, uint8_t *byte);

t_buffer_return buffer_write_string(t_buffer *buffer, uint8_t *str);

#endif //SW_SYSTEM_BUFFER_H
