import cv2 
import numpy as np 
import matplotlib.pyplot as plt
from collections import deque
import paho.mqtt.client as mqtt 

"""ssh -L 8081:idpcam2.eng.cam.ac.uk:8080 aps85@gate.eng.cam.ac.uk"""

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')
mqttBroker = "mqtt.eclipseprojects.io" 

client = mqtt.Client("Python")
client.username_pw_set('IDP211', 'CUED')
client.connect(mqttBroker) 

# shape of the image: (760, 1016, 3)
# AruCo board?
# https://docs.opencv.org/4.x/d9/d6a/group__aruco.html

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

block_coordinates = deque([], maxlen = 10)

while(True):
    ret, frame = stream.read()
    frame = cv2.undistort(frame, mtx, dist, None, newmtx)
    frame = frame[:700, 200:800]
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
      rotation_vectors, translation_vectors, _objPoints = cv2.aruco.estimatePoseSingleMarkers(corners, 1, mtx, dist)

      for rvec, tvec in zip(rotation_vectors, translation_vectors):
        frame = cv2.aruco.drawAxis(frame, mtx, dist, rvec, tvec, 3)
    
    lower_red = np.array([159, 50, 70])
    upper_red = np.array([180, 255, 255])

    red_mask = cv2.inRange(hsv, lower_red, upper_red)
    red_mask[:380, :] = 0
    red_mask[380:, 508:] = 0

    # Bitwise-AND mask and original image
    red_res = cv2.bitwise_and(frame, frame, mask= red_mask)

    gray = cv2.cvtColor(red_res, cv2.COLOR_BGR2GRAY)
    ret,thresh = cv2.threshold(gray,127,255,0)

    contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    for c in contours:
        M = cv2.moments(c)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            block_coordinates.append((cX, cY))

    # Draw the vector
    if len(corners) > 0 and len(block_coordinates) == 10: 
      corners_x = (corners[0][0][0][0] + corners[0][0][1][0] + corners[0][0][2][0] + corners[0][0][3][0]) / 4
      corners_y = (corners[0][0][0][1] + corners[0][0][1][1] + corners[0][0][2][1] + corners[0][0][3][1]) / 4

      block_x = sum([pair[0] for pair in block_coordinates]) / 10
      block_y = sum([pair[1] for pair in block_coordinates]) / 10

      cv2.line(frame, (int(block_x), int(block_y)), (int(corners_x), int(corners_y)), (0, 0, 255), 3)
    # client.publish("IDP211", #INSERT INFO HERE) 
    # Display the resulting frame
    cv2.imshow('frame',frame)
          
    # If "q" is pressed on the keyboard, 
    # exit this loop
    if cv2.waitKey(1) & 0xFF == ord('q'):
      break
  
stream.release()
cv2.destroyAllWindows()