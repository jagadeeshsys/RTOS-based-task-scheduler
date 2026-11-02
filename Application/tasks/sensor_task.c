
#include <stdio.h>

#include "app_tasks.h"
#include "app_queue.h"
#include "app_sync.h"

void vSensorTask(void *pvParameters)
{
    (void) pvParameters;

    SensorData_t sensorData;
    float temperature = 25.0f;
    BaseType_t alertSent = pdFALSE;

    for (;;)
    {
        temperature += 0.5f;

        if (temperature > 35.0f)
        {
            temperature = 25.0f;
            alertSent = pdFALSE;
        }

        sensorData.temperature = temperature;

        /* Send temperature data to the queue */
        if (xQueueSend(xSensorQueue,
                       &sensorData,
                       pdMS_TO_TICKS(100)) == pdPASS)
        {
            if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
            {
                printf("[SensorTask] Temperature sent: %.2f C\n",
                       temperature);

                xSemaphoreGive(xPrintMutex);
            }
        }
        else
        {
            if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
            {
                printf("[SensorTask] Queue send failed\n");

                xSemaphoreGive(xPrintMutex);
            }
        }

        /* Generate temperature alert at 30 degrees */
        if (temperature >= 30.0f && alertSent == pdFALSE)
        {
            xSemaphoreGive(xTemperatureAlertSemaphore);

            if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
            {
                printf("[SensorTask] TEMPERATURE ALERT: %.2f C\n",
                       temperature);

                xSemaphoreGive(xPrintMutex);
            }

            alertSent = pdTRUE;
        }

        /* Wait for 2 seconds */
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
