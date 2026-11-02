
#ifndef APP_TASKS_H
#define APP_TASKS_H

#include "FreeRTOS.h"
#include "task.h"

void vSensorTask(void *pvParameters);
void vUARTTask(void *pvParameters);
void vLEDTask(void *pvParameters);

#endif
