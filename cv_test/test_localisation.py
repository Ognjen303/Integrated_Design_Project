import cv2 
import numpy as np 
import matplotlib.pyplot as plt
from collections import deque
import math

apriltag_dict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_APRILTAG_36h11)
aruco_parameters = cv2.aruco.DetectorParameters_create()

axis = np.float32([[-5,-5,0], [-5,5,0], [5,5,0], [5,-5,0],
                   [-5,-5,10],[-5,5,10],[5,5,10],[5,-5,10] ])

board = cv2.aruco.GridBoard_create(
        markersX=1,
        markersY=1,
        markerLength=0.09,
        markerSeparation=0.01,
        dictionary= apriltag_dict)

mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')

mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

frame = cv2.imread("/Users/adhi/Desktop/IDP/cv_test/mon4.jpeg")

gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

# Detect ArUco markers in the video frame
(corners, ids, rejected) = cv2.aruco.detectMarkers(
gray, apriltag_dict, parameters=aruco_parameters)

corners, ids, rejected, recoveredIds = cv2.aruco.refineDetectedMarkers(
            image = gray,
            board = board,
            detectedCorners = corners,
            detectedIds = ids,
            rejectedCorners = rejected,
            cameraMatrix = mtx,
            distCoeffs = dist)   

# Check that at least one ArUco marker was detected
if len(corners) > 0:
  
  frame = cv2.aruco.drawDetectedMarkers(frame, corners, borderColor=(0, 255, 0))

  rotation_vectors, translation_vectors, _objPoints = cv2.aruco.estimatePoseSingleMarkers(corners, 1, newmtx, dist)
  R, _ = cv2.Rodrigues(rotation_vectors)
  
  x_unit = np.array([3, 0, 0])
  x_rotated = R.dot(x_unit)
  x_vector = [x_rotated[0], x_rotated[1]]

  V = np.array([[x_rotated[0], x_rotated[1]]])
  origin = np.array([[0, 0, 0],[0, 0, 0]]) # origin point
  plt.quiver(*origin, V[:,0], V[:,1], color=['r','b','g'], scale=21)
  plt.show()

  for rvec, tvec in zip(rotation_vectors, translation_vectors):
    frame = cv2.aruco.drawAxis(frame, newmtx, dist, rvec, tvec, 3)

lower_red = np.array([159, 50, 70])
upper_red = np.array([180, 255, 255])
# Threshold the HSV image to get only red colors
red_mask = cv2.inRange(hsv, lower_red, upper_red)
red_mask[:380, :] = 0
red_mask[380:, 508:] = 0

# Bitwise-AND mask and original image
red_res = cv2.bitwise_and(frame, frame, mask= red_mask)

gray = cv2.cvtColor(red_res, cv2.COLOR_BGR2GRAY)
ret,thresh = cv2.threshold(gray,127,255,0)

contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

cX, cY = None, None

for c in contours:
    M = cv2.moments(c)
    if M["m00"] != 0:
        cX = int(M["m10"] / M["m00"])
        cY = int(M["m01"] / M["m00"])


if len(corners) > 0 and (cX and cY):
  corners_x = int((corners[0][0][0][0] + corners[0][0][1][0] + corners[0][0][2][0] + corners[0][0][3][0])/4)
  corners_y = int((corners[0][0][0][1] + corners[0][0][1][1] + corners[0][0][2][1] + corners[0][0][3][1])/4)
  cv2.line(frame, (cX, cY), (corners_x, corners_y), (0, 0, 255), 3)

path_vector = [cX - corners_x, cY - corners_y]

unit_vector_1 = x_vector / np.linalg.norm(x_vector)
unit_vector_2 = path_vector / np.linalg.norm(path_vector)
dot_product = np.dot(unit_vector_1, unit_vector_2)
angle_rad = np.arccos(dot_product)
angle_deg = angle_rad * (180 / math.pi)

print(angle_deg)

# Display the resulting frame
cv2.imshow('frame',frame)
cv2.waitKey(0)
cv2.destroyAllWindows()
