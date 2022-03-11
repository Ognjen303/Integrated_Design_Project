import cv2
import numpy as np

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))
# hape of the image: (760, 1016, 3)

while True:
    r, f = stream.read()
    f = cv2.undistort(f, mtx, dist, None, newmtx)
    f = f[:, 200:800]
    # Convert BGR to HSV
    hsv = cv2.cvtColor(f, cv2.COLOR_BGR2HSV)
    # Define range of blue color in HSV
    sensitivity = 15
    lower_white = np.array([0,0,255-sensitivity])
    upper_white = np.array([255,sensitivity,255])
    # Threshold the HSV image to get only blue colors
    white_mask = cv2.inRange(hsv, lower_white, upper_white)
    # Bitwise-AND mask and original image
    white_res = cv2.bitwise_and(f,f, mask= white_mask)
    
    cv2.imshow('IP Camera stream', white_res)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()