#ifndef CONFIG_H
#define CONFIG_H

// =====================================================
// WiFi
// =====================================================

#define WIFI_SSID      "Xiaomi 12S Pro"
#define WIFI_PASSWORD  "giftboy11"


// =====================================================
// OneNET MQTT
// =====================================================

// 产品 ID
#define ONENET_PRODUCT_ID  "W9Obq2w01j"

#define ONENET_DEVICE_NAME "ESP32_HomeGuard"

#define ONENET_TOKEN "version=2018-10-31&res=products%2FW9Obq2w01j%2Fdevices%2FESP32_HomeGuard&et=1893456000&method=md5&sign=l1jx9C1%2BjAlXBiEYgnTzfA%3D%3D"    


// =====================================================
// 屏幕引脚
// =====================================================

#define SCREEN_LED    15
#define SCREEN_SCK    12
#define SCREEN_SDA    11
#define SCREEN_A0     10
#define SCREEN_RESET  13
#define SCREEN_CS     14


// =====================================================
// SD 卡引脚
// =====================================================

#define SD_SCK        38
#define SD_MOSI       39
#define SD_MISO       40
#define SD_CS         41


// =====================================================
// 麦克风引脚
// =====================================================

#define MIC_SCK       4
#define MIC_WS        5
#define MIC_SD        6


// =====================================================
// 报警灯
// =====================================================

#define LED_PIN       47


// =====================================================
// 音频参数
// =====================================================

#define SAMPLE_RATE         16000
#define AUDIO_BUFFER_SIZE   512


// =====================================================
// 声音事件枚举
// =====================================================

enum SoundEvent {
    EVENT_NONE = 0,
    EVENT_GLASS_BREAK,
    EVENT_BABY_CRY,
    EVENT_KNOCK
};

#endif