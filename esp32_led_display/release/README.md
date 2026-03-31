# Build Artifacts

编译产物目录，用于烧录到 ESP32-S3。

## 文件说明

- `bootloader.bin` - ESP32-S3 启动引导程序
- `partitions.bin` - 分区表

## 烧录命令

```bash
esptool.py --chip esp32s3 write_flash @flash_args
```

或使用 PlatformIO：

```bash
pio run --target upload
```

## 编译时间

2026-03-31
