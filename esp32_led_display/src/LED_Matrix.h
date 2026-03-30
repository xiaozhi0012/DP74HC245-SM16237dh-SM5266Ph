#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <Arduino.h>

constexpr int HUB75_R1 = 4;
constexpr int HUB75_G1 = 5;
constexpr int HUB75_B1 = 6;
constexpr int HUB75_R2 = 7;
constexpr int HUB75_G2 = 15;
constexpr int HUB75_B2 = 16;
constexpr int HUB75_A = 17;
constexpr int HUB75_B = 18;
constexpr int HUB75_C = 8;
constexpr int HUB75_D = 9;
constexpr int HUB75_E = 10;
constexpr int HUB75_LAT = 12;
constexpr int HUB75_OE = 13;
constexpr int HUB75_CLK = 11;

constexpr uint16_t MATRIX_WIDTH = 320;
constexpr uint16_t MATRIX_HEIGHT = 160;
constexpr uint8_t SCAN_MODE = 32;

constexpr uint16_t COLOR_DEPTH = 8;
constexpr uint16_t FRAME_BUFFER_SIZE = (MATRIX_WIDTH * MATRIX_HEIGHT * 3) / 8;

class LED_Matrix {
public:
    LED_Matrix();
    void begin();
    void clear();
    void setPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);
    void setPixel(uint16_t x, uint16_t y, uint32_t color);
    void fillScreen(uint8_t r, uint8_t g, uint8_t b);
    void fillScreen(uint32_t color);
    void drawTestPattern();
    void refresh();
    
private:
    uint8_t frameBuffer[MATRIX_WIDTH * MATRIX_HEIGHT * 3 / 8];
    
    void initPins();
    void latchRow();
    void clockData();
    void setRow(uint8_t row);
    void outputRow(uint8_t row);
    void shiftBitPlane(uint8_t bit);
    
    volatile uint32_t* portOutRegister;
    uint32_t pinMask_R1, pinMask_G1, pinMask_B1;
    uint32_t pinMask_R2, pinMask_G2, pinMask_B2;
    uint32_t pinMask_A, pinMask_B, pinMask_C, pinMask_D, pinMask_E;
    uint32_t pinMask_LAT, pinMask_OE, pinMask_CLK;
};

#endif
