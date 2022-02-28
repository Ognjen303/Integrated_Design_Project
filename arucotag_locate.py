import cv2 
import numpy as np 

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

# shape of the image: (760, 1016, 3)
# could try AruCo board for more robust detection 
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

while(True):
    ret, frame = stream.read()  

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Detect ArUco markers in the video frame
    (corners, ids, rejected) = cv2.aruco.detectMarkers(
      gray, apriltag_dict, parameters=aruco_parameters)
    
    if len(corners) > 0:
      print(corners[0].shape)

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
    # Threshold the HSV image to get only red colors
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
        else:
            cX, cY = 0, 0

    if len(corners) > 0:
      print((corners[0][0][0][0], corners[0][0][0][1]))
      cv2.line(frame, ((cX, cY)), (int(corners[0][0][0][0]), int(corners[0][0][0][1])), (0, 0, 255), 3)
      cv2.line(frame, ((cX, cY)), (int(corners[0][0][1][0]), int(corners[0][0][1][1])), (0, 0, 255), 3)
      cv2.line(frame, ((cX, cY)), (int(corners[0][0][2][0]), int(corners[0][0][2][1])), (0, 0, 255), 3)
      cv2.line(frame, ((cX, cY)), (int(corners[0][0][3][0]), int(corners[0][0][3][1])), (0, 0, 255), 3)

    # Display the resulting frame
    cv2.imshow('frame',frame)
          
    # If "q" is pressed on the keyboard, 
    # exit this loop
    if cv2.waitKey(1) & 0xFF == ord('q'):
      break
  
stream.release()
cv2.destroyAllWindows()