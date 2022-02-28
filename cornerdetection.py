import cv2
import numpy as np

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

while True:
    r, f = stream.read()

    gray = cv2.cvtColor(f,cv2.COLOR_BGR2GRAY)
    gray = np.float32(gray)
    dst = cv2.cornerHarris(gray,2,3,0.04)
    #result is dilated for marking the corners, not important
    dst = cv2.dilate(dst,None)
    # Threshold for an optimal value, it may vary depending on the image.
    f[dst>0.01*dst.max()]=[0,0,255]
    cv2.imshow('IP Camera stream',f)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
