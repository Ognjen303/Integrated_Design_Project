import numpy as np
import math
import cv2 as cv

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
    
    # Fix math domain error from bit precision errors
    if cross_product[2] > 1: 
        cross_product[2] = 1
    if cross_product[2] < -1:
        cross_product[2] = -1
    
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
'''
Deprecated detect block function

def detect_block(stream):
    """f: frame
       returns (x, y) of block"""
    frame_counter = 0 
    while frame_counter <= 20:
        frame_counter += 1
        r, f = stream.read() 
        f = cv.undistort(f, mtx, dist, None, newmtx)
        f = f[535:675, 200:335]
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
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            return (cX + 200, cY + 535)
    return (83, 590)
        #Find hard coded block starting point!!!
    '''

def detect_block(s):
    """s:stream
       returns (x, y) of the block"""
    frame_counter = 0
    while frame_counter <= 30: ###
        frame_counter += 1
        r, f = s.read() # Shape is (760, 1016, 3)
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

        cnts = cv.findContours(close, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
        cnts = cnts[0] if len(cnts) == 2 else cnts[1]

        min_area = 0 #expect approx 70
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
                print(center_list[0]) ###
                return center_list[0]
            else:
                center_list = []
        else:
            pass
        print((83, 590)) ### 
        return (83, 590) # Tuple

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
    """f: frame -
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
Deprecated drop off point detection function

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
# forward_path(s) and detect_dropoff(s) not used for final competition during run time
def forward_path(s):
    """s:stream
    returns [(x, y), (x,y) ...] of the forward_path"""
    frame_counter = 0
    block = detect_block(s)
    while frame_counter <= 10:
        frame_counter += 1
        r, f = s.read() # Shape is (760, 1016, 3)
        f = cv.undistort(f, mtx, dist, None, newmtx)
        f = f[:, 200:800] # ROI - Shape is (760, 600, 3)
        intersections = detect_intersections(f)
        intermediate_checkpts = intermediates(intersections[0], intersections[1])
        destinations = [intersections[0], intermediate_checkpts[0], intersections[1], block] # Detect all the checkpoints real time
        # Stop if all the checkpoints are found
        if None not in destinations:
            return destinations
    return [(421, 275), (288, 396), (156, 522), block] # Harcode checkpoints + detected block 

def detect_dropoff(s):
    """s:stream
    returns [(x, y), (x,y) ...] of the drop_off_locations - bottom blue, top blue, top red, bottom red"""
    frame_counter = 0
    threshold = 230
    drop_off = None
    while frame_counter <= 10:
        frame_counter += 1
        r, f = s.read()
        f = cv.undistort(f, mtx, dist, None, newmtx)
        f = f[:, 200:800]
        blues = f[140:250, 280:430]
        reds = f[300:420, 400:550]

        gray_b = cv.cvtColor(blues, cv.COLOR_BGR2GRAY)
        gray_r = cv.cvtColor(reds, cv.COLOR_BGR2GRAY)
        blur_b = cv.medianBlur(gray_b, 5)
        blur_r = cv.medianBlur(gray_r, 5)
        sharpen_kernel = np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]])
        sharpen_b = cv.filter2D(blur_b, -1, sharpen_kernel)
        sharpen_r = cv.filter2D(blur_r, -1, sharpen_kernel)
        
        thresh_b = cv.threshold(sharpen_b,threshold,255, cv.THRESH_BINARY_INV)[1]
        thresh_r = cv.threshold(sharpen_r,threshold,255, cv.THRESH_BINARY_INV)[1]
        thresh_b = cv.bitwise_not(thresh_b)
        thresh_r = cv.bitwise_not(thresh_r)

        cnts_r = cv.findContours(thresh_r, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
        cnts_r = cnts_r[0] if len(cnts_r) == 2 else cnts_r[1]

        cnts_b = cv.findContours(thresh_b, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
        cnts_b = cnts_b[0] if len(cnts_b) == 2 else cnts_b[1]

        min_area = 50 #blue is between 115 and 140, 0.5 is for safety (up to half the pixels)
        max_area = 220 #red between 130 and 180, 2 is for up to double the pixels

        centers = []
        for c in cnts_b:
            area = cv.contourArea(c)
            if area > min_area and area < max_area:
                (x,y),radius = cv.minEnclosingCircle(c)
                center = (int(x) + 280 ,int(y) + 140)
                centers.append(center)

        for c in cnts_r:
            area = cv.contourArea(c)
            if area > min_area and area < max_area:
                (x,y),radius = cv.minEnclosingCircle(c)
                center = (int(x) + 400 ,int(y) + 300)
                centers.append(center)

        if len(centers) == 4:
            drop_off = centers
            drop_off.sort(key=lambda y: y[0])
            return [drop_off[0], drop_off[1], drop_off[3], drop_off[2]] 
        
        return [(313, 222), (375, 166), (525, 331), (467, 389)]