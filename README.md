# 🏆 IoT & AI Hackathon Starter Templates & Materials

Welcome to the official repository for hackathon starter templates! This codebase provides ready-to-use, modular templates for students and participants building **AI/Computer Vision** and **IoT Smart Systems (ESP32)**.

---

## 📁 Repository Directory Structure

```text
Code and Materials/
├── 📄 README.md                             <-- Main Hackathon Overview & Guide
├── 🌐 demo local web dashboard.html         <-- Standalone Browser Simulator (No Hardware Needed)
├── 🧪 demo local server data show.ino       <-- ESP32 Test Code (Simulated Sensor Data)
├── 📁 Codes Template/
│   ├── 📁 Arduino and cloud server/
│   │   ├── 📄 Sensor with firebase connection.ino   <-- ESP32 + DHT11 + Water Level Sensor -> Firebase
│   │   └── 📄 readme.md                             <-- Step-by-Step Firebase Console Setup Guide
│   └── 📁 Arduino and local server/
│       ├── 📄 sensor with local server.ino          <-- ESP32 Standalone Web Server (Local IP UI)
│       └── 📄 readme.md                             <-- Local Web Server Guide
```

---

## 🤖 1. AI & Computer Vision Template

Located in `Codes Template/AI/object detection with mobile phone as cam.py`:
- **Real-Time YOLO Object Detection**: Compatible with YOLOv8 / YOLOv11 (`yolov8n.pt`, `yolo11n.pt`, or custom trained `.pt` models).
- **Mobile Camera Support**: Stream live video from your smartphone using IP Camera apps (IP Webcam / DroidCam) over Wi-Fi, or switch to built-in laptop webcams.
- **Multithreaded Performance**: Asynchronous background inference loop for high FPS.

---

## 📡 2. IoT & Hardware Templates (ESP32)

### A. ESP32 + Firebase Realtime Database (Cloud Mode)
- **File**: `Codes Template/Arduino and cloud server/Sensor with firebase connection.ino`
- **Sensors**: DHT11 (Temperature & Humidity) + Water Depth Sensor (ADC1 Pin GPIO 34).
- **Features**: Automatically creates database nodes (`/sensor/temperature`, `/sensor/humidity`, `/sensor/water_level_percent`, `/sensor/water_detected`) and pushes real-time telemetry to the Firebase Cloud.
- **Guide**: Read `Codes Template/Arduino and cloud server/readme.md` for Firebase Console setup.

### B. ESP32 Standalone Local Web Server (Offline Mode)
- **File**: `Codes Template/Arduino and local server/sensor with local server.ino`
- **Features**: Hosts a responsive, dark-mode HTML & CSS web dashboard directly on the ESP32's local IP address (`http://192.168.x.x`).
- **Live Updates**: Includes a lightweight JSON API endpoint (`/api/data`) that updates telemetry every 2 seconds without full page reloads.

### C. Browser Simulator (No Microcontroller Required)
- **File**: `demo local web dashboard.html`
- **Features**: Single-file HTML/JS simulator. Double-click to open in any web browser (Chrome, Edge, Firefox) to present or test the web dashboard UI without physical hardware.

---

## 🚀 Quick Start for Hackathon Participants

1. **For IoT Projects**:
   - Install **Arduino IDE**.
   - Install required ESP32 libraries: `Firebase ESP32 Client`, `DHT sensor library`, `Adafruit Unified Sensor`, `ArduinoJson`.
   - Update `WIFI_SSID` & `WIFI_PASSWORD` in the `.ino` files.

2. **For AI Projects**:
   - Install Python 3.9+ and dependencies:
     ```bash
     pip install opencv-python ultralytics numpy
     ```
   - Run `python "object detection with mobile phone as cam.py"`.

---

## 📝 License & Attribution
Designed as an educational starter kit for student hackathons. Feel free to modify and adapt for your team projects!
