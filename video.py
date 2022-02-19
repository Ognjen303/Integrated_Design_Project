import numpy as np
import cv2 as cv



cap = cv.VideoCapture("http://localhost:8081/stream/video.mjpeg")

if not cap.isOpened():
    print("Cannot open camera")
    exit()
    
while True:
    
    # Capture frame-by-frame
    ret, frame = cap.read()
    
    # if frame is read correctly ret is True
    if not ret:
        print("Can't receive frame (stream end?). Exiting ...")
        break
    
    # Our operations on the frame come here
    # gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    
    # Display the resulting frame
    cv.imshow('Livestream', frame)
    
    # Close video by pressing "q" on keyboard
    if ((cv.waitKey(1) & 0xFF) == ord('q')):
        break

# When everything done, release the capture
cap.release()
cv.destroyAllWindows()