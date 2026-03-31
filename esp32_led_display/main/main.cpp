#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "LED_Matrix.h"

static const char* TAG = "main";

LED_Matrix matrix;
uint8_t animationFrame = 0;

void led_task(void* pvParameters) {
    ESP_LOGI(TAG, "LED Task started");
    
    matrix.begin();
    vTaskDelay(pdMS_TO_TICKS(500));
    
    matrix.drawTestPattern();
    ESP_LOGI(TAG, "Test pattern displayed");
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    while (1) {
        matrix.clear();
        
        uint32_t colors[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF};
        uint32_t color = colors[animationFrame % 6];
        
        for (uint16_t y = 20; y < 140; y++) {
            for (uint16_t x = 80; x < 240; x++) {
                matrix.setPixel(x, y, color);
            }
        }
        
        uint16_t centerX = 160;
        uint16_t centerY = 80;
        uint16_t radius = 30;
        
        for (uint16_t y = centerY - radius; y <= centerY + radius; y++) {
            for (uint16_t x = centerX - radius; x <= centerX + radius; x++) {
                int32_t dx = x - centerX;
                int32_t dy = y - centerY;
                if (dx * dx + dy * dy <= radius * radius) {
                    matrix.setPixel(x, y, 0xFFFFFF);
                }
            }
        }
        
        animationFrame++;
        ESP_LOGD(TAG, "Frame: %d", animationFrame);
        
        matrix.refresh();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "ESP32-S3 LED Matrix Starting...");
    
    xTaskCreatePinnedToCore(led_task, "led_task", 4096, NULL, 1, NULL, 0);
}
