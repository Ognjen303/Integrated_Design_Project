import apriltag
import cv2
import numpy as np
import paho.mqtt.client as mqtt 
from util import calculate_angle
from util import intermediates
from util import detect_dropoff
from util import forward_path
from util import detect_starting_location

'''
ssh -L 8081:idpcam2.eng.cam.ac.uk:8080 aps85@gate.eng.cam.ac.uk
'''

# Number of frames between sending information to the Arduino
comm_interval = 100
#off-set for drop-off accuracy
offset = 0.03

mqttBroker = "broker.hivemq.com" #mqttBroker = "test.mosquitto.org" #mqttBroker = "public.mqtthq.com" 
client = mqtt.Client("Python")
client.connect(mqttBroker) 

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

# Load camera properties 
mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

# Find the drop off points
drop_off = detect_dropoff(stream)
#Find starting location
starting_location = detect_starting_location(stream)

frame_counter = 0

while len(drop_off) > 0:
    # Find the forward path
    destinations = forward_path(stream)
    while len(destinations) > 0:
        if frame_counter % 10 == 0:
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

                # Draw the center (x, y)-coordinates of the AprilTag
                (cX, cY) = (int(r.center[0]), int(r.center[1]))
                
                x_comp = ((ptB[0] + ptA[0])/ 2) - cX
                y_comp = cY - ((ptB[1] + ptA[1])/2)
                robot_vector = np.array([x_comp, y_comp, 0])
                robot_vector = robot_vector/np.linalg.norm(robot_vector)

                translation_x = destinations[0][0] - cX
                translation_y = cY - destinations[0][1]
                translation_vector = np.array([translation_x, translation_y, 0])
                translation_pixel_distance = np.linalg.norm(translation_vector) # From center
                translation_vector = translation_vector/np.linalg.norm(translation_vector) 

                # Basic pixel to distance calibration
                front_edge = np.array([(ptB[0] - ptA[0]), (ptB[1] - ptA[1])])
                front_edge_pixel_distance = np.linalg.norm(front_edge) 
                front_edge_length = 0.093 # Fix up!!!
                translation_distance = ((translation_pixel_distance / front_edge_pixel_distance) * front_edge_length) 
                # Angle is positive for turn left and negative for turn right 
                angle = calculate_angle(robot_vector, translation_vector)
                
                # Send angle + distance distance every 100 frames
                if frame_counter % comm_interval == 0:
                    if len(destinations) == 1:
                        information = str(int(angle)) + ";" + str(round((-translation_distance), 2)) 
                        client.publish("IDP211", information)
                    else:
                        information = str(int(angle)) + ";" + str(round(translation_distance, 2)) 
                        client.publish("IDP211", information)
                # Regardless of frame_count, send the first distance below the threshold for arduino to break out
                if translation_distance < 0.10:
                    if len(destinations) == 1:
                        information = str(int(angle)) + ";" + str(round((-translation_distance), 2)) 
                        client.publish("IDP211", information)
                    else:
                        information = str(int(angle)) + ";" + str(round(translation_distance, 2)) 
                        client.publish("IDP211", information)
                    del destinations[0] # Start listening here
            
        frame_counter += 1

    block_colour = None

    def on_message(client, userdata, msg):
        global block_colour
        block_colour = msg.payload.decode()
        print(msg.payload.decode())

    client.subscribe('IDP211')
    client.on_message = on_message

    while block_colour == None:
        client.loop()

    # Set a return path based on colour
    if block_colour == 'blue':
        destinations = [destinations[-2], destinations[-3], destinations[-4], intermediates(starting_location, destinations[0]), drop_off[0]]
        del drop_off[0]
    else: # must be red
        destinations = [destinations[-2], destinations[-3], destinations[-4], intermediates(starting_location, destinations[0]), drop_off[-1]]
        del drop_off[-1]

    while len(destinations) > 0:
        if frame_counter % 10 == 0:
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

                # Draw the center (x, y)-coordinates of the AprilTag
                (cX, cY) = (int(r.center[0]), int(r.center[1]))
                
                x_comp = ((ptB[0] + ptA[0])/ 2) - cX
                y_comp = cY - ((ptB[1] + ptA[1])/2)
                robot_vector = np.array([x_comp, y_comp, 0])
                robot_vector = robot_vector/np.linalg.norm(robot_vector)

                translation_x = destinations[0][0] - cX
                translation_y = cY - destinations[0][1]
                translation_vector = np.array([translation_x, translation_y, 0])
                translation_pixel_distance = np.linalg.norm(translation_vector) # From center
                translation_vector = translation_vector/np.linalg.norm(translation_vector) 

                # Basic pixel to distance calibration
                front_edge = np.array([(ptB[0] - ptA[0]), (ptB[1] - ptA[1])])
                front_edge_pixel_distance = np.linalg.norm(front_edge) 
                front_edge_length = 0.093 # Fix up!!!
                translation_distance = ((translation_pixel_distance / front_edge_pixel_distance) * front_edge_length) - offset #to line up block with drop-off
                # Angle is positive for turn left and negative for turn right 
                angle = calculate_angle(robot_vector, translation_vector)
                
                # Send angle + distance distance every 100 frames
                if frame_counter % comm_interval == 0:
                    if len(destinations) == 1:
                        information = str(int(angle)) + ";" + str(round((-translation_distance), 2)) 
                        client.publish("IDP211", information)
                    else:
                        information = str(int(angle)) + ";" + str(round(translation_distance, 2)) 
                        client.publish("IDP211", information)
                # Regardless of frame_count, send the first distance below the threshold for arduino to break out
                    if translation_distance < 0.10 and len(destinations) > 1:
                        information = str(int(angle)) + ";" + str(round(translation_distance, 2)) 
                        client.publish("IDP211", information)
                        del destinations[0]
                    
                    if translation_distance < 0.02 and len(destinations) == 1:
                        information = str(int(angle)) + ";" + str(round(-translation_distance, 2)) 
                        client.publish("IDP211", information)
                        del destinations[0]

            '''
            cv2.line(f, ptA, ptB, (0, 255, 0), 2) #front-edge
            cv2.line(f, (cX, cY), (int((ptB[0] + ptA[0])/ 2), int((ptB[1] + ptA[1])/ 2)), (0, 255, 0), 2) #line - centre to mid front
            cv2.line(f, destinations[0], (int((ptB[0] + ptA[0])/ 2), int((ptB[1] + ptA[1])/ 2)), (0, 255, 0), 2) #line - mid front to desination
            cv2.circle(f, (int((ptB[0] + ptA[0])/ 2), int((ptB[1] + ptA[1])/ 2)), 5, (0, 0, 255), -1) #circle - mid front
            cv2.circle(f, (int(destinations[0][0]), int(destinations[0][1])), 5, (0, 0, 255), -1) #circle - destination
            cv2.circle(f, (int(drop_off[0]), int(drop_off[1])), 5, (0, 0, 255), -1)

            cv2.imshow('Hi', f)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
            '''
            
        frame_counter += 1
    
    print('Block done!')

print('Done')