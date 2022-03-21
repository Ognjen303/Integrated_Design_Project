import cv2 as cv
import numpy as np

# BLOCK DETECTION FUNCTION TESTING+ITERATION SCRIPT

stream = cv.VideoCapture('http://localhost:8081/stream/video.mjpeg')

mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

def detect_block(s):
    """s:stream
       returns (x, y) of the block"""
    frame_counter = 0
    while frame_counter <= 20:
        frame_counter += 1
        r, f = s.read() 
        f = cv.undistort(f, mtx, dist, None, newmtx)
        f = f[535:675, 200:335] #TIGHT BOX ROI with cropped image
        f_hsv=cv.cvtColor(f, cv.COLOR_BGR2HSV)
        # lower mask (0-10)
        lower_red = np.array([0,50,50])
        upper_red = np.array([20,255,255]) #10. 255, 255 to be safe
        mask0 = cv.inRange(f_hsv, lower_red, upper_red)
        # upper mask (170-180)
        lower_red = np.array([170,50,50])
        upper_red = np.array([180,255,255])
        mask1 = cv.inRange(f_hsv, lower_red, upper_red)
        # join my masks
        mask = mask0+mask1
        # set my output img to zero everywhere except my mask
        output_f = f.copy()
        output_f[np.where(mask==0)] = 0
        gray = cv.cvtColor(output_f, cv.COLOR_BGR2GRAY)
        blur = cv.medianBlur(gray, 5)
        sharpen_kernel = np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]])
        sharpen = cv.filter2D(blur, -1, sharpen_kernel)

        thresh = cv.threshold(sharpen,20,255, cv.THRESH_BINARY_INV)[1]
        kernel = cv.getStructuringElement(cv.MORPH_RECT, (3,3))
        close = cv.morphologyEx(thresh, cv.MORPH_CLOSE, kernel, iterations=2)
        close = cv.bitwise_not(close)
        cv.imshow('close', thresh)
        cv.waitKey()
        cv.imshow('close', close)
        cv.waitKey()
        cnts = cv.findContours(close, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
        cnts = cnts[0] if len(cnts) == 2 else cnts[1]

        min_area = 5 #expect approx 70
        max_area = 150 
        if len(cnts) == 1:
            center_list = []
            for c in cnts:
                area = cv.contourArea(c)
                if area > min_area and area < max_area: 
                    (x,y),radius = cv.minEnclosingCircle(c)
                    center = (int(x),int(y) + 535)
                    center_list.append(center)
            if len(center_list) == 1: 
                return center_list[0]
            else:
                center_list = []
        else:
            pass

        return (83, 590) # Default value if the block is not found

while True:
    r, f = stream.read()
    coord = detect_block(stream)
    f = cv.undistort(f, mtx, dist, None, newmtx)
    f = f[:, 200:800]
    cv.circle(f, coord, 0,(0, 0, 255),5)
    cv.imshow('Frame', f)
    cv.waitKey()