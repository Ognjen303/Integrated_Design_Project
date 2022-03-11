import cv2
import numpy as np
import math

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

while True:
    r, f = stream.read()
    gray = cv2.cvtColor(f,cv2.COLOR_BGR2GRAY)
    gray = np.float32(gray)
    edges = cv2.Canny(f, 50, 200, None, 3)
    cdst = cv2.cvtColor(edges, cv2.COLOR_GRAY2BGR)

    cv2.imshow('IP Camera stream', edges)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
