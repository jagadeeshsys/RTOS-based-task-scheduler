cmake_minimum_required(VERSION 3.15)

project(RTOS_Task_Scheduler C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(FREERTOS_KERNEL_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/FreeRTOS-Kernel"
)

set(FREERTOS_PORT GCC_POSIX CACHE STRING "FreeRTOS port" FORCE)
set(FREERTOS_HEAP 4 CACHE STRING "FreeRTOS heap" FORCE)

add_library(freertos_config INTERFACE)

target_include_directories(freertos_config
    SYSTEM INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/Config"
)

find_package(Threads REQUIRED)

add_subdirectory(
    "${FREERTOS_KERNEL_PATH}"
    FreeRTOS-Kernel
)

add_executable(rtos_scheduler
    Application/main.c
    Application/tasks/sensor_task.c
    Application/tasks/uart_task.c
    Application/tasks/led_task.c
    Application/tasks/scheduling_demo.c
    Application/tasks/task_monitor.c
    Application/tasks/priority_inversion.c
    Application/tasks/cpu_monitor.c
)

target_include_directories(rtos_scheduler
    PRIVATE
    Application/include
)

target_link_libraries(rtos_scheduler
    PRIVATE
    freertos_kernel
    freertos_config
    Threads::Threads
)
target_link_libraries(rtos_scheduler
    PRIVATE
    freertos_kernel
    freertos_config
    Threads::Threads
)
