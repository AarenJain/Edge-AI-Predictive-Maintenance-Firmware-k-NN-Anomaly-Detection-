#ifndef RTOS_SIMULATOR_H
#define RTOS_SIMULATOR_H

#include <stdint.h>
#include <stdbool.h>

// Mocking FreeRTOS Types
typedef void *QueueHandle_t;
typedef uint32_t TickType_t;

#define pdMS_TO_TICKS(ms) (ms)
#define portMAX_DELAY 0xFFFF

// Function Prototypes representing the RTOS Kernel Core
QueueHandle_t xQueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize);
bool xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue, uint32_t xTicksToWait);
bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, uint32_t xTicksToWait);
uint32_t xTaskGetTickCount(void);
void vTaskDelayUntil(uint32_t *const pxPreviousWakeTime, const uint32_t xTimeIncrement);
void xTaskCreate(void (*pxTaskCode)(void *), const char *const pcName, uint16_t usStackDepth, void *pvParameters, uint32_t uxPriority, void **pxCreatedTask);
void vTaskStartScheduler(void);

#endif // RTOS_SIMULATOR_H
