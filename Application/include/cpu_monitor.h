#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

#include "FreeRTOS.h"

/* CPU utilization monitoring task */
void vCpuMonitorTask(void *pvParameters);

#endif