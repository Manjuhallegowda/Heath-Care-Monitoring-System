#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials
char ssid[] = "MHG";
char pass[] = "chaitu288";

// Blynk Auth Token
char auth[] = "bfecVrCYy2SyemFbNap6FkZZDu7r4njC";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    
    // Parse the data
    int ecgIndex = data.indexOf("ECG:");
    int tempIndex = data.indexOf(",TEMP:");
    int hrIndex = data.indexOf(",HR:");
    int spo2Index = data.indexOf(",SPO2:");

    float ecgValue = data.substring(ecgIndex + 4, tempIndex).toFloat();
    float tempValue = data.substring(tempIndex + 6, hrIndex).toFloat();
    float hrValue = data.substring(hrIndex + 4, spo2Index).toFloat();
    float spo2Value = data.substring(spo2Index + 6).toFloat();

    // Send data to Blynk
    Blynk.virtualWrite(V1, ecgValue);
    Blynk.virtualWrite(V2, tempValue);
    Blynk.virtualWrite(V3, hrValue);
    Blynk.virtualWrite(V4, spo2Value);
    
    // Check for abnormal values and raise alerts
    if (tempValue > 30) {
      Blynk.notify("Warning: High body temperature!");
    }
    if (hrValue < 60 || hrValue > 100) {
      Blynk.notify("Warning: Abnormal heart rate!");
    }
  }

  Blynk.run();
}
