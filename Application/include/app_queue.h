
#ifndef APP_QUEUE_H
#define APP_QUEUE_H

#include "FreeRTOS.h"
#include "queue.h"

/* Structure for sensor data */
typedef struct
{
    float temperature;
} SensorData_t;

/* Shared queue handle */
extern QueueHandle_t xSensorQueue;

#endif
