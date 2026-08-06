#define BLYNK_TEMPLATE_ID   "TMPL6Xrb54_MO"           // From Blynk cloud
#define BLYNK_TEMPLATE_NAME "Water Monitor"       // From Blynk cloud
#define BLYNK_AUTH_TOKEN    "M2s9dPNUFDmHV0ztBnWAFTmjmv31Vc23"      // From Blynk device

#define WIFI_SSID           ""  //input your device name wifi ssid
#define WIFI_PASS           "" //input your hotspot password
// >>>>>> END OF CHANGES <<<<<<

#define BLYNK_PRINT Serial  // Debug prints to USB serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// UART2 to STM32
HardwareSerial stm32Serial(2);  // UART2: RX=GPIO16, TX=GPIO17

// Data from STM32
float waterLevel = 0;
float distance = 0;
int pumpStatus = 0;
bool dataReceived = false;
unsigned long lastDataTime = 0;

// Timer for sending data to Blynk
BlynkTimer timer;

// Called when app button V2 (manual override) is pressed
BLYNK_WRITE(V2) {
    int override = param.asInt();
    // Send command repeatedly to make sure STM32 gets it
    for (int i = 0; i < 3; i++) {
        stm32Serial.printf("OVERRIDE:%d\n", override);
        delay(50);
    }
    Serial.printf("Sent override: %d\n", override);
}

// Send sensor data to Blynk (called by timer every 2 seconds)
void sendToBlynk() {
    if (dataReceived) {
        Blynk.virtualWrite(V0, waterLevel);
        Blynk.virtualWrite(V1, pumpStatus);
        Blynk.virtualWrite(V3, distance);
        Blynk.virtualWrite(V4, WiFi.RSSI());

        Serial.printf("Sent to Blynk: Level=%.1f%% Pump=%d Dist=%.1f\n",
                       waterLevel, pumpStatus, distance);

        // Send notifications on critical events
        if (waterLevel <= 10.0 && waterLevel > 0) {
            Blynk.logEvent("critical_low", "Water level critically low!");
        }
        if (waterLevel >= 95.0) {
            Blynk.logEvent("tank_full", "Tank is full!");
        }
    }
}

// Parse incoming data from STM32
// Format: "L:45.2,D:27.3,P:1\n"
void parseSTM32Data(String data) {
    data.trim();

    int lIdx = data.indexOf("L:");
    int dIdx = data.indexOf("D:");
    int pIdx = data.indexOf("P:");

    if (lIdx >= 0 && dIdx >= 0 && pIdx >= 0) {
        waterLevel = data.substring(lIdx + 2, data.indexOf(",", lIdx)).toFloat();
        distance = data.substring(dIdx + 2, data.indexOf(",", dIdx)).toFloat();
        pumpStatus = data.substring(pIdx + 2).toInt();
        dataReceived = true;
        lastDataTime = millis();
    }
}

void setup() {
    // USB serial for debug
    Serial.begin(115200);
    Serial.println("ESP32 Blynk Bridge Starting...");

    // UART2 to STM32
    stm32Serial.begin(9600, SERIAL_8N1, 16, 17);  // RX=16, TX=17

    // Connect to WiFi and Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

    // Send data to Blynk every 2 seconds
    timer.setInterval(2000L, sendToBlynk);

    Serial.println("Setup complete!");
}

void loop() {
    Blynk.run();
    timer.run();

    // Read data from STM32
    if (stm32Serial.available()) {
        String line = stm32Serial.readStringUntil('\n');
        if (line.length() > 0) {
            Serial.printf("From STM32: %s\n", line.c_str());
            parseSTM32Data(line);
        }
    }

    // If no data from STM32 for 10 seconds, mark as disconnected
    if (dataReceived && (millis() - lastDataTime > 10000)) {
        dataReceived = false;
        Serial.println("STM32 data timeout!");
    }
}
