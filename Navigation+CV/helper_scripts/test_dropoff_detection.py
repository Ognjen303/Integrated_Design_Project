import cv2 as cv
import numpy as np

# DROP OFF DETECTION FUNCTION TESTING+ITERATION SCRIPT

stream = cv.VideoCapture('http://localhost:8081/stream/video.mjpeg')

mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

def detect_dropoff(s):
    """s:stream
    returns [(x, y), (x,y) ...] of the drop_off_locations - bottom blue, top blue, top red, bottom red"""
    frame_counter = 0
    threshold = 230 # Try different values 
    drop_off = None
    while frame_counter <= 10:
        frame_counter += 1
        r, f = s.read()
        f = cv.undistort(f, mtx, dist, None, newmtx)
        f = f[:, 200:800]
        blues = f[140:250, 280:430]
        reds = f[300:420, 400:550]

        gray_b = cv.cvtColor(blues, cv.COLOR_BGR2GRAY)
        gray_r = cv.cvtColor(reds, cv.COLOR_BGR2GRAY)
        blur_b = cv.medianBlur(gray_b, 5)
        blur_r = cv.medianBlur(gray_r, 5)
        sharpen_kernel = np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]])
        sharpen_b = cv.filter2D(blur_b, -1, sharpen_kernel)
        sharpen_r = cv.filter2D(blur_r, -1, sharpen_kernel)
        
        thresh_b = cv.threshold(sharpen_b,threshold,255, cv.THRESH_BINARY_INV)[1]
        thresh_r = cv.threshold(sharpen_r,threshold,255, cv.THRESH_BINARY_INV)[1]
        thresh_b = cv.bitwise_not(thresh_b)
        thresh_r = cv.bitwise_not(thresh_r)

        cnts_r = cv.findContours(thresh_r, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
        cnts_r = cnts_r[0] if len(cnts_r) == 2 else cnts_r[1]

        cnts_b = cv.findContours(thresh_b, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
        cnts_b = cnts_b[0] if len(cnts_b) == 2 else cnts_b[1]

        min_area = 50 #blue is between 115 and 140, 0.5 is for safety (up to half the pixels)
        max_area = 220 #red between 130 and 180, 2 is for up to double the pixels

        centers = []
        for c in cnts_b:
            area = cv.contourArea(c)
            if area > min_area and area < max_area:
                (x,y),radius = cv.minEnclosingCircle(c)
                center = (int(x) + 280 ,int(y) + 140)
                centers.append(center)

        for c in cnts_r:
            area = cv.contourArea(c)
            if area > min_area and area < max_area:
                (x,y),radius = cv.minEnclosingCircle(c)
                center = (int(x) + 400 ,int(y) + 300)
                centers.append(center)

        if len(centers) == 4:
            drop_off = centers
            drop_off.sort(key=lambda y: y[0])
            return [drop_off[0], drop_off[1], drop_off[3], drop_off[2]] 
        
        return [(313, 222), (375, 166), (525, 331), (467, 389)] # Default values 

r, f = stream.read()
f = cv.undistort(f, mtx, dist, None, newmtx)
f = f[:, 200:800]
drop_off = detect_dropoff(stream)
print(drop_off)
cv.circle(f, drop_off[0], 0,(255, 0,0),5)
cv.circle(f, drop_off[1], 0,(0, 255,0),5)
cv.circle(f, drop_off[2], 0,(0,0,255),5)
cv.circle(f, drop_off[3], 0,(255, 255, 0),5)
cv.imshow('Drop-off', f)
cv.waitKey()