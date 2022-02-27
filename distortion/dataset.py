from anyio import TypedAttributeLookupError
import cv2
import numpy
import time


cap = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

c=22400

while True:
    grabbed, frame = cap.read()
    if c%80==0:
        cv2.imwrite('distortion/imgs/%d.jpg' % (c + 400),frame)
        cv2.waitKey(1)

    c+=1