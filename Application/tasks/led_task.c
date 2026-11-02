#include <stdio.h>

#include "app_tasks.h"
#include "app_sync.h"

void vLEDTask(void *pvParameters)
{
    (void) pvParameters;

    for (;;)
    {
        /* Wait until SensorTask signals a temperature alert */
        if (xSemaphoreTake(xTemperatureAlertSemaphore,
                           portMAX_DELAY) == pdTRUE)
        {
            /* Protect shared terminal output */
            if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
            {
                printf("\n[LEDTask] !!! HIGH TEMPERATURE ALERT !!!\n");
                printf("[LEDTask] WARNING LED ON\n");

                xSemaphoreGive(xPrintMutex);
            }
        }
    }
}