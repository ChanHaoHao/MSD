from flask import Flask, render_template, Response
import cv2
# import numpy as np
# import time

app = Flask(__name__)
# If you want to use the camera on the laptop
# camera = cv2.VideoCapture(0)
# If you want to use the usb camera
camera = cv2.VideoCapture(0)

def gen_frames():
    while True:
        success, frame = camera.read()
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

def complementary_color():
    while True:
        success, image = camera.read()
        image = 255-image
        # in_data = np.asarray(image)
        # lo = np.amin(in_data, axis=2, keepdims=True)
        # hi = np.amin(in_data, axis=2, keepdims=True)
        # out_data = (lo+hi)-in_data
        ret, buffer = cv2.imencode('.jpg', image)
        image = buffer.tobytes()
        yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + image + b'\r\n')
            

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/complement_feed')
def complement_feed():
    return Response(complementary_color(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    app.run('0.0.0.0')