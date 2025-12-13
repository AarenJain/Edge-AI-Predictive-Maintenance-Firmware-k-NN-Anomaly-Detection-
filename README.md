# Embedded_C_WiFi_Noise_Predictive_Maintenance

## 💡 Project Overview: Edge AI for Industrial Predictive Maintenance

This project demonstrates a low-cost, resource-efficient Edge AI solution for monitoring industrial equipment health using an existing wireless network (Wi-Fi). It uses the **Embedded C** language, making it ideal for microcontrollers like the ESP32, without relying on complex, resource-heavy frameworks.

### Goal

To detect early signs of machine failure (e.g., motor bearing wear) by identifying **anomalous spikes in the ambient Wi-Fi Channel Noise Floor (CNF)** caused by electromagnetic interference (EMI).

### Core Technology: k-Nearest Neighbors (k-NN) Anomaly Detection

Instead of complex deep learning, we use a simple, pure C-implemented k-NN algorithm for anomaly detection.

1.  **Feature Vector:** The system collects a short time-series of **CNF readings (dBm)**, which is the input feature vector (e.g., 5 sequential readings).
2.  **Reference Set:** A memory of known "Normal" CNF feature vectors is stored in program memory (`k_nn_reference_set`).
3.  **Anomaly Score:** The k-NN function calculates the **Minimum Distance Anomaly Score**—the smallest Euclidean distance between the new input and any point in the "Normal" reference set. A high distance indicates an anomaly. 

### 📊 Quantifiable Results & Threshold Justification

The system's performance is governed by the **Anomaly Threshold** (`1.0f`).

* **Quantification Method:** The threshold was set by finding the **Maximum Anomaly Score recorded during healthy operation** (e.g., 0.50) and adding a **Safety Buffer ($\Delta$)** (e.g., 0.50).
* **Decision Rule:** If the **Anomaly Score** > **1.0**, a predictive maintenance alert is triggered.

---
   
