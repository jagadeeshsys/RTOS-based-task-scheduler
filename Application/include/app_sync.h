
#ifndef APP_SYNC_H
#define APP_SYNC_H

#include "FreeRTOS.h"
#include "semphr.h"

/* Binary semaphore for temperature alert */
extern SemaphoreHandle_t xTemperatureAlertSemaphore;

/* Mutex for shared terminal output */
extern SemaphoreHandle_t xPrintMutex;

#endif
