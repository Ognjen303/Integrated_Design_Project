import cv2
import numpy as np

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

# shape of the image: (760, 1016, 3)

while True:
    r, f = stream.read()
    # Convert BGR to HSV
    hsv = cv2.cvtColor(f, cv2.COLOR_BGR2HSV)
    # define range of blue color in HSV

    lower_red = np.array([159, 50, 70])
    upper_red = np.array([180, 255, 255])
    # Threshold the HSV image to get only red colors
    red_mask = cv2.inRange(hsv, lower_red, upper_red)
    red_mask[:380, :] = 0
    red_mask[380:, 508:] = 0

    # Bitwise-AND mask and original image
    red_res = cv2.bitwise_and(f,f, mask= red_mask)
    
    gray = cv2.cvtColor(red_res, cv2.COLOR_BGR2GRAY)
    ret,thresh = cv2.threshold(gray,127,255,0)

    contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    for c in contours:
        M = cv2.moments(c)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
        else:
            cX, cY = 0, 0
        cv2.circle(red_res, (cX, cY), 5, (255, 255, 255), -1)
        cv2.putText(red_res, "block!", (cX - 25, cY - 25),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)

    cv2.imshow('IP Camera stream', red_res)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()