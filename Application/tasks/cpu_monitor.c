#include <stdio.h>
#include <sys/resource.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"

#include "cpu_monitor.h"
#include "app_sync.h"

/* Convert timeval to seconds */
static double getCpuTimeSeconds(struct timeval time)
{
    return (double)time.tv_sec +
           ((double)time.tv_usec / 1000000.0);
}

/* Get elapsed wall-clock time */
static double getWallTimeSeconds(void)
{
    struct timespec time;

    clock_gettime(CLOCK_MONOTONIC, &time);

    return (double)time.tv_sec +
           ((double)time.tv_nsec / 1000000000.0);
}

void vCpuMonitorTask(void *pvParameters)
{
    (void)pvParameters;

    struct rusage usage;

    double previousCpuTime;
    double currentCpuTime;

    double previousWallTime;
    double currentWallTime;

    double cpuUsage;
    double machineUsage;

    const double logicalCpuCount = 10.0;

    /* Capture initial CPU and wall-clock measurements */
    getrusage(RUSAGE_SELF, &usage);

    previousCpuTime =
        getCpuTimeSeconds(usage.ru_utime) +
        getCpuTimeSeconds(usage.ru_stime);

    previousWallTime = getWallTimeSeconds();

    printf("[CPU Monitor] CPU monitoring started\n");

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));

        if (getrusage(RUSAGE_SELF, &usage) != 0)
        {
            printf("[CPU Monitor] Failed to read CPU usage\n");
            continue;
        }

        currentCpuTime =
            getCpuTimeSeconds(usage.ru_utime) +
            getCpuTimeSeconds(usage.ru_stime);

        currentWallTime = getWallTimeSeconds();

        cpuUsage =
            ((currentCpuTime - previousCpuTime) /
             (currentWallTime - previousWallTime)) * 100.0;

        machineUsage = cpuUsage / logicalCpuCount;

        if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
        {
            printf("\n========== CPU UTILIZATION ==========\n");
            printf("Process CPU usage : %.2f%%\n", cpuUsage);
            printf("Machine capacity  : %.2f%%\n", machineUsage);
            printf("Logical CPUs      : %.0f\n", logicalCpuCount);
            printf("=====================================\n");

            xSemaphoreGive(xPrintMutex);
        }

        previousCpuTime = currentCpuTime;
        previousWallTime = currentWallTime;
    }
}