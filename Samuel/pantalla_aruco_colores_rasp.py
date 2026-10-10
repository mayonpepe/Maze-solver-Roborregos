
import cv2
import serial
import time
import threading
from flask import Flask, Response
from picamera2 import Picamera2

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
time.sleep(2)

aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
params = cv2.aruco.DetectorParameters()
detector = cv2.aruco.ArucoDetector(aruco_dict, params)

picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"size": (640, 480)}))
picam2.start()

latest_frame = None
lock = threading.Lock()
app = Flask(__name__)

def camera_loop():
    global latest_frame
    last_sent = 0
    while True:
        frame = picam2.capture_array()
        frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)

        corners, ids, _ = detector.detectMarkers(frame)

        if ids is not None:
            cv2.aruco.drawDetectedMarkers(frame, corners, ids)
            now = time.time()
            if now - last_sent > 1:
                marker_id = str(ids.flatten()[0])
                ser.write((marker_id + '\n').encode())
                print(f"Enviado: {marker_id}")
                last_sent = now

        with lock:
            latest_frame = frame.copy()

def generate_mjpeg():
    while True:
        with lock:
            if latest_frame is None:
                continue
            frame = latest_frame.copy()
        _, jpeg = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 70])
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + jpeg.tobytes() + b'\r\n')
        time.sleep(0.05)

@app.route('/')
def index():
    return '<h1>Robot Cam</h1><img src="/stream" style="width:100%;max-width:800px">'

@app.route('/stream')
def stream():
    return Response(generate_mjpeg(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    t = threading.Thread(target=camera_loop, daemon=True)
    t.start()
    app.run(host='0.0.0.0', port=5000, threaded=True)