
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "scheduling_demo.h"

void vLowPriorityTask(void *pvParameters)
{
    (void) pvParameters;

    volatile unsigned long counter;

    printf("[LOW] Low-priority task started\n");

    for (;;)
    {
        /* Simulate CPU-intensive work */
        for (counter = 0; counter < 100000000UL; counter++)
        {
            /* Busy work */
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void vHighPriorityTask(void *pvParameters)
{
    (void) pvParameters;

    /* Wait for 2 seconds before becoming ready */
    vTaskDelay(pdMS_TO_TICKS(2000));

    for (;;)
    {
        printf("[HIGH] High-priority task is running\n");

        /* Block for 2 seconds */
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
