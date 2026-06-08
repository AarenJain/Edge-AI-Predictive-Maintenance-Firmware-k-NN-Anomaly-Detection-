# Edge-AI Predictive Maintenance Firmware (k-NN Anomaly Detection)

This repository contains an optimized, resource-efficient Edge AI firmware module designed to detect industrial equipment faults. It monitors electromagnetic interference (EMI) by tracking shifts in the wireless Channel Noise Floor (CNF).

Targeted for constrained microcontrollers running FreeRTOS, this project demonstrates how to take a data-science prototype and refactor it into a thread-safe, real-time embedded application that runs completely without floating-point overhead.

---

## Architecture and Task Strategy

The system uses a standard Producer-Consumer design pattern managed by FreeRTOS primitives to keep tasks isolated and prevent CPU starvation:

- **Task A: Wifi_Sampler (Producer - High Priority):** Wakes up precisely every 200ms using `vTaskDelayUntil` to eliminate time jitter. It polls the hardware register and pushes raw telemetry data into an RTOS queue.
- **Task B: AI_Inference (Consumer - Lower Priority):** Blocks indefinitely on `xQueueReceive` using `portMAX_DELAY`, consuming zero CPU cycles until a new sample arrives. Once awakened, it copies the sample into a local sliding-window buffer and runs the k-NN logic.

---

## Key Firmware Optimizations

### Fixed-Point Math Conversion

To ensure the code runs efficiently on low-power 8, 16, or 32-bit MCUs without a hardware Floating Point Unit (FPU), all processing was converted from standard `float` to signed 8-bit integers (`int8_t`).

### Eliminating Square Roots

Standard Euclidean distance equations require a `sqrtf` call, which is computationally expensive on microcontrollers. Instead, this engine evaluates metrics in Squared Euclidean Space. By squaring the static alert threshold (`ANOMALY_THRESHOLD_SQ`), the firmware compares raw square products and avoids calling `math.h`.

### Stack Isolation

The sliding-window buffer is maintained entirely within the inference task's stack frame. This keeps data local, removes the need for global variables or complex mutexes, and protects the system from heap fragmentation.

---

## Technical Specifications

- **Time Complexity:** O(N \* K) where N = 5 features and K = 5 reference states.
- **Space Complexity:** O(1) auxiliary runtime memory footprint.
- **RAM Footprint:** Less than 128 bytes, entirely stack-allocated.
- **FPU Overhead:** None (100% integer arithmetic).

---

## Project Structure

```text
Embedded_C_Predictive_Maintenance/
├── firmware/
│   ├── src/
│   │   ├── main.c           # FreeRTOS application and core tasks
│   │   └── rtos_simulator.c # Abstraction layer emulating the FreeRTOS scheduler
│   └── include/
│       └── rtos_simulator.h # Mocked kernel types (Queues, Tasks, Delays)
├── simulation_prototype/
│   └── desktop_knn.c        # Original floating-point desktop prototype
└── README.md
```
