/*
  ==============================================================================
  🎯 HACKATHON STARTER TEMPLATE: ESP32 Local Web Server (DHT11 + Water Sensor)
  ==============================================================================

  📌 Microcontroller: ESP32 Dev Module / ESP32 WROOM
  
  📦 Required Libraries (Install via Arduino Library Manager):
     1. "DHT sensor library" by Adafruit
     2. "Adafruit Unified Sensor" by Adafruit
     (Built-in libraries used: <WiFi.h>, <WebServer.h>)

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
#include <WebServer.h>
#include "DHT.h"

// ------------------------------------------------------------------------------
// ⚙️ CONFIGURATION SECTION [MODIFIED BY PARTICIPANTS]
// ------------------------------------------------------------------------------

// [TODO 1]: Wi-Fi Network Credentials (ESP32 connects to your local router)
#define WIFI_SSID     "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// [TODO 2]: Hardware Pin Configuration
#define DHTPIN           4        // GPIO pin for DHT11 data
#define DHTTYPE          DHT11    // Sensor type
#define WATER_SENSOR_PIN 34       // Analog ADC1 pin for Water Level (GPIO 34)

const int WATER_THRESHOLD = 500;  // Threshold for water detection alert

// ------------------------------------------------------------------------------
// 📦 GLOBAL OBJECTS & INITIALIZATION
// ------------------------------------------------------------------------------

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80); // Web server running on HTTP standard port 80

// Global variables to store sensor readings
float temperature = 0.0;
float humidity = 0.0;
int rawWaterLevel = 0;
float waterPercentage = 0.0;
bool waterDetected = false;

// ------------------------------------------------------------------------------
// 🎨 HTML & CSS DASHBOARD TEMPLATE (Stored in Flash Memory)
// ------------------------------------------------------------------------------
const char HTML_DASHBOARD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Local Sensor Dashboard</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
    body { background: #0f172a; color: #f8fafc; display: flex; flex-direction: column; align-items: center; min-height: 100vh; padding: 20px; }
    header { margin-top: 20px; text-align: center; }
    header h1 { font-size: 2rem; color: #38bdf8; margin-bottom: 5px; }
    header p { color: #94a3b8; font-size: 0.95rem; }
    .status-badge { background: #1e293b; border: 1px solid #334155; padding: 6px 14px; border-radius: 20px; font-size: 0.85rem; color: #4ade80; display: inline-block; margin-top: 10px; }
    
    .grid-container { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; width: 100%; max-width: 900px; margin-top: 30px; }
    
    .card { background: #1e293b; border: 1px solid #334155; border-radius: 16px; padding: 25px; text-align: center; transition: transform 0.2s, border-color 0.2s; box-shadow: 0 10px 25px -5px rgba(0,0,0,0.3); }
    .card:hover { transform: translateY(-5px); border-color: #38bdf8; }
    .card .icon { font-size: 2.5rem; margin-bottom: 10px; }
    .card .label { font-size: 0.9rem; color: #94a3b8; text-transform: uppercase; letter-spacing: 1px; }
    .card .value { font-size: 2.5rem; font-weight: bold; margin: 10px 0; color: #f8fafc; }
    .card .unit { font-size: 1.2rem; color: #94a3b8; }
    
    .alert-card { grid-column: 1 / -1; background: #1e293b; border: 1px solid #334155; }
    .alert-box { padding: 15px; border-radius: 10px; font-weight: bold; text-align: center; }
    .alert-dry { background: rgba(74, 222, 128, 0.1); color: #4ade80; border: 1px solid rgba(74, 222, 128, 0.3); }
    .alert-wet { background: rgba(248, 113, 113, 0.15); color: #f87171; border: 1px solid rgba(248, 113, 113, 0.4); animation: pulse 1.5s infinite; }
    
    @keyframes pulse { 0% { opacity: 1; } 50% { opacity: 0.6; } 100% { opacity: 1; } }
    footer { margin-top: auto; padding: 20px 0; color: #64748b; font-size: 0.85rem; }
  </style>
</head>
<body>

  <header>
    <h1>⚡ ESP32 Local Server Dashboard</h1>
    <p>Live Real-Time Sensor Telemetry</p>
    <div class="status-badge">● System Online</div>
  </header>

  <div class="grid-container">
    <!-- Temperature Card -->
    <div class="card">
      <div class="icon">🌡️</div>
      <div class="label">Temperature</div>
      <div class="value"><span id="temp">--</span><span class="unit"> °C</span></div>
    </div>

    <!-- Humidity Card -->
    <div class="card">
      <div class="icon">💧</div>
      <div class="label">Humidity</div>
      <div class="value"><span id="humidity">--</span><span class="unit"> %</span></div>
    </div>

    <!-- Water Level Card -->
    <div class="card">
      <div class="icon">🌊</div>
      <div class="label">Water Depth Level</div>
      <div class="value"><span id="water">--</span><span class="unit"> %</span></div>
    </div>

    <!-- Water Detection Alert Card -->
    <div class="card alert-card">
      <div id="alertBox" class="alert-box alert-dry">Checking Water Sensor Status...</div>
    </div>
  </div>

  <footer>ESP32 Local Web Server | Hackathon Starter Kit</footer>

  <script>
    // Automatic Live Data Update using AJAX/JSON API fetch
    function updateSensorData() {
      fetch('/api/data')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temp').innerText = data.temperature.toFixed(1);
          document.getElementById('humidity').innerText = data.humidity.toFixed(1);
          document.getElementById('water').innerText = data.water_percentage.toFixed(1);

          const alertBox = document.getElementById('alertBox');
          if (data.water_detected) {
            alertBox.innerText = '🚨 WARNING: WATER DETECTED!';
            alertBox.className = 'alert-box alert-wet';
          } else {
            alertBox.innerText = '✅ NORMAL: DRY ENVIRONMENT';
            alertBox.className = 'alert-box alert-dry';
          }
        })
        .catch(err => console.error("Error fetching data:", err));
    }

    // Refresh telemetry every 2 seconds
    setInterval(updateSensorData, 2000);
    updateSensorData();
  </script>

</body>
</html>
)rawliteral";

// ------------------------------------------------------------------------------
// 🛠️ SENSOR READING HELPER
// ------------------------------------------------------------------------------
void readSensors() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  if (!isnan(t)) temperature = t;
  if (!isnan(h)) humidity = h;

  rawWaterLevel = analogRead(WATER_SENSOR_PIN);
  waterPercentage = (rawWaterLevel / 4095.0) * 100.0;
  waterDetected = (rawWaterLevel > WATER_THRESHOLD);
}

// ------------------------------------------------------------------------------
// 🌐 WEB SERVER ROUTE HANDLERS
// ------------------------------------------------------------------------------

// Handler for Main HTML Web Page Route ("/")
void handleRoot() {
  readSensors();
  server.send(200, "text/html", HTML_DASHBOARD);
}

// Handler for JSON Data API Endpoint ("/api/data")
void handleApiData() {
  readSensors();

  String json = "{";
  json += "\"temperature\":" + String(temperature, 1) + ",";
  json += "\"humidity\":" + String(humidity, 1) + ",";
  json += "\"raw_water\":" + String(rawWaterLevel) + ",";
  json += "\"water_percentage\":" + String(waterPercentage, 1) + ",";
  json += "\"water_detected\":" + String(waterDetected ? "true" : "false");
  json += "}";

  server.send(200, "application/json", json);
}

// Handler for unknown 404 routes
void handleNotFound() {
  server.send(404, "text/plain", "404: Route Not Found");
}

// ------------------------------------------------------------------------------
// 🚀 SETUP FUNCTION
// ------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("==================================================");
  Serial.println("🌐 ESP32 Local Web Server (DHT11 + Water Sensor)");
  Serial.println("==================================================");

  // Initialize sensors
  dht.begin();
  pinMode(WATER_SENSOR_PIN, INPUT);

  // Connect to Local Wi-Fi Network
  Serial.print("📶 Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("✅ Wi-Fi Connected Successfully!");
  Serial.print("📍 Open Browser at: http://");
  Serial.println(WiFi.localIP());

  // Setup Web Server Routes
  server.on("/", handleRoot);            // Main Dashboard page
  server.on("/api/data", handleApiData); // JSON API endpoint for live JavaScript updates
  server.onNotFound(handleNotFound);

  // Start HTTP Server
  server.begin();
  Serial.println("🚀 Local Web Server Started!");
}

// ------------------------------------------------------------------------------
// 🔁 MAIN LOOP FUNCTION
// ------------------------------------------------------------------------------
void loop() {
  // Keep listening for incoming HTTP requests from web browsers
  server.handleClient();
}
