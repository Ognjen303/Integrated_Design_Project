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
    i = f[50:390,500:830]
    f = f[:, 200:800]
    gray = cv2.cvtColor(i, cv2.COLOR_BGR2GRAY)
    blur = cv2.medianBlur(gray, 5)
    sharpen_kernel = np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]])
    sharpen = cv2.filter2D(blur, -1, sharpen_kernel)

    thresh = cv2.threshold(sharpen,160,255, cv2.THRESH_BINARY_INV)[1]
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3,3))
    close = cv2.morphologyEx(thresh, cv2.MORPH_CLOSE, kernel, iterations=2)
    close = cv2.bitwise_not(close)

    cnts = cv2.findContours(close, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cnts = cnts[0] if len(cnts) == 2 else cnts[1]

    min_area = 115*0.5 #blue is between 115 and 140, 0.5 is for safety (up to half the pixels)
    max_area = 180*2 #red between 130 and 180, 2 is for up to double the pixels

    counter = 0
    contour_centres = []
    for c in cnts:
        area = cv2.contourArea(c)
        if area > min_area and area < max_area:
            counter += 1
            #x,y,w,h = cv2.boundingRect(c)
            #cv2.rectangle(close, (x, y), (x + w, y + h), (36,255,12), 2)

            (x,y),radius = cv2.minEnclosingCircle(c)
            center = (int(x) + 300,int(y) + 50)
            contour_centres.append(center)
            #radius = int(radius)
            #v2.circle(i,center,radius,(150,120,255),2)
            #cv2.circle(i,center,0,(0,0,0),1)
    
    if counter == 4:
        drop_off = contour_centres
        drop_off.sort(key=lambda y: y[0])
        cv2.circle(f,drop_off[1],2,(150,120,255),2)
        cv2.imshow('Strean', f)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break


cv2.destroyAllWindows()
