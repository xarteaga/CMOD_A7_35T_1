
/* Standard C Includes */
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* Xilinx Includes */
#include "xintc.h"

/* Project Includes */
#include "scheduler.h"

/* Variables */
extern XIntc platform_intc;
static uint32_t timestamp = 0;
static scheduler_entry_t *scheduler_entries [SCHEDULER_MAX_ENTRIES];
static uint32_t scheduler_entries_count;
static uint8_t scheduler_busy = TRUE;

void scheduler_interrupt_handler(void){
    uint32_t i = 0, elapsed = 0;

    /* Check if it is busy */
    if (scheduler_busy == FALSE) {
        /* Rise Busy */
        scheduler_busy = TRUE;

        /* For each entry */
        for (i = 0; i < scheduler_entries_count; i++) {
            /* Get entry */
            scheduler_entry_t *entry = scheduler_entries[i];

            /* Process */
            elapsed = (u32)(timestamp - entry->timestamp);
            if (elapsed >= entry->period) {
                /* Call callback if required */
                ((scheduler_callback)entry->callback)((u32)elapsed);

                /* Update timestamp */
                entry->timestamp = timestamp;
            }
        }

        /* Down Busy */
        scheduler_busy = FALSE;
    }

    /* Increase timestamp */
    timestamp++;
}

void scheduler_init(void){
    /* Set all entries to zero */
    memset(scheduler_entries, 0, sizeof(scheduler_entries));

    /* Set entry count to zero */
    scheduler_entries_count = 0;

    /* Reset Timestamp */
    timestamp = 0;

    /* Reset busy */
    scheduler_busy = FALSE;


    xil_printf("%32s ... OK\r\n", __func__);
}

void scheduler_add_entry(scheduler_entry_t *entry) {
    /* Check that the list has not reached its limit */
    if (scheduler_entries_count < SCHEDULER_MAX_ENTRIES) {
        /* Set new entry */
        scheduler_entries[scheduler_entries_count] = entry;

        /* Increment number of entries */
        scheduler_entries_count++;

        //xil_printf("%s: number of entries: %d.\r\n", __FUNCTION__, scheduler_entries_count);
    } else {
        xil_printf("[%s] List has reached its limit.\r\n", __FUNCTION__);
    }
}
