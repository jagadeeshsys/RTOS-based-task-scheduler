#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "app_tasks.h"
#include "app_queue.h"
#include "app_sync.h"
#include "scheduling_demo.h"
#include "task_monitor.h"
#include "priority_inversion.h"
#include "cpu_monitor.h"

/* Global RTOS objects */
QueueHandle_t xSensorQueue = NULL;

SemaphoreHandle_t xTemperatureAlertSemaphore = NULL;

SemaphoreHandle_t xPrintMutex = NULL;

/* Application task priorities */
#define SENSOR_TASK_PRIORITY       (tskIDLE_PRIORITY + 2)
#define UART_TASK_PRIORITY         (tskIDLE_PRIORITY + 2)
#define LED_TASK_PRIORITY          (tskIDLE_PRIORITY + 1)

/* Scheduling demo priorities */
#define HIGH_DEMO_PRIORITY         (tskIDLE_PRIORITY + 3)
#define LOW_DEMO_PRIORITY          (tskIDLE_PRIORITY + 1)

/* Monitoring task priorities */
#define TASK_MONITOR_PRIORITY      (tskIDLE_PRIORITY + 1)
#define CPU_MONITOR_TASK_PRIORITY  (tskIDLE_PRIORITY + 1)


int main(void)
{
    printf("\n====================================\n");
    printf(" RTOS-Based Task Scheduler\n");
    printf(" Starting FreeRTOS application tasks\n");
    printf("====================================\n");

    /* Create sensor queue */
    xSensorQueue = xQueueCreate(5, sizeof(SensorData_t));

    if (xSensorQueue == NULL)
    {
        printf("ERROR: Sensor queue creation failed\n");
        return 1;
    }

    printf("Sensor queue created successfully.\n");

    /* Create temperature alert binary semaphore */
    xTemperatureAlertSemaphore = xSemaphoreCreateBinary();

    if (xTemperatureAlertSemaphore == NULL)
    {
        printf("ERROR: Semaphore creation failed\n");
        return 1;
    }

    printf("Temperature alert semaphore created successfully.\n");

    /* Create print mutex */
    xPrintMutex = xSemaphoreCreateMutex();

    if (xPrintMutex == NULL)
    {
        printf("ERROR: Print mutex creation failed\n");
        return 1;
    }

    printf("Print mutex created successfully.\n");


    /* Create Sensor Task */
    if (xTaskCreate(vSensorTask,
                    "Sensor",
                    configMINIMAL_STACK_SIZE * 2,
                    NULL,
                    SENSOR_TASK_PRIORITY,
                    NULL) != pdPASS)
    {
        printf("SensorTask creation failed\n");
        return 1;
    }


    /* Create UART Task */
    if (xTaskCreate(vUARTTask,
                    "UART",
                    configMINIMAL_STACK_SIZE * 2,
                    NULL,
                    UART_TASK_PRIORITY,
                    NULL) != pdPASS)
    {
        printf("UARTTask creation failed\n");
        return 1;
    }


    /* Create LED Task */
    if (xTaskCreate(vLEDTask,
                    "LED",
                    configMINIMAL_STACK_SIZE * 2,
                    NULL,
                    LED_TASK_PRIORITY,
                    NULL) != pdPASS)
    {
        printf("LEDTask creation failed\n");
        return 1;
    }


    /* Create High-Priority Scheduling Demo Task */
    if (xTaskCreate(vHighPriorityTask,
                    "HighDemo",
                    configMINIMAL_STACK_SIZE * 2,
                    NULL,
                    HIGH_DEMO_PRIORITY,
                    NULL) != pdPASS)
    {
        printf("HighPriorityTask creation failed\n");
        return 1;
    }


    /* Create Low-Priority Scheduling Demo Task */
    if (xTaskCreate(vLowPriorityTask,
                    "LowDemo",
                    configMINIMAL_STACK_SIZE * 2,
                    NULL,
                    LOW_DEMO_PRIORITY,
                    NULL) != pdPASS)
    {
        printf("LowPriorityTask creation failed\n");
        return 1;
    }

    printf("Scheduling demo tasks created successfully.\n");


    /* Create Task Monitor */
    if (xTaskCreate(vTaskMonitor,
                    "Monitor",
                    configMINIMAL_STACK_SIZE * 8,
                    NULL,
                    TASK_MONITOR_PRIORITY,
                    NULL) != pdPASS)
    {
        printf("TaskMonitor creation failed\n");
        return 1;
    }

    printf("TaskMonitor created successfully.\n");


    /* Create CPU Monitor */
    if (xTaskCreate(vCpuMonitorTask,
                    "CPUMonitor",
                    configMINIMAL_STACK_SIZE * 2,
                    NULL,
                    CPU_MONITOR_TASK_PRIORITY,
                    NULL) != pdPASS)
    {
        printf("CPUMonitor creation failed\n");
        return 1;
    }

    printf("CPU Monitor task created successfully.\n");


    /* Create Priority Inversion Demo Tasks */
    if (xStartPriorityInversionDemo() != pdPASS)
    {
        printf("Priority inversion demo creation failed\n");
        return 1;
    }

    printf("Priority inversion demo created successfully.\n");

    printf("All application tasks created successfully.\n");


    /* Start FreeRTOS Scheduler */
    vTaskStartScheduler();

    printf("ERROR: Scheduler returned.\n");

    return 1;
}


/* FreeRTOS Stack Overflow Hook */
void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   char *pcTaskName)
{
    (void)xTask;

    printf("ERROR: Stack overflow in task: %s\n", pcTaskName);

    for (;;)
    {
    }
}