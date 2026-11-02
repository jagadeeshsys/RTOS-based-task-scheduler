
#include <stdio.h>

#include "app_tasks.h"
#include "app_queue.h"
#include "app_sync.h"

void vUARTTask(void *pvParameters)
{
    (void) pvParameters;

    SensorData_t receivedData;

    for (;;)
    {
        /* Wait for temperature data from SensorTask */
        if (xQueueReceive(xSensorQueue,
                          &receivedData,
                          portMAX_DELAY) == pdPASS)
        {
            /* Protect shared terminal output */
            if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
            {
                printf("[UARTTask] Received Temperature: %.2f C\n",
                       receivedData.temperature);

                xSemaphoreGive(xPrintMutex);
            }
        }
    }
}
