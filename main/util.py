import numpy as np
import math
from numpy import cross, eye, dot
from scipy.linalg import expm, norm
import cv2 as cv
import apriltag

mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

def calculate_angle (x, y):
    """x: robot vector
       y: path vector 
       returns angle (negative for turn right, positive for turn left"""
    cross_product = np.cross(x, y) 
    dot_product = np.dot(x, y)
    radians = math.asin(cross_product[2])
    angle = radians * 180/math.pi

    if dot_product < 0 and angle < 0:
        angle = -angle
        angle = -180 + angle
    elif dot_product < 0 and angle > 0:
        angle = -angle
        angle = 180 + angle
    elif dot_product < 0 and angle == 0:
        angle += 180
    return angle 

def detect_block(f):
    """f: frame
       returns (x, y) of block"""
    hsv = cv.cvtColor(f, cv.COLOR_BGR2HSV)
    lower_red = np.array([159, 50, 70])
    upper_red = np.array([180, 255, 255])
    # Threshold the HSV image to get only red colors
    #x ROI -  0 to 150
    #y ROI - 520 to 690
    red_mask = cv.inRange(hsv, lower_red, upper_red)
    red_mask[:, 150:] = 0
    red_mask[:520, :] = 0
    red_mask[690:, :] = 0
    _,thresh = cv.threshold(red_mask,127,255,0)
    M = cv.moments(thresh)
    cX = int(M["m10"] / M["m00"])
    cY = int(M["m01"] / M["m00"])
    return (cX, cY)

def detect_block_colour(s, threshold=15):
    """stream: stream, 
       threshold: number of blue pixels for the colour to be blue
       returns 'Blue' or 'Red'
    """
    blue_pixels = []

    while len(blue_pixels) < 50:
        r, f = s.read()
        f = cv.undistort(f, mtx, dist, None, newmtx)
        f = f[:, 200:800]

        # Convert BGR to HSV
        hsv = cv.cvtColor(f, cv.COLOR_BGR2HSV)
        lower_red = np.array([159, 50, 70])
        upper_red = np.array([180, 255, 255])
        # Threshold the HSV image to get only red colors
        red_mask = cv.inRange(hsv, lower_red, upper_red)
        red_mask[:, 150:] = 0
        red_mask[:520, :] = 0
        red_mask[690:, :] = 0

        ret,thresh = cv.threshold(red_mask,127,255,0)

        # calculate moments of binary image
        M = cv.moments(thresh)
        if M:
            # calculate x,y coordinate of center
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            
        # put text and highlight the center
        cv.circle(red_mask, (cX, cY), 5, (255, 255, 255), -1)
        cv.putText(red_mask, "centroid", (cX - 25, cY - 25),cv.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
        hsv = hsv[cY - 7:cY + 7, cX - 7:cX + 7]
        f = f[cY - 7:cY + 7, cX - 7:cX + 7]
        lower_blue = np.array([110,50,50])
        upper_blue = np.array([130,255,255])
        # Threshold the HSV image to get only blue colors
        blue_mask = cv.inRange(hsv, lower_blue, upper_blue)
        # Bitwise-AND mask and original image
        #white_res = cv2.bitwise_and(f,f, mask= blue_mask)
        n_pixels = np.sum(blue_mask == 255)
        blue_pixels.append(n_pixels)
    
    average = sum(blue_pixels) / 50

    if average < threshold: #SUBJECT TO CHANGE
        print("RED")
        return 'RED'
    else:
        return 'BLUE'

def intermediates(p1, p2, nb_points=1):
    """p1: first coordinate
       p2: second coordinate
       returns nb_points equally spaced points interpolated with p1 and p2"""
    # If we have 8 intermediate points, we have 8+1=9 spaces
    # between p1 and p2
    if (p1 == None) or (p2 == None):
        return [[None],  [None]]

    x_spacing = (p2[0] - p1[0]) / (nb_points + 1)
    y_spacing = (p2[1] - p1[1]) / (nb_points + 1)

    return [(int(p1[0] + i * x_spacing), int(p1[1] +  i * y_spacing))
            for i in range(1, nb_points+1)]

def detect_intersections(f):
    """f: frame
       returns [(x1, y1), (x2, y2)] - the coordiantes of first and second 
       intersection - returns [(None), (None)] if not found"""

    check1 = cv.imread('/Users/adhi/Desktop/IDP/main/features/checkpoint1.png',0)
    check2 = cv.imread('/Users/adhi/Desktop/IDP/main/features/checkpoint2.png',0) 
    sift = cv.SIFT_create()
    # find the keypoints and descriptors with SIFT
    kp1, des1 = sift.detectAndCompute(check1,None)
    kp2, des2 = sift.detectAndCompute(check2,None)

    FLANN_INDEX_KDTREE = 1
    index_params = dict(algorithm = FLANN_INDEX_KDTREE, trees = 5)
    search_params = dict(checks = 50)
    flann = cv.FlannBasedMatcher(index_params, search_params)

    kp3, des3 = sift.detectAndCompute(f,None)
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
    
    if (dst_pt1 and dst_pt2) and (int(dst_pt2[0][0]) < 380 and int(dst_pt2[0][1]) > 300):
        return [(int(dst_pt1[0][0]), int(dst_pt1[0][1])), (int(dst_pt2[0][0]), int(dst_pt2[0][1]))]
    else:
        return [(None), (None)]
'''
def detect_dropoff(f):
    """f: frame
    returns (x, y) of bottom red drop off point --> extend to finding all the four drop_offs"""
    
    red_b = cv.imread('/Users/adhi/Desktop/IDP/main/features/red_b.png',0)
    #red_t = cv.imread('/Users/adhi/Desktop/IDP/main/features/red_t.png',0) 
    #make it more robust?
    sift = cv.SIFT_create()
    # find the keypoints and descriptors with SIFT
    kp1, des1 = sift.detectAndCompute(red_b,None)
    #kp2, des2 = sift.detectAndCompute(red_t,None)

    FLANN_INDEX_KDTREE = 1
    index_params = dict(algorithm = FLANN_INDEX_KDTREE, trees = 5)
    search_params = dict(checks = 50)
    flann = cv.FlannBasedMatcher(index_params, search_params)

    kp3, des3 = sift.detectAndCompute(f,None)
    matches_1 = flann.knnMatch(des1,des3,k=2)
    #matches_2 = flann.knnMatch(des2,des3,k=2)
    # store all the good matches as per Lowe's ratio test.
    good_1 = []
    #good_2 = []
    
    for m,n in matches_1:
        if m.distance < 0.7*n.distance:
            good_1.append(m)
    
    #for m,n in matches_2:
        #if m.distance < 0.7*n.distance:
            #good_2.append(m)

    dst_pt1 = [kp3[m.trainIdx].pt for m in good_1]
    #dst_pt2 = [kp3[m.trainIdx].pt for m in good_2]
    
    if (dst_pt1):
        return (int(dst_pt1[0][0]), int(dst_pt1[0][1]))
    else:
        return None
'''
def forward_path(s):
    """s:stream
    returns [(x, y), (x,y) ...] of the forward_path"""
    frame_counter = 0
    while frame_counter < 10:
        frame_counter += 1
        r, f = s.read() # Shape is (760, 1016, 3)
        f = cv.undistort(f, mtx, dist, None, newmtx)
        f = f[:, 200:800] # ROI - Shape is (760, 600, 3)
        intersections = detect_intersections(f)
        intermediate_checkpts = intermediates(intersections[0], intersections[1])
        block = detect_block(f)
        destinations = [intersections[0], intermediate_checkpts[0], intersections[1], block] 
        frame_counter += 1
        # Stop if all the checkpoints are found
        if None not in destinations:
            return destinations

    return [(421, 281), (281, 394), (149, 514), (80, 592)]

def detect_dropoff(s):
    """s:stream
    returns [(x, y), (x,y) ...] of the drop_off_locations - first are two blue, second two are red"""
    frame_counter = 0
    drop_off = None
    while frame_counter < 10:
        frame_counter += 1
        r, f = s.read()
        f = cv.undistort(f, mtx, dist, None, newmtx)
        i = f[50:390,500:830]
        f = f[:, 200:800]

        gray = cv.cvtColor(i, cv.COLOR_BGR2GRAY)
        blur = cv.medianBlur(gray, 5)
        sharpen_kernel = np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]])
        sharpen = cv.filter2D(blur, -1, sharpen_kernel)

        thresh = cv.threshold(sharpen,160,255, cv.THRESH_BINARY_INV)[1]
        kernel = cv.getStructuringElement(cv.MORPH_RECT, (3,3))
        close = cv.morphologyEx(thresh, cv.MORPH_CLOSE, kernel, iterations=2)
        close = cv.bitwise_not(close)
        cnts = cv.findContours(close, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
        cnts = cnts[0] if len(cnts) == 2 else cnts[1]

        min_area = 115*0.5 #blue is between 115 and 140, 0.5 is for safety (up to half the pixels)
        max_area = 180*2 #red between 130 and 180, 2 is for up to double the pixels

        counter = 0
        contour_centres = []
        for c in cnts:
            area = cv.contourArea(c)
            if area > min_area and area < max_area:
                counter += 1

                (x,y),radius = cv.minEnclosingCircle(c)
                center = (int(x) + 300 ,int(y) + 50)
                contour_centres.append(center)

        if counter == 4:
            drop_off = contour_centres
            drop_off.sort(key=lambda y: y[0])
            return drop_off
        
    return [(313, 223), (375, 166), (422, 275), (524, 332)]


def detect_starting_location(s):
    """s:stream
    returns (x, y) of the starting position"""
    frame_counter = 0
    position = None
    while frame_counter < 10:
        frame_counter += 1
        r, f = s.read()
        f = cv.undistort(f, mtx, dist, None, newmtx)
        f = f[:, 200:800]
        gray = cv.cvtColor(f, cv.COLOR_BGR2GRAY)
        options = apriltag.DetectorOptions(families="tag36h11")
        detector = apriltag.Detector()
        results = detector.detect(gray)

        for r in results:
            (ptA, ptB, _, _) = r.corners
            ptB = (int(ptB[0]), int(ptB[1]))
            ptA = (int(ptA[0]), int(ptA[1]))

            # Draw the center (x, y)-coordinates of the AprilTag
            position = (int(r.center[0]), int(r.center[1]))
        
        if position != None:
            return position
    
    return 'Start not found :(' # Hard-code a value!