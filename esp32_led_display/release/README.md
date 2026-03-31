# Build Artifacts

使用 ESP-IDF v5.1.2 编译的固件产物。

## 文件说明

- `esp32_led_display.bin` - 主程序固件 (204KB)
- `bootloader.bin` - ESP32-S3 启动引导程序
- `partitions.bin` - 分区表

## 烧录命令

```bash
esptool.py --chip esp32s3 -p (PORT) -b 460800 write_flash @flash_args
```

或使用 ESP-IDF：

```bash
idf.py -p (PORT) flash
```

## 编译信息

- ESP-IDF 版本: v5.1.2
- 目标芯片: ESP32-S3
- 编译时间: 2026-03-31
