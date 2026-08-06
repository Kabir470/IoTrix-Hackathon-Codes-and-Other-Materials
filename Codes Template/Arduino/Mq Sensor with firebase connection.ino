/*
  ==============================================================================
  🎯 HACKATHON STARTER TEMPLATE: MQ Gas Sensor + Firebase Realtime Database
  ==============================================================================

  📌 Board Compatibility: ESP32 Dev Module / ESP32 WROOM / ESP32-CAM
  
  📦 Required Libraries (Install via Arduino Library Manager):
     1. "Firebase ESP32 Client" by Mobizt
     2. "ArduinoJson" (Dependency for Firebase library)

  🔌 Hardware Wiring Guide for ESP32:
     --------------------------------------------------
     MQ Sensor Pin   |  ESP32 Pin
     --------------------------------------------------
     VCC             |  5V (or VIN)
     GND             |  GND
     AO (Analog Out) |  GPIO 36 / VP (or any ADC pin e.g. GPIO 34)
     DO (Digital Out)|  GPIO 4 (Optional threshold output)
     --------------------------------------------------

  ==============================================================================
*/

#include <WiFi.h>
#include <FirebaseESP32.h>

// ------------------------------------------------------------------------------
// ⚙️ CONFIGURATION SECTION [MODIFIED BY PARTICIPANTS]
// ------------------------------------------------------------------------------

// [TODO 1]: Wi-Fi Credentials
#define WIFI_SSID     "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// [TODO 2]: Firebase Realtime Database Configuration
// Note: Database URL must NOT include 'https://' or trailing slashes '/'
// Example: "your-project-id-default-rtdb.firebaseio.com" or "your-project-id.firebaseio.com"
#define FIREBASE_HOST "YOUR_PROJECT_ID-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET_OR_API_KEY"

// [TODO 3]: ESP32 Hardware Pin Configuration
#define MQ_ANALOG_PIN  36     // ESP32 ADC pin (GPIO 36 / VP pin)
#define MQ_DIGITAL_PIN 4      // Optional digital alert pin (GPIO 4)

// [TODO 4]: Update Interval (in milliseconds)
const unsigned long SEND_INTERVAL = 3000; // Send data to Firebase every 3 seconds

// ------------------------------------------------------------------------------
// 📦 GLOBAL OBJECTS & VARIABLES
// ------------------------------------------------------------------------------

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastSendTime = 0;

// ------------------------------------------------------------------------------
// 🚀 SETUP FUNCTION (Runs Once)
// ------------------------------------------------------------------------------
void setup() {
  // Step 1: Initialize Serial Monitor for debugging
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("=========================================");
  Serial.println("🔥 ESP32 MQ Sensor Firebase Pipeline");
  Serial.println("=========================================");

  // Set pin modes
  pinMode(MQ_ANALOG_PIN, INPUT);
  pinMode(MQ_DIGITAL_PIN, INPUT);

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
  Serial.println("⏳ Warming up MQ sensor...");
}

// ------------------------------------------------------------------------------
// 🔁 MAIN LOOP FUNCTION (Runs Continuously)
// ------------------------------------------------------------------------------
void loop() {
  // Non-blocking timer to control data transmission rate
  if (millis() - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = millis();

    // Step 4: Read MQ Sensor Values (ESP32 ADC is 12-bit: 0 to 4095)
    int rawAnalogValue = analogRead(MQ_ANALOG_PIN);
    int digitalState = digitalRead(MQ_DIGITAL_PIN);

    // Convert raw 12-bit ADC reading (0-4095) to percentage
    float gasPercentage = (rawAnalogValue / 4095.0) * 100.0;

    // Print reading details to Serial Monitor
    Serial.println("-----------------------------------------");
    Serial.print("📊 Raw Analog Value (0-4095): ");
    Serial.println(rawAnalogValue);
    Serial.print("💨 Estimated Gas Level (%): ");
    Serial.print(gasPercentage);
    Serial.println("%");
    Serial.print("🚨 Digital Threshold Triggered: ");
    Serial.println(digitalState == LOW ? "YES (Gas Detected!)" : "NO (Normal)");

    // Step 5: Send Data to Firebase Database
    if (Firebase.setInt(firebaseData, "/sensor/gas_raw", rawAnalogValue)) {
      Serial.println("✅ Firebase Update: /sensor/gas_raw -> SUCCESS");
    } else {
      Serial.print("❌ Firebase Update Failed: ");
      Serial.println(firebaseData.errorReason());
    }

    if (Firebase.setFloat(firebaseData, "/sensor/gas_percent", gasPercentage)) {
      Serial.println("✅ Firebase Update: /sensor/gas_percent -> SUCCESS");
    } else {
      Serial.print("❌ Firebase Update Failed: ");
      Serial.println(firebaseData.errorReason());
    }

    if (Firebase.setBool(firebaseData, "/sensor/alert_triggered", digitalState == LOW)) {
      Serial.println("✅ Firebase Update: /sensor/alert_triggered -> SUCCESS");
    } else {
      Serial.print("❌ Firebase Update Failed: ");
      Serial.println(firebaseData.errorReason());
    }
  }
}
