import cv2
import numpy as np
import time
import threading
from ultralytics import YOLO

# ==============================================================================
# 🎯 HACKATHON STARTER TEMPLATE: Real-Time Object Detection (Mobile Cam / Webcam)
# ==============================================================================
# 
# 📌 Instructions for Participants:
# 1. Install dependencies:  pip install opencv-python ultralytics numpy
# 2. To use your Mobile Phone as a Camera:
#    - Download an IP Camera app on your mobile phone (e.g., "IP Webcam" for Android or "DroidCam").
#    - Ensure your phone and computer are connected to the SAME Wi-Fi network.
#    - Start the server on the mobile app and copy the video URL (e.g., http://192.168.1.100:8080/video).
# 3. Modify the configuration section below marked with [TODO] according to your hackathon project requirements.
# ==============================================================================

# ------------------------------------------------------------------------------
# ⚙️ CONFIGURATION SECTION [MODIFIED BY PARTICIPANTS]
# ------------------------------------------------------------------------------

# [TODO 1]: Camera Input Source
# For Built-in / USB Webcam: set CAMERA_SOURCE = 0 (or 1, 2 depending on port)
# For Mobile Phone IP Camera: set CAMERA_SOURCE = "http://<YOUR_MOBILE_IP>:8080/video"
CAMERA_SOURCE = "http://192.168.0.105:8080/video"  # Example IP: "http://192.168.0.105:8080/video"

# [TODO 2]: Model Selection & Configuration
# You can use standard pre-trained models: 'yolov8n.pt', 'yolo11n.pt'
# Or pass your custom trained model path: 'best.pt' or 'my_custom_model.pt'
MODEL_PATH = 'yolo11n.pt'

# Confidence threshold (0.0 to 1.0) - filter out weak detections
CONFIDENCE_THRESHOLD = 0.5

# Non-Maximum Suppression (IoU) threshold - control overlapping boxes
NMS_THRESHOLD = 0.45

# [TODO 3]: Custom Class Names (Optional)
# Leave as None to automatically use the class labels embedded inside the model.
# Or specify a list of class names if using a custom model with specific index ordering:
# CLASS_NAMES = ['person', 'car', 'dog']
CLASS_NAMES = None

# [TODO 4]: Performance & Display Optimization Settings
SKIP_FRAMES = 2       # Process object detection every Nth frame to reduce lag (1 = process every frame)
SCALE_PERCENT = 60    # Scale factor (%) for the display window (e.g., 50% = half resolution for speed)

# ------------------------------------------------------------------------------
# 📦 MODEL & CAMERA INITIALIZATION
# ------------------------------------------------------------------------------

print("🚀 Initializing Object Detection Pipeline...")
print(f"📦 Loading YOLO model: '{MODEL_PATH}'...")

try:
    model = YOLO(MODEL_PATH)
    print("✅ Model loaded successfully!")
    
    # Auto-assign class names if not explicitly defined by participant
    if CLASS_NAMES is None:
        CLASS_NAMES = model.names
except Exception as e:
    print(f"❌ Error loading model: {e}")
    print("📝 Tip: Make sure the model path is correct or check your internet connection for pre-trained models.")
    exit()

# Global variables for multi-threaded inference
latest_detections = []
detection_lock = threading.Lock()
frame_count = 0

def process_detection(frame):
    """
    Runs YOLO inference in a background thread to maintain high frame rate (FPS) 
    in the main UI thread.
    """
    global latest_detections
    
    # Run YOLO object detection
    results = model(frame, conf=CONFIDENCE_THRESHOLD, iou=NMS_THRESHOLD, verbose=False)
    
    detections = []
    
    for result in results:
        if result.boxes is not None:
            for box in result.boxes:
                # Bounding box coordinates
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
                
                # Confidence score
                confidence = float(box.conf[0])
                
                # Class index and class name lookup
                class_id = int(box.cls[0])
                if isinstance(CLASS_NAMES, dict):
                    class_name = CLASS_NAMES.get(class_id, f"class_{class_id}")
                elif isinstance(CLASS_NAMES, list) and class_id < len(CLASS_NAMES):
                    class_name = CLASS_NAMES[class_id]
                else:
                    class_name = f"class_{class_id}"
                
                # [TODO 5]: Add Custom Trigger / Action Logic Here
                # Example: Trigger an alert or API call when a specific target object is detected
                # if class_name.lower() == 'target_object':
                #     print(f"⚠️ TARGET DETECTED: {class_name} ({confidence:.2f})")
                
                detections.append({
                    'class_id': class_id,
                    'class_name': class_name,
                    'confidence': confidence,
                    'coords': (x1, y1, x2, y2)
                })
    
    # Thread-safe update of latest detection results
    with detection_lock:
        latest_detections = detections.copy()

# ------------------------------------------------------------------------------
# 📹 VIDEO STREAM & MAIN PROCESSING LOOP
# ------------------------------------------------------------------------------

print(f"🎥 Connecting to Camera Source: {CAMERA_SOURCE} ...")
cap = cv2.VideoCapture(CAMERA_SOURCE)

# Reduce frame buffer latency for live video streams
cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

if not cap.isOpened():
    print("❌ Error: Could not open the video stream.")
    print("📝 Troubleshooting:")
    print("   - For Mobile Cam: Check IP address, port, and ensure phone & PC share the same Wi-Fi.")
    print("   - For Webcam: Ensure index 0 is valid and not used by another app.")
    exit()

print("✅ Camera feed connected successfully!")
print("💡 Press 'q' key in the video window to exit.")

ret = True
detection_thread = None

while ret:
    start_time = time.time()
    ret, frame = cap.read()
    
    if ret:
        frame_count += 1
        
        # Calculate scaled dimensions for display window
        width = int(frame.shape[1] * SCALE_PERCENT / 100)
        height = int(frame.shape[0] * SCALE_PERCENT / 100)
        dim = (width, height)
        display_frame = cv2.resize(frame, dim, interpolation=cv2.INTER_AREA)
        
        # Trigger detection frame processing asynchronously
        if frame_count % SKIP_FRAMES == 0:
            if detection_thread is None or not detection_thread.is_alive():
                detection_thread = threading.Thread(target=process_detection, args=(frame.copy(),))
                detection_thread.daemon = True
                detection_thread.start()
        
        # Draw latest detection bounding boxes and labels on display frame
        with detection_lock:
            for detection in latest_detections:
                x1, y1, x2, y2 = detection['coords']
                class_name = detection['class_name']
                confidence = detection['confidence']
                
                # Coordinate scaling mapping original frame size to display frame size
                frame_height, frame_width = frame.shape[:2]
                scale_x = width / frame_width
                scale_y = height / frame_height
                
                x1_scaled = int(x1 * scale_x)
                y1_scaled = int(y1 * scale_y)
                x2_scaled = int(x2 * scale_x)
                y2_scaled = int(y2 * scale_y)
                
                # Bounding box color (B, G, R)
                box_color = (0, 255, 0)  # Green default
                
                # Draw bounding box rectangle
                cv2.rectangle(display_frame, (x1_scaled, y1_scaled), (x2_scaled, y2_scaled), box_color, 2)
                
                # Draw label background & text
                label = f"{class_name}: {confidence:.2f}"
                cv2.putText(display_frame, label, (x1_scaled, max(y1_scaled - 10, 20)),
                           cv2.FONT_HERSHEY_SIMPLEX, 0.5, box_color, 2)
        
        # Calculate & display live FPS
        elapsed_time = time.time() - start_time
        fps = 1.0 / elapsed_time if elapsed_time > 0 else 0.0
        cv2.putText(display_frame, f"FPS: {fps:.1f}", (10, 30),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)
        
        # Render frame in window
        cv2.imshow('Object Detection Starter Template', display_frame)
        
        # Press 'q' to stop execution
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    else:
        print("⚠️ Warning: Failed to read frame from stream.")
        break

# ------------------------------------------------------------------------------
# 🧹 CLEANUP RESOURCES
# ------------------------------------------------------------------------------
cap.release()
cv2.destroyAllWindows()
print("🛑 Video stream closed. Program terminated.")
