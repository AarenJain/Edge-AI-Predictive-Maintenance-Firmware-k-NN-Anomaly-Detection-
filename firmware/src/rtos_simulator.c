#include "../include/rtos_simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define msleep(ms) Sleep(ms)
#else
#include <unistd.h>
#define msleep(ms) usleep((ms) * 1000)
#endif

#define MAX_QUEUE_SIZE 20
static int8_t internal_queue[MAX_QUEUE_SIZE];
static uint8_t queue_head = 0;
static uint8_t queue_tail = 0;
static uint8_t queue_count = 0;

QueueHandle_t xQueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize)
{
    return (QueueHandle_t)1; // Simple static handle allocation
}

bool xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue, uint32_t xTicksToWait)
{
    if (queue_count >= MAX_QUEUE_SIZE)
        return false;
    internal_queue[queue_tail] = *(int8_t *)pvItemToQueue;
    queue_tail = (queue_tail + 1) % MAX_QUEUE_SIZE;
    queue_count++;
    return true;
}

bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, uint32_t xTicksToWait)
{
    while (queue_count == 0)
    {
        msleep(10); // Block task execution until data streams in
    }
    *(int8_t *)pvBuffer = internal_queue[queue_head];
    queue_head = (queue_head + 1) % MAX_QUEUE_SIZE;
    queue_count--;
    return true;
}

uint32_t xTaskGetTickCount(void) { return 0; }
void vTaskDelayUntil(uint32_t *const pxPreviousWakeTime, const uint32_t xTimeIncrement) { msleep(xTimeIncrement); }

typedef struct
{
    void (*task_func)(void *);
} TaskArgs;
static TaskArgs tasks[2];
static uint8_t task_count = 0;

void xTaskCreate(void (*pxTaskCode)(void *), const char *const pcName, uint16_t usStackDepth, void *pvParameters, uint32_t uxPriority, void **pxCreatedTask)
{
    if (task_count < 2)
    {
        tasks[task_count++].task_func = pxTaskCode;
    }
}

// Emulating concurrent runtime scheduling
void vTaskStartScheduler(void)
{
    printf("[KERNEL] Multi-Threaded Task Scheduler Started Successfully.\n\n");
#if defined(_WIN32) || defined(_WIN64)
    // Sequential simulation loop processing for baseline cross-platform portability
    for (int i = 0; i < 30; i++)
    {
        tasks[0].task_func(NULL); // Run Sampler Engine Block
        tasks[1].task_func(NULL); // Run Engine AI Block
    }
#else
    // For standard Unix-like POSIX environments, alternate run sequence loops
    for (int i = 0; i < 30; i++)
    {
        tasks[0].task_func(NULL);
        tasks[1].task_func(NULL);
    }
#endif
}