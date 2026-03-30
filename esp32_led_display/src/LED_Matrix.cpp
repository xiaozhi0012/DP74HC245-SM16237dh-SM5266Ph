#include "LED_Matrix.h"
#include <Arduino.h>

LED_Matrix::LED_Matrix() {
    portOutRegister = (volatile uint32_t*)((GPIO.out_u.data) );
}

void LED_Matrix::begin() {
    initPins();
    clear();
}

void LED_Matrix::initPins() {
    pinMode(HUB75_R1, OUTPUT);
    pinMode(HUB75_G1, OUTPUT);
    pinMode(HUB75_B1, OUTPUT);
    pinMode(HUB75_R2, OUTPUT);
    pinMode(HUB75_G2, OUTPUT);
    pinMode(HUB75_B2, OUTPUT);
    pinMode(HUB75_A, OUTPUT);
    pinMode(HUB75_B, OUTPUT);
    pinMode(HUB75_C, OUTPUT);
    pinMode(HUB75_D, OUTPUT);
    pinMode(HUB75_E, OUTPUT);
    pinMode(HUB75_LAT, OUTPUT);
    pinMode(HUB75_OE, OUTPUT);
    pinMode(HUB75_CLK, OUTPUT);
    
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
    
    GPIO.out_u.val = 0;
}

void LED_Matrix::clear() {
    memset(frameBuffer, 0, sizeof(frameBuffer));
}

void LED_Matrix::setPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return;
    
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
    
    if (r > 0 || g > 0 || b > 0) {
        for (uint16_t y = 0; y < MATRIX_HEIGHT; y++) {
            for (uint16_t x = 0; x < MATRIX_WIDTH; x++) {
                setPixel(x, y, r, g, b);
            }
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
    
    for (uint16_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint16_t x = 0; x < MATRIX_WIDTH; x++) {
            uint32_t color;
            if (x < MATRIX_WIDTH / 3) {
                color = 0xFF0000;
            } else if (x < MATRIX_WIDTH * 2 / 3) {
                color = 0x00FF00;
            } else {
                color = 0x0000FF;
            }
            setPixel(x, y, color);
        }
    }
}

void LED_Matrix::setRow(uint8_t row) {
    uint32_t rowMask = 0;
    
    if (row & 0x01) rowMask |= pinMask_A;
    if (row & 0x02) rowMask |= pinMask_B;
    if (row & 0x04) rowMask |= pinMask_C;
    if (row & 0x08) rowMask |= pinMask_D;
    if (row & 0x10) rowMask |= pinMask_E;
    
    GPIO.out_u.val = rowMask;
}

void LED_Matrix::latchRow() {
    GPIO.out_w1ts.val = pinMask_LAT;
    GPIO.out_w1tc.val = pinMask_LAT;
}

void LED_Matrix::clockData() {
    GPIO.out_w1ts.val = pinMask_CLK;
    GPIO.out_w1tc.val = pinMask_CLK;
}

void LED_Matrix::shiftBitPlane(uint8_t bit) {
    for (uint16_t col = 0; col < MATRIX_WIDTH; col++) {
        uint32_t pixelData = 0;
        
        for (uint8_t rowGroup = 0; rowGroup < 8; rowGroup++) {
            uint8_t actualRow = bit * 8 + rowGroup;
            if (actualRow >= MATRIX_HEIGHT) continue;
            
            uint32_t pixelIndex = (actualRow * MATRIX_WIDTH + col) * 3;
            uint8_t byteIndex = pixelIndex >> 3;
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
        
        GPIO.out_u.val = dataMask;
        clockData();
    }
}

void LED_Matrix::outputRow(uint8_t row) {
    setRow(row);
    shiftBitPlane(row / 8);
    latchRow();
}

void LED_Matrix::refresh() {
    GPIO.out_w1ts.val = pinMask_OE;
    
    for (uint8_t row = 0; row < 8; row++) {
        outputRow(row);
        
        GPIO.out_w1tc.val = pinMask_OE;
        delayMicroseconds(200);
        GPIO.out_w1ts.val = pinMask_OE;
    }
    
    GPIO.out_w1ts.val = pinMask_OE;
}
