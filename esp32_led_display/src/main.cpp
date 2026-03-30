#include <Arduino.h>
#include "LED_Matrix.h"

LED_Matrix matrix;

unsigned long lastUpdate = 0;
uint8_t animationFrame = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Initializing LED Matrix...");
    matrix.begin();
    Serial.println("LED Matrix initialized!");
    
    matrix.fillScreen(0, 0, 0);
    delay(500);
    
    matrix.drawTestPattern();
    delay(2000);
    
    Serial.println("Starting animation...");
}

void loop() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdate > 100) {
        lastUpdate = currentTime;
        
        matrix.clear();
        
        uint32_t colors[] = {
            0xFF0000,
            0x00FF00,
            0x0000FF,
            0xFFFF00,
            0xFF00FF,
            0x00FFFF
        };
        
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
        
        Serial.printf("Frame: %d\n", animationFrame);
    }
    
    matrix.refresh();
}

void drawCheckerboard() {
    matrix.clear();
    
    for (uint16_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint16_t x = 0; x < MATRIX_WIDTH; x++) {
            bool isWhite = ((x / 20) + (y / 20)) % 2 == 0;
            if (isWhite) {
                matrix.setPixel(x, y, 255, 255, 255);
            }
        }
    }
}

void drawRainbow() {
    matrix.clear();
    
    uint16_t sectionWidth = MATRIX_WIDTH / 6;
    
    for (uint16_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint16_t x = 0; x < MATRIX_WIDTH; x++) {
            uint32_t color;
            if (x < sectionWidth) {
                color = 0xFF0000;
            } else if (x < sectionWidth * 2) {
                color = 0xFF7F00;
            } else if (x < sectionWidth * 3) {
                color = 0xFFFF00;
            } else if (x < sectionWidth * 4) {
                color = 0x00FF00;
            } else if (x < sectionWidth * 5) {
                color = 0x0000FF;
            } else {
                color = 0x8B00FF;
            }
            matrix.setPixel(x, y, color);
        }
    }
}
