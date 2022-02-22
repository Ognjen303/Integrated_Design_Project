import numpy as np
import time
import cv2

print_flag = True

cap = cv2.VideoCapture("http://localhost:8081/stream/video.mjpeg")


if not cap.isOpened():
    print("Cannot open camera")
    exit()
    
start_time = time.time()
counter = 0
    

while True:
    
    # Capture frame-by-framejj
    ret, frame = cap.read()
    
    # if frame is read correctly ret is True
    if not ret:
        print("Can't receive frame (stream end?). Exiting ...")
        break
    
    
    # Resolution of camera is 1012x760
    # Frame rate 20
    # Bandwidth 14Mbps
    
    counter+=1
    
    #------------------------------------------------
    
    # One pixes has colours in format BGR (blue, green, red)
    
    # Our operations on the frame come here
    # gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    
        
    # Region of interest
    # First argument is y, second is x component
    # because frame actually has row and column coordinates, we as humans think of it as
    # y and x
    roi = frame[:, 140:900]
    
    
    # Returns pixels with either value 0 or 1 if the pixel is in colour range
    Whiteline = cv2.inRange(roi, (110, 110, 110), (230, 230, 230))
    
    # Kernel takes one pixes, and asignes to it a value which
    # is averaged value of nearby pixels
    kernel = np.ones((3,3), np.uint8)
    
    # cv2.erode basically shrinks down noise
    # need more iteraitions for a high resolution
    Whiteline = cv2.erode(Whiteline, kernel, iterations = 1)
    
    # cv2.dilate does opposite of erode. It is used to fill in any holes in the lines 
    Whiteline = cv2.dilate(Whiteline, kernel, iterations = 2)
    
    if(print_flag == True):
        print(Whiteline[0, 0])
        print_flag = False
    
    
    
    
    

    
    
    # Display the resulting frame
    cv2.imshow('Livestream', Whiteline)
    
    # Close video by pressing "q" on keyboard
    if ((cv2.waitKey(1) & 0xFF) == ord('q')):
        break

finish_time = time.time()
fps = counter / (finish_time - start_time)
print("Frames per second = " + str(fps))

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()