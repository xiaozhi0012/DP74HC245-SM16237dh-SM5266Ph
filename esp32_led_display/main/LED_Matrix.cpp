#include "LED_Matrix.h"
#include <driver/gpio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_heap_caps.h>

static const char* TAG = "LED_Matrix";

LED_Matrix::LED_Matrix() {
    frameBuffer = nullptr;
}

void LED_Matrix::begin() {
    initPins();
    clear();
    ESP_LOGI(TAG, "LED Matrix initialized");
}

void LED_Matrix::initPins() {
    frameBuffer = (uint8_t*)heap_caps_malloc(FRAME_BUFFER_SIZE, MALLOC_CAP_32BIT);
    if (frameBuffer == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate frame buffer");
        return;
    }
    
    gpio_config_t io_conf = {
        .pin_bit_mask = 0,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    
    gpio_num_t pinList[] = {
        (gpio_num_t)HUB75_R1, (gpio_num_t)HUB75_G1, (gpio_num_t)HUB75_B1,
        (gpio_num_t)HUB75_R2, (gpio_num_t)HUB75_G2, (gpio_num_t)HUB75_B2,
        (gpio_num_t)HUB75_A, (gpio_num_t)HUB75_B, (gpio_num_t)HUB75_C,
        (gpio_num_t)HUB75_D, (gpio_num_t)HUB75_E,
        (gpio_num_t)HUB75_LAT, (gpio_num_t)HUB75_OE, (gpio_num_t)HUB75_CLK
    };
    
    for (int i = 0; i < 14; i++) {
        io_conf.pin_bit_mask = (1ULL << pinList[i]);
        gpio_config(&io_conf);
        gpio_set_level(pinList[i], 0);
    }
    
    pinMask_R1 = 1ULL << HUB75_R1;
    pinMask_G1 = 1ULL << HUB75_G1;
    pinMask_B1 = 1ULL << HUB75_B1;
    pinMask_R2 = 1ULL << HUB75_R2;
    pinMask_G2 = 1ULL << HUB75_G2;
    pinMask_B2 = 1ULL << HUB75_B2;
    pinMask_A = 1ULL << HUB75_A;
    pinMask_B = 1ULL << HUB75_B;
    pinMask_C = 1ULL << HUB75_C;
    pinMask_D = 1ULL << HUB75_D;
    pinMask_E = 1ULL << HUB75_E;
    pinMask_LAT = 1ULL << HUB75_LAT;
    pinMask_OE = 1ULL << HUB75_OE;
    pinMask_CLK = 1ULL << HUB75_CLK;
    pinMask_DATA = pinMask_R1 | pinMask_G1 | pinMask_B1 | pinMask_R2 | pinMask_G2 | pinMask_B2;
}

void LED_Matrix::clear() {
    if (frameBuffer) {
        memset(frameBuffer, 0, FRAME_BUFFER_SIZE);
    }
}

void LED_Matrix::setPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT || !frameBuffer) return;
    
    uint32_t pixelIndex = (y * MATRIX_WIDTH + x) * 3;
    uint8_t bitPosition = pixelIndex & 0x7;
    uint32_t byteIndex = pixelIndex >> 3;
    
    if (r > 0) frameBuffer[byteIndex] |= (1 << bitPosition);
    if (g > 0) frameBuffer[byteIndex + 1] |= (1 << bitPosition);
    if (b > 0) frameBuffer[byteIndex + 2] |= (1 << bitPosition);
}

void LED_Matrix::setPixel(uint16_t x, uint16_t y, uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    setPixel(x, y, r, g, b);
}

void LED_Matrix::fillScreen(uint8_t r, uint8_t g, uint8_t b) {
    clear();
    
    if ((r == 0 && g == 0 && b == 0) || !frameBuffer) return;
    
    for (uint16_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint16_t x = 0; x < MATRIX_WIDTH; x++) {
            setPixel(x, y, r, g, b);
        }
    }
}

void LED_Matrix::fillScreen(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    fillScreen(r, g, b);
}

void LED_Matrix::drawTestPattern() {
    clear();
    
    uint16_t sectionWidth = MATRIX_WIDTH / 3;
    
    for (uint16_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint16_t x = 0; x < MATRIX_WIDTH; x++) {
            uint32_t color;
            if (x < sectionWidth) {
                color = 0x00FF00;
            } else if (x < sectionWidth * 2) {
                color = 0xFF0000;
            } else {
                color = 0x0000FF;
            }
            setPixel(x, y, color);
        }
    }
}

void LED_Matrix::setRow(uint8_t row) {
    gpio_set_level((gpio_num_t)HUB75_A, (row & 0x01) ? 1 : 0);
    gpio_set_level((gpio_num_t)HUB75_B, (row & 0x02) ? 1 : 0);
    gpio_set_level((gpio_num_t)HUB75_C, (row & 0x04) ? 1 : 0);
    gpio_set_level((gpio_num_t)HUB75_D, (row & 0x08) ? 1 : 0);
    gpio_set_level((gpio_num_t)HUB75_E, (row & 0x10) ? 1 : 0);
}

void LED_Matrix::latchRow() {
    gpio_set_level((gpio_num_t)HUB75_LAT, 1);
    gpio_set_level((gpio_num_t)HUB75_LAT, 0);
}

void LED_Matrix::clockData() {
    gpio_set_level((gpio_num_t)HUB75_CLK, 1);
    gpio_set_level((gpio_num_t)HUB75_CLK, 0);
}

void LED_Matrix::shiftBitPlane(uint8_t bit) {
    uint32_t rowBase = bit * MATRIX_WIDTH * 3;
    
    for (uint16_t col = 0; col < MATRIX_WIDTH; col++) {
        uint32_t pixelData = 0;
        
        for (uint8_t rowGroup = 0; rowGroup < 8; rowGroup++) {
            uint8_t actualRow = bit * 8 + rowGroup;
            if (actualRow >= MATRIX_HEIGHT) continue;
            
            uint32_t pixelIndex = rowBase + (actualRow * MATRIX_WIDTH + col) * 3;
            uint32_t byteIndex = pixelIndex >> 3;
            uint8_t bitPos = pixelIndex & 0x7;
            
            if (frameBuffer[byteIndex] & (1 << bitPos)) pixelData |= (1 << (rowGroup * 4));
            if (frameBuffer[byteIndex + 1] & (1 << bitPos)) pixelData |= (2 << (rowGroup * 4));
            if (frameBuffer[byteIndex + 2] & (1 << bitPos)) pixelData |= (4 << (rowGroup * 4));
        }
        
        uint32_t dataMask = 0;
        if (pixelData & 0x01) dataMask |= pinMask_R1;
        if (pixelData & 0x02) dataMask |= pinMask_G1;
        if (pixelData & 0x04) dataMask |= pinMask_B1;
        if (pixelData & 0x10) dataMask |= pinMask_R2;
        if (pixelData & 0x20) dataMask |= pinMask_G2;
        if (pixelData & 0x40) dataMask |= pinMask_B2;
        
        gpio_set_level((gpio_num_t)HUB75_R1, (dataMask & pinMask_R1) ? 1 : 0);
        gpio_set_level((gpio_num_t)HUB75_G1, (dataMask & pinMask_G1) ? 1 : 0);
        gpio_set_level((gpio_num_t)HUB75_B1, (dataMask & pinMask_B1) ? 1 : 0);
        gpio_set_level((gpio_num_t)HUB75_R2, (dataMask & pinMask_R2) ? 1 : 0);
        gpio_set_level((gpio_num_t)HUB75_G2, (dataMask & pinMask_G2) ? 1 : 0);
        gpio_set_level((gpio_num_t)HUB75_B2, (dataMask & pinMask_B2) ? 1 : 0);
        
        clockData();
    }
}

void LED_Matrix::outputRow(uint8_t row) {
    setRow(row);
    shiftBitPlane(row);
    latchRow();
}

void LED_Matrix::refresh() {
    gpio_set_level((gpio_num_t)HUB75_OE, 1);
    
    for (uint8_t row = 0; row < 5; row++) {
        outputRow(row);
        
        gpio_set_level((gpio_num_t)HUB75_OE, 0);
        esp_rom_delay_us(200);
        gpio_set_level((gpio_num_t)HUB75_OE, 1);
    }
}
