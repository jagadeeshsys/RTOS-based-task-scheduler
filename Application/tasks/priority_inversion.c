
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "priority_inversion.h"
#include "app_sync.h"

/*
 * Set to 1: Mutex with priority inheritance
 * Set to 0: Binary semaphore without priority inheritance
 */
#define PRIORITY_INVERSION_USE_MUTEX    1

/* Task priorities */
#define PI_LOW_PRIORITY       (tskIDLE_PRIORITY + 1)
#define PI_MEDIUM_PRIORITY    (tskIDLE_PRIORITY + 2)
#define PI_HIGH_PRIORITY      (tskIDLE_PRIORITY + 3)

/* Shared resource */
static SemaphoreHandle_t xSharedResource;

/* Synchronization semaphores */
static SemaphoreHandle_t xLowHasResource;
static SemaphoreHandle_t xStartMedium;

/* Low-priority task */
static void vPI_LowTask(void *pvParameters)
{
    (void) pvParameters;

    volatile unsigned long counter;

    if (xSemaphoreTake(xSharedResource, portMAX_DELAY) == pdTRUE)
    {
        if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
        {
            printf("\n[LOW] Shared resource acquired\n");
            printf("[LOW] Starting CPU-intensive work\n");

            xSemaphoreGive(xPrintMutex);
        }

        /* Inform HighPriorityTask that the resource is locked */
        xSemaphoreGive(xLowHasResource);

        /* Simulate CPU-intensive work */
        for (counter = 0; counter < 100000000UL; counter++)
        {
            /* Busy work */
        }

        if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
        {
            printf("[LOW] CPU work completed\n");
            printf("[LOW] Releasing shared resource\n");

            xSemaphoreGive(xPrintMutex);
        }

        xSemaphoreGive(xSharedResource);
    }

    vTaskDelete(NULL);
}

/* Medium-priority task */
static void vPI_MediumTask(void *pvParameters)
{
    (void) pvParameters;

    volatile unsigned long counter;

    /* Wait until HighPriorityTask activates this task */
    xSemaphoreTake(xStartMedium, portMAX_DELAY);

    if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
    {
        printf("[MEDIUM] CPU-intensive task started\n");

        xSemaphoreGive(xPrintMutex);
    }

    /* Simulate CPU-intensive work */
    for (counter = 0; counter < 200000000UL; counter++)
    {
        /* Busy work */
    }

    if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
    {
        printf("[MEDIUM] CPU-intensive task completed\n");

        xSemaphoreGive(xPrintMutex);
    }

    vTaskDelete(NULL);
}

/* High-priority task */
static void vPI_HighTask(void *pvParameters)
{
    (void) pvParameters;

    /* Wait until LowPriorityTask acquires the resource */
    xSemaphoreTake(xLowHasResource, portMAX_DELAY);

    if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
    {
        printf("[HIGH] Attempting to acquire shared resource\n");

        xSemaphoreGive(xPrintMutex);
    }

    /* Activate medium-priority task */
    xSemaphoreGive(xStartMedium);

    /* This blocks while LowPriorityTask owns the resource */
    xSemaphoreTake(xSharedResource, portMAX_DELAY);

    if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE)
    {
        printf("[HIGH] Shared resource acquired\n");
        printf("[HIGH] Executing critical operation\n");

        xSemaphoreGive(xPrintMutex);
    }

    xSemaphoreGive(xSharedResource);

    vTaskDelete(NULL);
}

/* Create synchronization objects and tasks */
BaseType_t xStartPriorityInversionDemo(void)
{
#if PRIORITY_INVERSION_USE_MUTEX

    xSharedResource = xSemaphoreCreateMutex();

#else

    xSharedResource = xSemaphoreCreateBinary();

    if (xSharedResource != NULL)
    {
        /* Binary semaphore must initially be available */
        xSemaphoreGive(xSharedResource);
    }

#endif

    xLowHasResource = xSemaphoreCreateBinary();
    xStartMedium = xSemaphoreCreateBinary();

    if ((xSharedResource == NULL) ||
        (xLowHasResource == NULL) ||
        (xStartMedium == NULL))
    {
        return pdFAIL;
    }

    /* Create high-priority task */
    if (xTaskCreate(vPI_HighTask,
                    "PI_High",
                    configMINIMAL_STACK_SIZE * 2,
                    NULL,
                    PI_HIGH_PRIORITY,
                    NULL) != pdPASS)
    {
        return pdFAIL;
    }

    /* Create medium-priority task */
    if (xTaskCreate(vPI_MediumTask,
                    "PI_Medium",
                    configMINIMAL_STACK_SIZE * 2,
                    NULL,
                    PI_MEDIUM_PRIORITY,
                    NULL) != pdPASS)
    {
        return pdFAIL;
    }

    /* Create low-priority task */
    if (xTaskCreate(vPI_LowTask,
                    "PI_Low",
                    configMINIMAL_STACK_SIZE * 2,
                    NULL,
                    PI_LOW_PRIORITY,
                    NULL) != pdPASS)
    {
        return pdFAIL;
    }

    return pdPASS;
}
