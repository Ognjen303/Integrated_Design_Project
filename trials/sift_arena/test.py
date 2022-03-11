import cv2 as cv
import numpy as np 
import matplotlib.pyplot as plt
from collections import deque

stream = cv.VideoCapture('http://localhost:8081/stream/video.mjpeg')

mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

MIN_MATCH_COUNT = 5
check1 = cv.imread('/Users/adhi/Desktop/IDP/trials/sift_arena/blue_b.png',0)
check2 = cv.imread('/Users/adhi/Desktop/IDP/trials/sift_arena/red_b.png',0) #make check2 less varied

check2_coordinates = deque([], maxlen = 5)

sift = cv.SIFT_create()
# find the keypoints and descriptors with SIFT
kp1, des1 = sift.detectAndCompute(check1,None)
kp2, des2 = sift.detectAndCompute(check2,None)

FLANN_INDEX_KDTREE = 1
index_params = dict(algorithm = FLANN_INDEX_KDTREE, trees = 5)
search_params = dict(checks = 50)
flann = cv.FlannBasedMatcher(index_params, search_params)

while True:
    r, img2 = stream.read()
    img2 = cv.undistort(img2, mtx, dist, None, newmtx)
    img2 = img2[:, 200:800]

    kp3, des3 = sift.detectAndCompute(img2,None)
    matches_1 = flann.knnMatch(des1,des3,k=2)
    matches_2 = flann.knnMatch(des2,des3,k=2)
    # store all the good matches as per Lowe's ratio test.
    good_1 = []
    good_2 = []
    
    for m,n in matches_1:
        if m.distance < 0.7*n.distance:
            good_1.append(m)
    
    for m,n in matches_2:
        if m.distance < 0.7*n.distance:
            good_2.append(m)

    dst_pt1 = [kp3[m.trainIdx].pt for m in good_1]
    dst_pt2 = [kp3[m.trainIdx].pt for m in good_2]
    
    if dst_pt1:
        cv.circle(img2, (int(dst_pt1[0][0]), int(dst_pt1[0][1])), 5, (0, 0, 255), -1)
        print((int(dst_pt1[0][0]), int(dst_pt1[0][1])))
    
    if dst_pt2 and (int(dst_pt2[0][0]) < 380 and int(dst_pt2[0][1]) > 300):
        #check2_coordinates.append((int(dst_pt2[0][0]), int(dst_pt2[0][1])))
        cv.circle(img2, ((int(dst_pt2[0][0]), int(dst_pt2[0][1]))), 5, (0, 0, 255), -1)
        print((int(dst_pt2[0][0]), int(dst_pt2[0][1])))
    
    #if len(check2_coordinates) == 5:
        #check_x = sum([pair[0] for pair in check2_coordinates]) / 5
        #check_y = sum([pair[1] for pair in check2_coordinates]) / 5
        #cv.circle(img2, ((int(check_x)), int(check_y)), 5, (0, 0, 255), -1)
    
    cv.imshow('IP Camera stream',img2)
    
    if cv.waitKey(1) & 0xFF == ord('q'):
        break
    
cv.destroyAllWindows()