# CogniLog: On-Device Edge AI Productivity Assistant

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-F58220?style=for-the-badge&logo=PlatformIO&logoColor=white)
![TensorFlow Lite](https://img.shields.io/badge/TensorFlow_Lite-FF6F00?style=for-the-badge&logo=tensorflow&logoColor=white)
![MQTT](https://img.shields.io/badge/MQTT-660066?style=for-the-badge&logo=mqtt&logoColor=white)

## Overview

CogniLog is an embedded IoT assistant designed to track user focus and activity states entirely on the edge. By running inference directly on an ESP32 using TensorFlow Lite Micro, the system eliminates cloud dependency for real time processing, ensuring low latency and strict data privacy. An optional lightweight MQTT pipeline handles telemetry logging.

---

## Why CogniLog? (The Engineering Motivation)

Most productivity trackers rely on continuous cloud-side audio/visual processing, which introduces severe data privacy concerns, high latency, and massive power consumption over network interfaces. 

CogniLog was built from a hardware-first perspective: **What if we could run focus-state classification entirely on a sub-$5 microcontroller at the milliwatt level?** By bringing the processing directly to the edge, we completely eliminate cloud dependencies, minimize network bandwidth usage, and keep user data securely inside the local silicon.

---

## 🛠️ Engineering War Stories: Bugs & Breakthroughs

### 1. The I2S DMA Buffer vs. FreeRTOS Task Starvation Crisis
* **The Problem:** When initializing the microphone pipeline, the system frequently encountered audio stuttering and corrupted data frames. The raw data streaming from the microphone was missing packets, causing the TFLite Micro inference engine to classify inputs completely inaccurately.
* **The Root Cause:** The audio sampling task was competing for CPU cycles with the core background WiFi stack on the ESP32. Because the audio task wasn't retrieving data from the I2S hardware FIFO buffer fast enough, the buffer overflowed.
* **The Fix:** Shifted the architecture to utilize Direct Memory Access (DMA) buffers to handle background data transfers directly from peripheral to RAM without CPU intervention. I then isolated the critical sensor-ingestion pipeline into a high-priority FreeRTOS task pinned to Core 0, while relegating the non-deterministic WiFi/MQTT network telemetry to Core 1. 

### 2. The Network Stack Heap Collision
* **The Problem:** The firmware compiled successfully, but crashed with a `Guru Meditation Error: Core 1 panicked (LoadProhibited)` the exact moment the device attempted to connect to the MQTT broker while the TFLite interpreter was initializing.
* **The Root Cause:** The ESP32's network stack requires massive, unpredictable chunks of heap allocation during TLS handshake and WiFi initialization. Concurrently, the TFLite Micro interpreter was attempting to allocate its memory pool dynamically at runtime, causing a severe heap exhaustion and collision.
* **The Fix:** Switched to a strictly **hardware-aware memory strategy**. I calculated the maximum network overhead and explicitly defined a static byte array (`uint8_t tensor_arena[tensor_arena_size]`) for the TFLite interpreter at compile time. This forced the model’s memory footprint into a predictable, dedicated block, ensuring the runtime heap remained stable for network operations.

---

## 🧠 Key Lessons Learned

* **Memory Consciousness:** Every byte counts. Operating on the edge means understanding the trade-offs between model accuracy (INT8 quantization vs. FP32 precision) and the remaining volatile memory available for standard network stacks.
* **Concurrency Primitives:** Learned how to safely pass data across asynchronous execution contexts using thread-safe FreeRTOS queues without blocking real-time mathematical operations.
* **Hardware-Protocol Realities:** Gained deep respect for protocol timing, DMA buffer sizing, and understanding how peripherals interact with register files behind the scenes.

## System Architecture

> **Note:** See `assets/architecture.png` for the detailed hardware & software flow.

```mermaid
graph TD;
    A[Sensor Data / IMU] -->|I2S / SPI| B(ESP32 MCU)
    B -->|Buffer| C{TFLite Micro Inference}
    C -->|Activity State| D[Local Logic / Display]
    C -->|Telemetry| E[(MQTT Broker)]

    style B fill:#e7352c,stroke:#333,stroke-width:2px,color:#fff
    style C fill:#ff6f00,stroke:#333,stroke-width:2px,color:#fff
```

---

## Hardware & Firmware Specifications

| Parameter | Value |
|-----------|-------|
| Microcontroller | ESP32 (Xtensa Dual-Core 32-bit LX6 @ 240 MHz) |
| Framework | PlatformIO / ESP-IDF |
| Sensors | INMP441 |
| Sensor Protocol | I2S @ 400 kHz (fast-mode) |
| Telemetry Protocol | MQTT over WiFi |

---

## Edge AI Pipeline & Performance Metrics

The focus-tracking model was trained and quantized to fit within the strict memory constraints of the ESP32.
### (WIP-To be updated)

| Metric | Value |
|--------|-------|
| Model Type | DS-CNN |
| Quantization | INT8 |
| Inference Time | `[XX]` ms per frame |
| Flash Usage | `[XX]` KB |
| SRAM Peak (Arena) | `[XX]` KB — tuned to prevent heap exhaustion |
| Validation Accuracy | `[XX.X]`% |


---

## Memory Optimization Strategy

To fit the model alongside the WiFi/MQTT stack within the ESP32's 520 KB SRAM, the following memory conscious designs were implemented:

1. **Static tensor arena allocation** — prevents heap fragmentation by reserving a fixed block for the TFLite interpreter at startup.
2. **Direct sensor-to-buffer pipeline** — raw sensor reads written directly into the input tensor buffer, avoiding intermediate copies.

---

## Build & Flash Instructions

This project is built using PlatformIO.

**1. Clone the repository:**
```bash
git clone https://github.com/mohansudhandhiram-2k07/CogniLog.git
cd CogniLog
```

**2. Configure Wi-Fi and MQTT credentials:**
```bash
cp include/secrets.example.h include/secrets.h
# Edit secrets.h with your SSID, password, and MQTT broker details
```

**3. Build the firmware:**
```bash
pio run
```

**4. Flash and monitor:**
```bash
pio run --target upload --target monitor
```

---

## Project Status

> 🚧 **Actively in development.** Metrics and sensor details will be updated as the firmware matures.

| Milestone | Status |
|-----------|--------|
| Sensor pipeline (I2S) | ✅ Complete |
| TFLite Micro integration | 🔄 In Progress |
| MQTT telemetry | 🔄 In Progress |
| Memory profiling & tuning | ⏳ Pending |
| Final accuracy benchmarks | ⏳ Pending |

---
