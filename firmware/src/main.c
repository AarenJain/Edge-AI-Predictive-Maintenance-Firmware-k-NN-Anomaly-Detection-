#include "../include/rtos_simulator.h"
#include <stdio.h>

#define FEATURE_COUNT 5
#define K_NN_REF_SIZE 5
#define ANOMALY_THRESHOLD_SQ 1

QueueHandle_t xCnfQueue = NULL;

const int8_t k_nn_reference_set[K_NN_REF_SIZE][FEATURE_COUNT] = {
    {-92, -93, -92, -93, -92},
    {-92, -92, -92, -93, -92},
    {-92, -93, -92, -92, -92},
    {-93, -92, -93, -92, -92},
    {-92, -93, -92, -93, -92}};

int8_t HAL_Wifi_GetNoiseFloorRegister(void)
{
    static uint8_t sample_counter = 0;
    sample_counter++;
    if (sample_counter > 8)
    {
        return (int8_t)(-85); // Simulated Machine Defect Fault Spike
    }
    return (int8_t)(-92); // Stable State
}

static uint32_t raw_euclidean_distance_sq(const int8_t *a, const int8_t *b)
{
    uint32_t dist_sq = 0;
    for (uint8_t i = 0; i < FEATURE_COUNT; i++)
    {
        int16_t diff = (int16_t)a[i] - (int16_t)b[i];
        dist_sq += (uint32_t)(diff * diff);
    }
    return dist_sq;
}

void vTaskWifiSampler(void *pvParameters)
{
    int8_t raw_sample = HAL_Wifi_GetNoiseFloorRegister();
    xQueueSend(xCnfQueue, &raw_sample, 0);
}

void vTaskAnomalyInference(void *pvParameters)
{
    int8_t received_sample = 0;
    static int8_t sliding_window[FEATURE_COUNT] = {0};
    static uint8_t window_idx = 0;
    static bool window_ready = false;

    if (xQueueReceive(xCnfQueue, &received_sample, portMAX_DELAY) == true)
    {
        sliding_window[window_idx] = received_sample;
        window_idx++;

        if (window_idx >= FEATURE_COUNT)
        {
            window_idx = 0;
            window_ready = true;
        }

        if (window_ready)
        {
            uint32_t min_distance_sq = 0xFFFFFFFF;
            for (uint8_t k = 0; k < K_NN_REF_SIZE; k++)
            {
                uint32_t current_dist_sq = raw_euclidean_distance_sq(sliding_window, k_nn_reference_set[k]);
                if (current_dist_sq < min_distance_sq)
                {
                    min_distance_sq = current_dist_sq;
                }
            }

            if (min_distance_sq > ANOMALY_THRESHOLD_SQ)
            {
                printf("[ALERT] Anomaly Detected! Score Sq: %u (Maintenance Required)\n", min_distance_sq);
            }
            else
            {
                printf("[INFO] System Metrics Nominal. Score Sq: %u\n", min_distance_sq);
            }
        }
    }
}

int main(void)
{
    xCnfQueue = xQueueCreate(10, sizeof(int8_t));
    if (xCnfQueue == NULL)
        return -1;

    xTaskCreate(vTaskWifiSampler, "Wifi_Sampler", 2048, NULL, 3, NULL);
    xTaskCreate(vTaskAnomalyInference, "AI_Inference", 4096, NULL, 2, NULL);

    vTaskStartScheduler();
    return 0;
}
