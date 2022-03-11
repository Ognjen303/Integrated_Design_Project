import cv2
import numpy as np
from collections import deque

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))
blue_pixels = []

while True:
    r, f = stream.read()
    f = cv2.undistort(f, mtx, dist, None, newmtx)
    f = f[:, 200:800]

    # Convert BGR to HSV
    hsv = cv2.cvtColor(f, cv2.COLOR_BGR2HSV)
    lower_red = np.array([159, 50, 70])
    upper_red = np.array([180, 255, 255])
    # Threshold the HSV image to get only red colors
    red_mask = cv2.inRange(hsv, lower_red, upper_red)
    red_mask[:, 150:] = 0
    red_mask[:520, :] = 0
    red_mask[690:, :] = 0

    ret,thresh = cv2.threshold(red_mask,127,255,0)

    # calculate moments of binary image
    M = cv2.moments(thresh)
    if M:
        # calculate x,y coordinate of center
        cX = int(M["m10"] / M["m00"])
        cY = int(M["m01"] / M["m00"])
        
    # put text and highlight the center
    cv2.circle(red_mask, (cX, cY), 5, (255, 255, 255), -1)
    cv2.putText(red_mask, "centroid", (cX - 25, cY - 25),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
    hsv = hsv[cY - 7:cY + 7, cX - 7:cX + 7]
    f = f[cY - 7:cY + 7, cX - 7:cX + 7]
    lower_blue = np.array([110,50,50])
    upper_blue = np.array([130,255,255])
    # Threshold the HSV image to get only blue colors
    blue_mask = cv2.inRange(hsv, lower_blue, upper_blue)
    # Bitwise-AND mask and original image
    #white_res = cv2.bitwise_and(f,f, mask= blue_mask)
    n_pixels = np.sum(blue_mask == 255)
    blue_pixels.append(n_pixels)

    cv2.imshow('IP Camera stream', red_mask)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    if len(blue_pixels) == 50:
        average = sum(blue_pixels) / 50
        print(average)
        if average < 15: #SUBJECT TO CHANGE
            print("RED")
            break
        else:
            print("BLUE")
            break

cv2.destroyAllWindows()
