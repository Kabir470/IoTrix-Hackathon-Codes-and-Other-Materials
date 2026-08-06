# 🌐 ESP32 Local Web Server (No Cloud Required)

This template turns your ESP32 into a standalone Wi-Fi Web Server hosting a live HTML/CSS dashboard directly from the board.

---

## 🔌 Hardware Wiring Guide for ESP32

| Sensor Module | Sensor Pin | ESP32 Board Pin | Notes |
|---|---|---|---|
| **DHT11** | VCC | 3.3V / 5V | Power (+) |
| **DHT11** | GND | GND | Ground (-) |
| **DHT11** | DATA / OUT | **GPIO 4** | Digital Data Pin |
| **Water Level** | VCC / (+) | 3.3V / 5V | Power (+) |
| **Water Level** | GND / (-) | GND | Ground (-) |
| **Water Level** | S / OUT (Signal) | **GPIO 34** | Must use ADC1 (GPIO 34) |

---

## ⚡ How to Run
1. Open `sensor with local server.ino` in Arduino IDE.
2. Fill in your Wi-Fi credentials:
   ```cpp
   #define WIFI_SSID     "YOUR_WIFI_NAME"
   #define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
   ```
3. Upload to your **ESP32 Dev Module**.
4. Open the **Serial Monitor (115200 baud)**.
5. Copy the IP address printed in the Serial Monitor (e.g. `http://192.168.1.50`).
6. Connect your phone or laptop to the **same Wi-Fi network**, open your web browser, and navigate to the IP address!

---

## 🌐 Features
- **Embedded Responsive UI**: Modern dark-mode dashboard styled with CSS stored directly in ESP32 Flash memory (`PROGMEM`).
- **Live JavaScript Auto-Refresh**: Uses a lightweight JSON API endpoint (`/api/data`) to update temperature, humidity, and water level values every 2 seconds without reloading the page.
- **Offline / Cloud-Free**: Works completely locally without needing an active internet connection or Firebase setup.
