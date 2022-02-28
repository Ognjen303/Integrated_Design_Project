import cv2
import apriltag
import math
import numpy as np
#maybe try: https://docs.opencv.org/3.4/d9/d6a/group__aruco.html

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')

mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

options = apriltag.DetectorOptions(families='tag36h11',
                                 border=1,
                                 nthreads=4,
                                 quad_decimate=1.0,
                                 quad_blur=0.0,
                                 refine_edges=True,
                                 refine_decode=False,
                                 refine_pose=False,
                                 debug=False,
                                 quad_contours=True)

detector = apriltag.Detector(options)

while True:
    r, f = stream.read()
    f = cv2.undistort(f, mtx, dist, None, newmtx)
    gray = cv2.cvtColor(f,cv2.COLOR_BGR2GRAY)
    results = detector.detect(gray)
    # loop over the AprilTag detection results
    
    for r in results:
        # extract the bounding box (x, y)-coordinates for the AprilTag
        # and convert each of the (x, y)-coordinate pairs to integers
        (ptA, ptB, ptC, ptD) = r.corners
        ptB = (int(ptB[0]), int(ptB[1]))
        ptC = (int(ptC[0]), int(ptC[1]))
        ptD = (int(ptD[0]), int(ptD[1]))
        ptA = (int(ptA[0]), int(ptA[1]))

        # draw the bounding box of the AprilTag detection
        cv2.line(f, ptA, ptB, (0, 255, 0), 2) #f
        cv2.line(f, ptB, ptC, (0, 255, 0), 2)
        cv2.line(f, ptC, ptD, (0, 255, 0), 2)
        cv2.line(f, ptD, ptA, (0, 255, 0), 2)

        # draw the center (x, y)-coordinates of the AprilTag
        (cX, cY) = (int(r.center[0]), int(r.center[1]))
        cv2.circle(f, (cX, cY), 5, (0, 0, 255), -1)
    
    cv2.imshow('IP Camera stream', f)
    cv2.imwrite('testpose.jpg', f)
        
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()