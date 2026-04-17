# ESP32-C6 Low Power Beacon (Deep Sleep & RTC Memory)

## Overview
This repository contains a foundational template for ultra-low-power IoT devices built on the **ESP32-C6** SoC using **ESP-IDF**. It demonstrates how to properly architect battery-operated firmware by utilizing Deep Sleep, RTC memory retention, and a linear execution model, bypassing traditional FreeRTOS blocking loops.

Crucially, this project addresses the hardware-specific quirks of the ESP32-C6, such as managing the internal USB CDC connection during aggressive power cycling.

## Architecture & Key Concepts

### 1. The "Kill Switch" Linear Execution Model
Battery-operated devices cannot afford to run infinite `while(1)` loops. This firmware executes linearly: it wakes up, diagnoses the wake cause, performs its task, calculates the next wake interval, and immediately issues the `esp_deep_sleep_start()` command to shut down the main RISC-V core.

### 2. RTC Slow Memory State Retention
During Deep Sleep, standard RAM is powered off to minimize current draw (achieving microamp-level consumption). To prevent data loss across sleep cycles, the `RTC_DATA_ATTR` macro is used to allocate critical state variables (`boot_count` and `simulated_temperature`) within the RTC Slow Memory, which remains powered by a low-dropout regulator.

### 3. Mitigating the Internal USB CDC Disconnect Quirk
Unlike older ESP32 models with external CP2102 UART chips, the ESP32-C6 features an internal USB PHY. When the chip enters Deep Sleep, the USB connection is physically severed, causing the host PC to drop the COM port.
* **The Race Condition:** The C6 boots in milliseconds, often printing its telemetry data before the host PC has time to re-enumerate the USB device and reattach the Serial Monitor.
* **The Solution:** A deliberate `vTaskDelay` is placed at the very beginning of the boot sequence, allowing the host PC sufficient time to reconnect before critical telemetry is transmitted over the console.

## Hardware & Toolchain
* **Target:** ESP32-C6 Development Board
* **Framework:** ESP-IDF (v5.x or newer)

## Build & Flash Instructions
1. Set the ESP-IDF target to ESP32-C6:
   ```bash
   idf.py set-target esp32c6

2. Build the project, flash it to the board, and open the monitor:
   ```bash
   idf.py build flash monitor

## Practical Application
This architecture mimics the exact firmware structure deployed in commercial environmental sensors and asset trackers. It highlights the ability to:

* Manage complex SoC sleep states to maximize battery life.

* Maintain variable state across hard power cycles without external EEPROM.

* Debug and resolve low-level USB peripheral behaviors on modern silicon.