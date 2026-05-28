#include "config.h"
#include "mic_driver.h"
#include "screen_driver.h"
#include "sdcard_driver.h"
#include "wifi_driver.h"
#include "audio_capture.h"
#include "sound_event_detect.h"
#include "action_executor.h"
#include "screen_display.h"
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

WebServer server(80);
int16_t audio_buf[AUDIO_BUFFER_SIZE];
unsigned long last_inference = 0;

// 保存最新一次报警事件
String last_event = "none";
int last_level = 0;
unsigned long last_event_time = 0;

// 返回完整 PWA 网页
void handleRoot() {
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-status-bar-style" content="black">
    <meta name="apple-mobile-web-app-title" content="家庭安防">
    <link rel="manifest" href="/manifest.json">
    <title>家庭安防</title>
    <style>
        *{margin:0;padding:0;box-sizing:border-box}
        body{background:#0a0a0a;color:#fff;font-family:'Microsoft YaHei',sans-serif;height:100vh;display:flex;flex-direction:column;align-items:center;justify-content:center;user-select:none}
        .status{font-size:14px;color:#888;margin-bottom:20px}
        .dot{display:inline-block;width:8px;height:8px;border-radius:50%;margin-right:5px}
        .dot.online{background:#0f0;box-shadow:0 0 10px #0f0;animation:pulse 1.5s infinite}
        .dot.offline{background:#f00}
        @keyframes pulse{0%,100%{opacity:1}50%{opacity:0.3}}
        .card{width:90%;max-width:380px;background:#111;border-radius:20px;padding:30px 20px;text-align:center;border:1px solid #222}
        h2{font-size:20px;color:#ccc;margin-bottom:8px}
        .subtitle{font-size:12px;color:#555;margin-bottom:20px}
        .alarm-box{background:#1a1a1a;border-radius:15px;padding:35px 20px;border:2px solid #222;transition:all 0.3s}
        .alarm-box.glass{border-color:#f00;background:#200;animation:flash 0.3s infinite}
        .alarm-box.baby{border-color:#f90;background:#210}
        .alarm-box.knock{border-color:#0af;background:#012}
        @keyframes flash{0%,100%{background:#200}50%{background:#400}}
        .icon{font-size:55px;margin-bottom:8px}
        .event-text{font-size:22px;font-weight:bold}
        .event-level{font-size:13px;color:#999;margin-top:5px}
        .logs{width:90%;max-width:380px;max-height:180px;overflow-y:auto;text-align:left;margin-top:15px}
        .logs h4{font-size:13px;color:#555;margin-bottom:8px}
        .log-item{font-size:11px;color:#aaa;padding:6px 8px;border-bottom:1px solid #1a1a1a;display:flex;justify-content:space-between}
        .tag{padding:2px 8px;border-radius:10px;font-size:10px}
        .tag.glass{background:#400;color:#f66}
        .tag.baby{background:#430;color:#fa0}
        .tag.knock{background:#023;color:#0af}
    </style>
</head>
<body>
    <div class="status"><span id="dot" class="dot online"></span><span id="statusText">监控中</span></div>
    <div class="card">
        <h2>家庭安防系统</h2>
        <div class="subtitle">基于声音事件识别</div>
        <div id="box" class="alarm-box">
            <div class="icon" id="icon">🟢</div>
            <div class="event-text" id="text">待机中</div>
            <div class="event-level" id="level">正在监听环境声音</div>
        </div>
    </div>
    <div class="logs"><h4>事件记录</h4><div id="logList"><div class="log-item"><span>--</span><span class="tag">等待事件</span></div></div></div>
    <script>
        const API_URL = "/status";
        let lastEvent = "";
        const box = document.getElementById("box"), icon = document.getElementById("icon"), text = document.getElementById("text"), level = document.getElementById("level"), logList = document.getElementById("logList"), dot = document.getElementById("dot"), statusText = document.getElementById("statusText");
        const labels = {
            glass_break:["🔴","玻璃破碎！","紧急","glass"],
            baby_cry:["🟠","婴儿啼哭","重要","baby"],
            knock:["🔵","有人敲门","一般","knock"],
            none:["🟢","待机中","正在监听环境声音",""]
        };
        let timer = null;
        function reset(){
            clearTimeout(timer);
            timer=setTimeout(()=>{
                box.className="alarm-box";
                icon.textContent="🟢";
                text.textContent="待机中";
                level.textContent="正在监听环境声音";
            },3000);
        }
        function addLog(e,l,tg){
            const d=new Date(),ts=d.toLocaleTimeString(),el=document.createElement("div");
            el.className="log-item";
            el.innerHTML=`<span>${ts} ${l}</span><span class="tag ${tg}">${e}</span>`;
            logList.insertBefore(el,logList.firstChild);
            while(logList.children.length>20)logList.removeChild(logList.lastChild);
        }
        async function fetchData(){
            try{
                const res = await fetch(API_URL);
                const json = await res.json();
                dot.className = "dot online";
                statusText.textContent = "监控中";
                if (json.event === lastEvent) return;
                lastEvent = json.event;
                if (json.event === "none") return;
                const info = labels[json.event] || ["🟢","未知事件","提示","none"];
                box.className="alarm-box";box.classList.add(info[3]);
                icon.textContent=info[0];text.textContent=info[1];level.textContent=new Date().toLocaleTimeString();
                addLog(json.event,info[1],info[3]);
                reset();
            }catch(e){
                dot.className = "dot offline";
                statusText.textContent = "连接断开";
            }
        }
        setInterval(fetchData,2000);
        fetchData();
    </script>
</body>
</html>
    )rawliteral";
    server.send(200, "text/html; charset=utf-8", html);
}

// 返回 manifest.json（PWA 需要）
void handleManifest() {
    String json = R"({"name":"家庭安防系统","short_name":"家庭安防","start_url":"/","display":"standalone","background_color":"#0a0a0a","theme_color":"#0a0a0a","icons":[{"src":"/icon.png","sizes":"192x192","type":"image/png"}]})";
    server.send(200, "application/json", json);
}

// 返回最新报警状态
void handleStatus() {
    String json = "{";
    json += "\"event\":\"" + last_event + "\",";
    json += "\"level\":" + String(last_level) + ",";
    json += "\"time\":" + String(last_event_time);
    json += "}";
    server.send(200, "application/json", json);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("System starting...");

    screen_driver_init();
    sdcard_driver_init();
    wifi_driver_init();
    mic_driver_init();
    audio_capture_init();
    sound_event_detect_init();
    action_executor_init();
    screen_display_init();
    screen_display_normal();
    pinMode(LED_PIN, OUTPUT);

    // NTP 校时
    configTime(8 * 3600, 0, "ntp.aliyun.com", "ntp1.aliyun.com");
    delay(1000);

    // 注册路由
    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.on("/manifest.json", handleManifest);
    
    server.begin();
    Serial.print("PWA ready at: http://");
    Serial.println(WiFi.localIP());
}

void loop() {
    server.handleClient();

    audio_capture_read(audio_buf, AUDIO_BUFFER_SIZE);

    unsigned long now = millis();
    if (now - last_inference >= 500) {
        last_inference = now;
        SoundEvent event = sound_event_detect_run(audio_buf, AUDIO_BUFFER_SIZE);
        if (event != EVENT_NONE) {
            last_event_time = now / 1000;
            switch (event) {
                case EVENT_GLASS_BREAK: last_event = "glass_break"; last_level = 3; break;
                case EVENT_BABY_CRY:    last_event = "baby_cry";    last_level = 2; break;
                case EVENT_KNOCK:       last_event = "knock";       last_level = 1; break;
                default: break;
            }
            action_executor_run(event);
        }
    }
    delay(50);
}