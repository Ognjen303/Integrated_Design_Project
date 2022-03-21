import cv2
import numpy as np

# CHECKPOINT VISUALISATION SCRIPT

# Load camera properties 
mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

original_destinations = [(421, 281), (281, 394), (149, 514)] 
starting_location = (532, 175)
drop_off_locations = [(315, 211), (381, 151),(536, 325), (475, 385)] 

r, f = stream.read()
f = cv2.undistort(f, mtx, dist, None, newmtx)
f = f[:, 200:800]
cv2.circle(f, drop_off_locations[0], 0,(0, 255,0),5) 

cv2.imshow('Checkpoint', f)
cv2.waitKey()