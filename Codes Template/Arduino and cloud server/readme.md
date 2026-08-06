# 🚀 ESP32 Firebase Realtime Database Setup Guide

Complete step-by-step guide to setting up Firebase Realtime Database for ESP32 hackathon projects.

---

## 📋 Step 1: Create a Firebase Project
1. Go to the [Firebase Console](https://console.firebase.google.com/).
2. Click **Create a project** (or **Add project**).
3. Enter your project name (e.g., `ESP32-IoT-Hackathon`).
4. (Optional) Toggle off Google Analytics for faster setup.
5. Click **Create project** and wait a few seconds until it says *"Your new project is ready"*.
6. Click **Continue**.

---

## 🗄️ Step 2: Create the Realtime Database
1. In the left-hand navigation sidebar, click **Build** > **Realtime Database**.
2. Click **Create Database**.
3. Select your Database location (leave default e.g., `United States (us-central1)`). Click **Next**.
4. Choose **Start in test mode** -> Click **Enable**.

---

## 🔓 Step 3: Configure Database Security Rules
By default, Firebase blocks unauthorized read/write requests. For hackathons, set public permissions so ESP32 can send data without OAuth authentication:

1. Inside your **Realtime Database** panel, click on the **Rules** tab at the top.
2. Replace the JSON rules with:
   ```json
   {
     "rules": {
       ".read": true,
       ".write": true
     }
   }
   ```
3. Click **Publish** at the top right.

> ⚠️ **Security Warning**: After the hackathon ends, set `.read` and `.write` back to `false` to prevent unauthorized access.

---

## 🌐 Step 4: Get Your Database URL (`FIREBASE_HOST`)
1. Click on the **Data** tab at the top of the Realtime Database panel.
2. Look at the reference URL at the top of your database tree (it looks like `https://my-project-default-rtdb.firebaseio.com/`).
3. **Copy the URL and remove `https://` and trailing slashes `/`**:
   - ❌ `https://my-project-default-rtdb.firebaseio.com/`
   - ✅ `my-project-default-rtdb.firebaseio.com`
4. Paste this cleaned string into your Arduino sketch:
   ```cpp
   #define FIREBASE_HOST "my-project-default-rtdb.firebaseio.com"
   ```

---

## 🔑 Step 5: Get Your Database Secret Key (`FIREBASE_AUTH`)
1. Click the **Gear Icon ⚙️** next to **Project Overview** in the left sidebar -> Select **Project settings**.
2. Go to the **Service accounts** tab at the top.
3. Click **Database secrets** in the left sub-menu.
4. Hover over your secret key and click **Show**.
5. Copy the long secret string.
6. Paste this key into your Arduino sketch:
   ```cpp
   #define FIREBASE_AUTH "Your_Copied_Secret_Key_Here"
   ```

---

## ⚡ Step 6: Flash the Code to ESP32
1. Open `Sensor with firebase connection.ino` in Arduino IDE.
2. Fill in:
   - `WIFI_SSID`
   - `WIFI_PASSWORD`
   - `FIREBASE_HOST`
   - `FIREBASE_AUTH`
3. Connect ESP32 via USB -> Select **Tools > Board > ESP32 Dev Module**.
4. Select the correct **COM Port**.
5. Click **Upload ➔**.
6. Open **Serial Monitor (115200 baud)** to view real-time data being pushed to Firebase!
