#include <stdio.h>
#include <math.h>
#include <float.h> // For FLT_MAX


// --- 1. Project Parameters ---
#define FEATURE_COUNT       5     // Number of sequential CNF readings (time-series vector size)
#define K_NN_REF_SIZE       5     // Size of the Normal reference dataset
#define ANOMALY_THRESHOLD   1.0f  // Quantifiable threshold for anomaly detection

// --- 2. Simulated Embedded C Data (Reference Set) ---
/*
 * This array represents 5 different 'Normal' time-series vectors of Channel Noise Floor (CNF) in dBm.
 * In a real application, this data would be collected during a "healthy" calibration phase.
 */
const float k_nn_reference_set[K_NN_REF_SIZE][FEATURE_COUNT] = {
    // Normal Reference Vectors (CNF dBm over 5 time steps)
    {-92.1f, -92.5f, -92.2f, -92.6f, -92.0f}, // Sample 1
    {-92.4f, -92.0f, -92.3f, -92.5f, -92.1f}, // Sample 2
    {-92.2f, -92.3f, -92.1f, -92.4f, -92.0f}, // Sample 3
    {-92.5f, -92.1f, -92.4f, -92.0f, -92.2f}, // Sample 4
    {-92.0f, -92.4f, -92.2f, -92.5f, -92.1f}  // Sample 5
};

// --- 3. Simulated Test Inputs ---

// TEST A: Normal Operation (Very similar to the reference set)
const float test_normal_input[FEATURE_COUNT] = 
    {-92.3f, -92.2f, -92.4f, -92.1f, -92.5f}; 

// TEST B: Anomaly (Simulated high-EMI noise from a failing motor)
// Note the sustained increase in noise floor (e.g., from -92dBm to -85dBm)
const float test_anomaly_input[FEATURE_COUNT] = 
    {-86.0f, -85.5f, -86.2f, -85.9f, -85.7f}; 

// --- 4. Core Anomaly Detection Logic (k-NN in Pure C) ---

/**
 * @brief Calculates the squared Euclidean distance between two feature vectors.
 */
static float euclidean_distance_sq(const float *a, const float *b, int size) {
    float dist_sq = 0.0f;
    for (int i = 0; i < size; i++) {
        float diff = a[i] - b[i];
        dist_sq += diff * diff;
    }
    return dist_sq;
}

/**
 * @brief Runs k-NN to find the Minimum Distance Anomaly Score.
 * @return The minimum Euclidean distance to the 'Normal' reference set.
 */
float knn_run_anomaly_detection(const float *features) {
    float min_distance_sq = FLT_MAX; 

    // Find the nearest neighbor distance
    for (int k = 0; k < K_NN_REF_SIZE; k++) {
        float current_dist_sq = euclidean_distance_sq(
            features,
            k_nn_reference_set[k],
            FEATURE_COUNT
        );

        if (current_dist_sq < min_distance_sq) {
            min_distance_sq = current_dist_sq;
        }
    }

    // Return the minimum Euclidean distance (Anomaly Score)
    return sqrtf(min_distance_sq);
}

// --- 5. Main Execution and Presentation Function ---

void run_test_case(const float *test_sample, const char *sample_name) {
    float anomaly_score = knn_run_anomaly_detection(test_sample);
    const char *status;

    if (anomaly_score > ANOMALY_THRESHOLD) {
        status = "ANOMALY DETECTED (MAINTENANCE ALERT)";
    } else {
        status = "NORMAL OPERATION";
    }

    printf("| %-35s | %-15.4f | %-32s |\n", 
           sample_name, 
           anomaly_score, 
           status);
}

int main() {
    printf("\n");
    printf("=================================================================================\n");
    printf(" EDGE AI PREDICTIVE MAINTENANCE (EMBEDDED C k-NN) DEMO\n");
    printf("=================================================================================\n");
    printf(" Project Goal: Detect deviations in Wi-Fi Channel Noise Floor (CNF) time-series.\n");
    printf(" Anomaly Threshold: %.2f\n", ANOMALY_THRESHOLD);
    printf("---------------------------------------------------------------------------------\n");
    printf("| Test Sample Name                          | Anomaly Score   | Status                         |\n");
    printf("---------------------------------------------------------------------------------\n");
    
    // Run the two simulated test cases
    run_test_case(test_normal_input, "TEST A: Stable CNF (Healthy Machine)");
    run_test_case(test_anomaly_input, "TEST B: Sustained CNF Spike (Failing Machine)");

    printf("---------------------------------------------------------------------------------\n");
    
    return 0;
}
