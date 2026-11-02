#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "task_monitor.h"
#include "app_sync.h"

#define MONITORED_TASK_COUNT 10

static const char *taskNames[MONITORED_TASK_COUNT] =
{
    "Sensor",
    "UART",
    "LED",
    "HighDemo",
    "LowDemo",
    "Monitor",
    "CPUMonitor",
    "PI_Low",
    "PI_Medium",
    "PI_High"
};

static const char *pcTaskStateName(eTaskState state)
{
    switch (state)
    {
        case eRunning:   return "RUNNING";
        case eReady:     return "READY";
        case eBlocked:   return "BLOCKED";
        case eSuspended: return "SUSPENDED";
        case eDeleted:   return "DELETED";
        default:         return "UNKNOWN";
    }
}

void vTaskMonitor(void *pvParameters)
{
    (void)pvParameters;

    TaskHandle_t taskHandles[MONITORED_TASK_COUNT];
    UBaseType_t stackRemaining[MONITORED_TASK_COUNT];

    char taskListBuffer[configSTATS_BUFFER_MAX_LENGTH];

    for (;;)
    {
        for (UBaseType_t i = 0; i < MONITORED_TASK_COUNT; i++)
        {
            taskHandles[i] = xTaskGetHandle(taskNames[i]);

            if (taskHandles[i] != NULL)
            {
                stackRemaining[i] =
                    uxTaskGetStackHighWaterMark(taskHandles[i]);
            }
            else
            {
                stackRemaining[i] = 0;
            }
        }

        if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
        {
            printf("\n========== TASK MONITOR ==========\n");

            vTaskList(taskListBuffer);
            printf("%s", taskListBuffer);

            for (UBaseType_t i = 0; i < MONITORED_TASK_COUNT; i++)
            {
                if (taskHandles[i] != NULL)
                {
                    eTaskState state =
                        eTaskGetState(taskHandles[i]);

                    printf("[Monitor] %-10s | State: %-9s | Stack remaining: %lu stack units\n",
                           taskNames[i],
                           pcTaskStateName(state),
                           (unsigned long)stackRemaining[i]);
                }
                else
                {
                    printf("[Monitor] %-10s | Task not found\n",
                           taskNames[i]);
                }
            }

            printf("==================================\n");

            xSemaphoreGive(xPrintMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}