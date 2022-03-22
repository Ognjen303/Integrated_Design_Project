import cv2
import numpy
import time

# Run this script to save a picture of the stream periodically for calibration 
stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')
counter=0

while True:
    r, f = stream.read()
    if counter%100==0:
        cv2.imwrite('distortion/imgs/%d.jpg' % (counter),f)
        cv2.waitKey(1)
    counter+=1
