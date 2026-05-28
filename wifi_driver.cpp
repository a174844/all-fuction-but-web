#include "wifi_driver.h"
#include "config.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>                          // ← 新增：用于时间函数

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// =====================================================
// MQTT 重连
// =====================================================

void mqtt_reconnect() {

    static bool firstConnect = true;

    while (!mqttClient.connected()) {

        if (mqttClient.connect(
                ONENET_DEVICE_NAME,   // ClientID
                ONENET_PRODUCT_ID,    // Username
                ONENET_TOKEN)) {      // Password

            if (firstConnect) {

                Serial.println("MQTT Connected");

                firstConnect = false;
            }

        } else {

            delay(3000);
        }
    }
}

// =====================================================
// WiFi 初始化
// =====================================================

void wifi_driver_init() {

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {

        delay(500);
    }

    Serial.println("WiFi Connected");

    // ==== 新增：同步网络时间（北京时间） ====
    configTime(8 * 3600, 0, "ntp.aliyun.com", "ntp1.aliyun.com");
    delay(500);
    Serial.println("Time synced");
    // ==== 新增结束 ====

    // OneNET MQTT
    mqttClient.setServer("mqtts.heclouds.com", 1883);

    // 增大缓存
    mqttClient.setBufferSize(1024);
}

// event_id: 0=glass_break, 1=baby_cry, 2=knock
void mqtt_send_alarm(const char* event_type, int level) {

    // 保证 MQTT 在线
    if (!mqttClient.connected()) {

        mqtt_reconnect();
    }

    // OneNET 物模型 Topic
    String topic =
        "$sys/" +
        String(ONENET_PRODUCT_ID) +
        "/" +
        String(ONENET_DEVICE_NAME) +
        "/thing/property/post";

    // OneNET 物模型 JSON
    String msg =
        "{"
        "\"id\":\"123\","
        "\"version\":\"1.0\","
        "\"params\":{"
            "\"event\":{"
                "\"value\":\"" + String(event_type) + "\""
            "},"
            "\"level\":{"
                "\"value\":" + String(level) +
            "}"
        "}"
        "}";

    // 发布
    bool ok = mqttClient.publish(topic.c_str(), msg.c_str());

    // 调试输出
    Serial.println("========== MQTT Upload ==========");

    Serial.println("Topic:");

    Serial.println(topic);

    Serial.println("Payload:");

    Serial.println(msg);

    if (ok) {

        Serial.println("Upload OK");

    } else {

        Serial.println("Upload Failed");
    }

    // ==== 新增：打印当前真实时间 ====
    struct tm timeinfo;
    char timeStr[20];
    if (getLocalTime(&timeinfo)) {
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
        Serial.print("Time: ");
        Serial.println(timeStr);
    }
    // ==== 新增结束 ====

    Serial.println("=================================");
}

// =====================================================
// MQTT Loop
// =====================================================

void mqtt_loop() {

    if (!mqttClient.connected()) {

        mqtt_reconnect();
    }

    mqttClient.loop();
}