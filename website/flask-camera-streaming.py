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
	## mask of green (36,0,0) ~ (70, 255,255)
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        # four colors masks
        mask_red = cv2.inRange(hsv, (0, 50,70), (9,255, 255))
        mask_green = cv2.inRange(hsv, (36, 50,70), (89, 255, 255))
        mask_yellow = cv2.inRange(hsv,(25,50, 70),(35,255,255))
        mask_blue = cv2.inRange(hsv, (110,50,50), (130,255,255))
        # or gate
        mask = cv2.bitwise_or(mask_green,mask_yellow)
        mask = cv2.bitwise_or(mask,mask_red)
        mask = cv2.bitwise_or(mask,mask_blue)
        output =cv2.bitwise_and(image,image, mask = mask )  # 套用影像遮罩
        image = 255-output
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
