#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MAX30100.h>

// OLED display width and height, change if different
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// OLED reset pin (or -1 if sharing Arduino reset pin)
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_MAX30100 max30100;

const int ecgPin = A0;   // ECG sensor pin
const int tempPin = A1;  // LM35 sensor pin
const int buttonPin = 2; // Button pin

volatile int screenState = 0; // Variable to hold the current screen state

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), changeScreen, FALLING);
  
  Serial.begin(9600);
  Serial1.begin(115200); // Debugging serial

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial1.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Initialize MAX30100
  if (!max30100.begin()) {
    Serial1.println("MAX30100 not found");
    while (1);
  }
  max30100.setup();
  max30100.setMode(MAX30100_MODE_SPO2_HR);
}

void loop() {
  int ecgValue = analogRead(ecgPin);
  float ecgVoltage = (ecgValue * 5.0) / 1023.0; // Convert to voltage
  
  int tempValue = analogRead(tempPin);
  float temperature = (tempValue * 5.0 * 100.0) / 1024.0; // Convert to Celsius
  
  uint16_t ir, red;
  float heartRate, SpO2;
  max30100.getRawValues(&ir, &red);
  heartRate = max30100.getHeartRate();
  SpO2 = max30100.getSpO2();

  // Send data to ESP32
  Serial.print("ECG:");
  Serial.print(ecgVoltage);
  Serial.print(",TEMP:");
  Serial.print(temperature);
  Serial.print(",HR:");
  Serial.print(heartRate);
  Serial.print(",SPO2:");
  Serial.println(SpO2);

  // Display data based on the current screen state
  display.clearDisplay();
  if (screenState == 0) {
    // Display ECG waveform
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("ECG: ");
    display.println(ecgVoltage);
    // Add waveform plotting code here if desired
  } else if (screenState == 1) {
    // Display Temperature
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");
  } else if (screenState == 2) {
    // Display Heart Rate
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("HR: ");
    display.print(heartRate);
    display.println(" bpm");
  } else if (screenState == 3) {
    // Display SpO2
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("SpO2: ");
    display.print(SpO2);
    display.println(" %");
  }
  display.display();
  delay(1000);
}

// Interrupt service routine to change screen state
void changeScreen() {
  screenState = (screenState + 1) % 4;
}
