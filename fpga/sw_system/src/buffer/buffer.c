
/* Standard C includes */
#include <stdlib.h>

/* BSP and Xilinx Includes */

/* Project includes */
#include "buffer.h"

inline t_buffer_return buffer_create(t_buffer *buffer, size_t size) {
    /* Vars */
    t_buffer_return ret = BUFFER_RETURN_OK;

    /* Check NULL pointer */
    if (buffer == NULL) {
        ret = BUFFER_RETURN_NULL_POINTER;
    } else {
        /* Set default structure */
        buffer->ptr = (uint8_t *) malloc(size + 1);
        buffer->read_ptr = 0;
        buffer->write_ptr = 0;
        buffer->size = size;

        /* Check if memory allocation has NOT given the desired memory */
        if (buffer->ptr == NULL) {
            buffer->error = BUFFER_RETURN_FAIL_ALLOCATION;
            ret = buffer->error;
        }
    }

    /* Return */
    return ret;
}

inline size_t buffer_available(t_buffer *buffer) {
    /* Default return error */
    size_t n;

    if (buffer->write_ptr >= buffer->read_ptr) {
        n = (size_t) (buffer->write_ptr - buffer->read_ptr);
    } else {
        n = (size_t) ((buffer->write_ptr + buffer->size) - buffer->read_ptr);
    }

    /* return count */
    return n;
}

inline t_buffer_return buffer_read_byte(t_buffer *buffer, uint8_t *byte) {
    size_t n = buffer_available(buffer);
    t_buffer_return ret = BUFFER_RETURN_OK;

    if (n > 0) {
        /* read byte */
        *byte = buffer->ptr[buffer->read_ptr];

        /* Increase read byte pointer */
        buffer->read_ptr++;

        /* Rollover pointer */
        if (buffer->read_ptr >= (buffer->size + 1)) {
            buffer->read_ptr = 0;
        }
    } else {
        buffer->error = BUFFER_RETURN_READ_EMPTY_BUFFER;
        ret = buffer->error;
    }

    return ret;
}

inline t_buffer_return buffer_write_byte(t_buffer *buffer, uint8_t *byte) {
    size_t n = buffer_available(buffer);
    t_buffer_return ret = BUFFER_RETURN_OK;

    if (buffer == NULL) {
        ret = BUFFER_RETURN_NULL_POINTER;
    } else if (buffer->ptr == NULL) {
        buffer->error = BUFFER_RETURN_NULL_POINTER;
        ret = buffer->error;
    } else if (n < buffer->size) {
        /* read byte */
        buffer->ptr[buffer->write_ptr] = *byte;

        /* Increase read byte pointer */
        buffer->write_ptr++;

        /* Rollover pointer */
        if (buffer->write_ptr >= (buffer->size + 1)) {
            buffer->write_ptr = 0;
        }
    } else {
        buffer->error = BUFFER_RETURN_WRITE_FULL_BUFFER;
        ret = buffer->error;
    }

    return ret;
}

t_buffer_return buffer_write_string(t_buffer *buffer, uint8_t *str) {
    t_buffer_return ret = BUFFER_RETURN_OK;

    if (buffer == NULL) {
        ret = BUFFER_RETURN_NULL_POINTER;
    }

    while ((*str != 0) && (ret == BUFFER_RETURN_OK)) {
        buffer_write_byte(buffer, str);
        /* Increment pointer */
        str++;
    }

    return ret;
}

inline t_buffer_return buffer_free(t_buffer *buffer) {
    t_buffer_return ret;

    if (buffer->ptr == NULL) {
        buffer->error = BUFFER_RETURN_NULL_POINTER;
        ret = buffer->error;
    } else {
        /* Free memory */
        free(buffer->ptr);

        /* Setup defaults */
        buffer->ptr = NULL;
        buffer->size = 0;
        buffer->error = BUFFER_RETURN_OK;
        buffer->write_ptr = 0;
        buffer->read_ptr = 0;
    }

    return ret;
}