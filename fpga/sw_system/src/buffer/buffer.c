
/* Standard C includes */
#include <stdio.h>
#include <stdlib.h>

/* BSP and Xilinx Includes */

/* Project includes */
#include "buffer.h"
#include "wifi_cfg.h"

static uint8_t buffer_memory[BUFFER_MAX_MEMORY];
static size_t buffer_memory_allocated = 0;

static size_t buffer_memory_free_bytes(void) {
    return BUFFER_MAX_MEMORY - buffer_memory_allocated;
}

void buffer_print_error (const char *func_name, t_buffer *buffer) {
#define PRINT_ERROR(e) case e: xil_printf("[%s] %s\r\n", func_name, #e); break
#define PRINT_ERROR_DEFAULT(e) default: xil_printf("[%s] %s\r\n", func_name, #e)
    switch(buffer->error) {
        PRINT_ERROR(BUFFER_ERROR_NOT_OK);
        PRINT_ERROR(BUFFER_ERROR_FAIL_ALLOCATION);
        PRINT_ERROR(BUFFER_ERROR_READ_EMPTY_BUFFER);
        PRINT_ERROR(BUFFER_ERROR_WRITE_FULL_BUFFER);
        PRINT_ERROR(BUFFER_ERROR_NULL_POINTER);
        PRINT_ERROR(BUFFER_ERROR_ALREADY_ALLOCATED);
        PRINT_ERROR(BUFFER_ERROR_FUNCTION_NOT_SUPPORTED);
        PRINT_ERROR_DEFAULT(BUFFER_ERROR_OK);
    }
#undef PRINT_ERROR
#undef PRINT_ERROR_DEFAULT
}

inline t_buffer_return buffer_create(t_buffer *buffer, size_t size) {
    /* Vars */
    t_buffer_return ret = BUFFER_ERROR_OK;

    /* Check NULL pointer */
    if (buffer == NULL) {
        ret = BUFFER_ERROR_NULL_POINTER;
    }else if (buffer->ptr != NULL) {
        ret = BUFFER_ERROR_ALREADY_ALLOCATED;
    } else if ((buffer_memory_allocated + size) > BUFFER_MAX_MEMORY) {
#if BUFFER_DEBUG_TRACES == 1
        xil_printf("[%s] ERROR! Impossible to allocate 0x%X bytes (0x%X bytes available).\r\n",
                   __FUNCTION__, size, buffer_memory_free_bytes());
#endif /* BUFFER_DEBUG_TRACES == 1 */
        ret = BUFFER_ERROR_FAIL_ALLOCATION;
    } else {
        /* Set default structure */
        buffer->ptr = buffer_memory + buffer_memory_allocated;
        buffer->read_ptr = 0;
        buffer->write_ptr = 0;
        buffer->size = size;

        /* Increase buffer_memory_allocated */
        buffer_memory_allocated += size;

#if BUFFER_DEBUG_TRACES == 1
        xil_printf("[%s] Allocated 0x%X bytes in 0x%X.\r\n", __FUNCTION__, size, buffer->ptr);
#endif /* BUFFER_DEBUG_TRACES == 1 */
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

    if (n > 0) {
        /* read byte */
        *byte = buffer->ptr[buffer->read_ptr];

        /* Increase read byte pointer */
        buffer->read_ptr++;

        /* Rollover pointer */
        if (buffer->read_ptr >= (buffer->size + 1)) {
            buffer->read_ptr = 0;
        }

        buffer->error = BUFFER_ERROR_OK;
    } else {
        buffer->error = BUFFER_ERROR_READ_EMPTY_BUFFER;
    }

    return buffer->error;
}

size_t buffer_read(t_buffer *buffer, uint8_t *data, size_t max_size) {
    size_t n = 0;
    t_buffer_return err = BUFFER_ERROR_OK;

    if (buffer == NULL) {
        n = 0;
    } else {
        while ((n < max_size) && (err == BUFFER_ERROR_OK)) {
            err = buffer_read_byte(buffer, data++);
            n++;
        }
    }

    return n;
}

inline t_buffer_return buffer_write_byte(t_buffer *buffer, uint8_t *byte) {
    size_t n = buffer_available(buffer);
    t_buffer_return ret = BUFFER_ERROR_OK;

    if (buffer == NULL) {
        ret = BUFFER_ERROR_NULL_POINTER;
    } else if (buffer->ptr == NULL) {
        buffer->error = BUFFER_ERROR_NULL_POINTER;
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
#if BUFFER_DEBUG_TRACES == 1
        xil_printf("[%s] The buffer is full (0x%X bytes)\r\n", __FUNCTION__, buffer->size);
#endif /* BUFFER_DEBUG_TRACES == 1 */
        buffer->error = BUFFER_ERROR_WRITE_FULL_BUFFER;
        ret = buffer->error;
    }

    return ret;
}

t_buffer_return buffer_write_string(t_buffer *buffer, uint8_t *str) {
    t_buffer_return ret = BUFFER_ERROR_OK;

    if (buffer == NULL) {
        ret = BUFFER_ERROR_NULL_POINTER;
    }

    while ((*str != 0) && (ret == BUFFER_ERROR_OK)) {
        buffer_write_byte(buffer, str);
        /* Increment pointer */
        str++;
    }

    return ret;
}
