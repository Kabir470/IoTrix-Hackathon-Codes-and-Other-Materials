/*
  ==============================================================================
  🎯 HACKATHON STARTER TEMPLATE: DHT11 (Temp & Humidity) + Water Level Sensor + Firebase
  ==============================================================================

  📌 Microcontroller: ESP32 Dev Module / ESP32 WROOM
  
  📦 Required Libraries (Install via Arduino Library Manager):
     1. "DHT sensor library" by Adafruit
     2. "Adafruit Unified Sensor" by Adafruit
     3. "Firebase ESP32 Client" by Mobizt
     4. "ArduinoJson" (Dependency for Firebase library)

  🔌 Hardware Wiring Guide for ESP32:
     -------------------------------------------------------------------------
     Sensor Pin               | ESP32 Pin     | Notes
     -------------------------------------------------------------------------
     [DHT11 Temperature & Humidity]
     - VCC                    | 3.3V or 5V    | Power (+)
     - GND                    | GND           | Ground (-)
     - DATA / OUT             | GPIO 4        | Digital Data Pin
     
     [Water Level Sensor]
     - VCC / (+)              | 3.3V or 5V    | Power (+)
     - GND / (-)              | GND           | Ground (-)
     - S / OUT (Signal)       | GPIO 34       | Must use ADC1 (GPIO 34, 35, 36, 39)
     -------------------------------------------------------------------------

  ==============================================================================
*/

#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHT.h"

// ------------------------------------------------------------------------------
// ⚙️ CONFIGURATION SECTION [MODIFIED BY PARTICIPANTS]
// ------------------------------------------------------------------------------

// [TODO 1]: Wi-Fi Credentials
#define WIFI_SSID     "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// [TODO 2]: Firebase Realtime Database Configuration
// Note: Database URL must NOT include 'https://' or trailing slashes '/'
// Example: "your-project-id-default-rtdb.firebaseio.com"
#define FIREBASE_HOST "YOUR_PROJECT_ID-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET_OR_API_KEY"

// [TODO 3]: Hardware Pin Configuration
#define DHTPIN               4        // Digital GPIO pin for DHT11 data
#define DHTTYPE              DHT11    // Sensor model: DHT11
#define WATER_SENSOR_PIN     34       // Analog ADC1 pin for Water Level (GPIO 34)

// [TODO 4]: Sensor Thresholds & Update Interval (in milliseconds)
const unsigned long SEND_INTERVAL = 3000;  // Upload to Firebase every 3 seconds
const int WATER_THRESHOLD = 500;           // Analog value threshold for water detection

// ------------------------------------------------------------------------------
// 📦 GLOBAL OBJECTS & VARIABLES
// ------------------------------------------------------------------------------

DHT dht(DHTPIN, DHTTYPE);

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastSendTime = 0;

// ------------------------------------------------------------------------------
// 🚀 SETUP FUNCTION (Runs Once)
// ------------------------------------------------------------------------------
void setup() {
  // Step 1: Initialize Serial Monitor
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("==================================================");
  Serial.println("🔥 ESP32 DHT11 + Water Sensor Firebase Pipeline");
  Serial.println("==================================================");

  // Initialize Sensors
  dht.begin();
  pinMode(WATER_SENSOR_PIN, INPUT);

  // Step 2: Connect ESP32 to Wi-Fi
  Serial.print("📶 Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("✅ Wi-Fi Connected!");
  Serial.print("📍 ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Step 3: Configure Firebase Connection
  Serial.println("🔥 Initializing Firebase Realtime Database...");
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Buffer size settings for ESP32
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);

  Serial.println("✅ Firebase Initialized Successfully!");
  Serial.println("⏳ Reading sensor data...");
}

// ------------------------------------------------------------------------------
// 🔁 MAIN LOOP FUNCTION (Runs Continuously)
// ------------------------------------------------------------------------------
void loop() {
  // Non-blocking timer to control data transmission rate
  if (millis() - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = millis();

    // Step 4: Read DHT11 Temperature & Humidity
    float humidity = dht.readHumidity();
    float tempC = dht.readTemperature();       // Celsius

    // Step 5: Read Water Level Sensor (ESP32 ADC is 12-bit: 0 to 4095)
    int rawWaterValue = analogRead(WATER_SENSOR_PIN);
    float waterPercentage = (rawWaterValue / 4095.0) * 100.0;
    bool isWaterDetected = rawWaterValue > WATER_THRESHOLD;

    // Check if DHT readings failed
    if (isnan(humidity) || isnan(tempC)) {
      Serial.println("❌ Failed to read from DHT sensor! Check wiring.");
    } else {
      // Print values to Serial Monitor
      Serial.println("-----------------------------------------");
      Serial.print("🌡️ Temperature: ");
      Serial.print(tempC);
      Serial.println(" °C");
      Serial.print("💧 Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");
      Serial.print("🌊 Water Level Raw (0-4095): ");
      Serial.println(rawWaterValue);
      Serial.print("🌊 Water Level (%): ");
      Serial.print(waterPercentage);
      Serial.println(" %");
      Serial.print("🚨 Water State: ");
      Serial.println(isWaterDetected ? "WATER DETECTED!" : "DRY");

      // Step 6: Push Data to Firebase Database
      if (Firebase.setFloat(firebaseData, "/sensor/temperature", tempC)) {
        Serial.println("✅ Firebase Update: /sensor/temperature -> SUCCESS");
      } else {
        Serial.print("❌ Firebase Update Failed: ");
        Serial.println(firebaseData.errorReason());
      }

      if (Firebase.setFloat(firebaseData, "/sensor/humidity", humidity)) {
        Serial.println("✅ Firebase Update: /sensor/humidity -> SUCCESS");
      } else {
        Serial.print("❌ Firebase Update Failed: ");
        Serial.println(firebaseData.errorReason());
      }

      if (Firebase.setFloat(firebaseData, "/sensor/water_level_percent", waterPercentage)) {
        Serial.println("✅ Firebase Update: /sensor/water_level_percent -> SUCCESS");
      } else {
        Serial.print("❌ Firebase Update Failed: ");
        Serial.println(firebaseData.errorReason());
      }

      if (Firebase.setBool(firebaseData, "/sensor/water_detected", isWaterDetected)) {
        Serial.println("✅ Firebase Update: /sensor/water_detected -> SUCCESS");
      } else {
        Serial.print("❌ Firebase Update Failed: ");
        Serial.println(firebaseData.errorReason());
      }
    }
  }
}
