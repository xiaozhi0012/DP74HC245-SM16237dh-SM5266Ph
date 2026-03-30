# ESP32-S3 LED Matrix Display Driver (ESP-IDF)

P2.5 LED 模块驱动，支持 320x160 分辨率，32扫描模式。使用 ESP-IDF 框架。

## 硬件配置

- **主控**: ESP32-S3
- **LED模块**: P2.5 室内屏，320x160
- **数据缓冲**: DP74HC245
- **列驱动**: SM16237DH
- **行解码**: SM5266PH
- **扫描模式**: 32扫

## 引脚定义

| 功能 | GPIO |
|------|------|
| R1 | 4 |
| G1 | 5 |
| B1 | 6 |
| R2 | 7 |
| G2 | 15 |
| B2 | 16 |
| A | 17 |
| B | 18 |
| C | 8 |
| D | 9 |
| E | 10 |
| LAT | 12 |
| OE | 13 |
| CLK | 11 |

## 开发环境

- PlatformIO
- ESP-IDF Framework

## 编译烧录

```bash
cd esp32_led_display
pio run
pio run --target upload
pio device monitor
```

## 项目结构

```
esp32_led_display/
├── platformio.ini
├── src/
│   ├── main.cpp
│   ├── LED_Matrix.h
│   └── LED_Matrix.cpp
└── README.md
```

## API 使用

```cpp
LED_Matrix matrix;

// 初始化
matrix.begin();

// 清屏
matrix.clear();

// 设置单个像素
matrix.setPixel(x, y, r, g, b);
// 或使用 0xRRGGBB 格式
matrix.setPixel(x, y, 0xFF0000);

// 全屏填充
matrix.fillScreen(r, g, b);

// 刷新显示
matrix.refresh();
```

## 驱动原理

32扫描模式将160行分成5组，每组32行。行地址通过 A/B/C/D/E 引脚选择，RGB数据通过 R1/G1/B1 (上半屏) 和 R2/G2/B2 (下半屏) 移入。
