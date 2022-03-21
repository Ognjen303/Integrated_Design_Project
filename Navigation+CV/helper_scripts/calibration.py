import apriltag
import cv2
import numpy as np

# CALIBRATION SCRIPT - PARALLEX ERROR CORRECTION
stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

# Load camera properties 
mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

r, f = stream.read()
f = cv2.undistort(f, mtx, dist, None, newmtx)
f = f[:, 200:800]
gray = cv2.cvtColor(f, cv2.COLOR_BGR2GRAY)
options = apriltag.DetectorOptions(families="tag36h11")
detector = apriltag.Detector()
results = detector.detect(gray)

for r in results:
    (ptA, ptB, _, _) = r.corners
    ptB = (int(ptB[0]), int(ptB[1]))
    ptA = (int(ptA[0]), int(ptA[1]))
    center_front_edge = (int((ptB[0] + ptA[0])/2), int((ptB[1] + ptA[1])/2))

print(center_front_edge)

'''
r, f = stream.read()
f = cv2.undistort(f, mtx, dist, None, newmtx)
f = f[:, 200:800]
cv2.circle(f, center_front_edge, 0,(0, 255,0),5)
cv2.imshow('Drop off points', f)
cv2.waitKey()
'''