#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"

static const char *TAG = "BLE_BEACON";

// ====================================================================
// The RTC_DATA_ATTR macro tells the linker to put these variables 
// into the RTC Slow Memory, which survives Deep Sleep!
// ====================================================================
RTC_DATA_ATTR int boot_count = 0;
RTC_DATA_ATTR float simulated_temperature = 20.0f;

void app_main(void)
{
    // 1. Diagnose WHY we woke up
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    if (cause != ESP_SLEEP_WAKEUP_TIMER) {
        // This was a cold boot (plugged in or reset button)
        ESP_LOGI(TAG, "--- COLD BOOT ---");
        boot_count = 0; 
        simulated_temperature = 20.0f;
    } else {
        // This was a scheduled wake-up
        ESP_LOGI(TAG, "--- WOKE UP FROM DEEP SLEEP ---");
    }

    // 2. Execute the "Active" Workload
    boot_count++;
    simulated_temperature += 0.5f; // Simulate data changing over time

    ESP_LOGI(TAG, "Boot Count: %d | Temp: %.1f C", boot_count, simulated_temperature);
    
    // --- THIS IS WHERE THE BLE CODE WILL GO ---
    ESP_LOGI(TAG, "Turning on RF Radio...");
    ESP_LOGI(TAG, "Broadcasting BLE Advertisement...");
    vTaskDelay(pdMS_TO_TICKS(1000)); // Simulate the time it takes to transmit
    ESP_LOGI(TAG, "Transmission Complete. Shutting down RF.");
    // ------------------------------------------

    // 3. Configure the Wake-Up Source
    const int sleep_time_sec = 10;
    ESP_LOGI(TAG, "Entering Deep Sleep for %d seconds...", sleep_time_sec);
    
    // ESP-IDF takes wake-up time in microseconds (uS)
    esp_sleep_enable_timer_wakeup((uint64_t)sleep_time_sec * 1000000ULL); 

    // 4. Pull the Plug
    // Warning: Any code written below this function will NEVER execute.
    esp_deep_sleep_start(); 
}