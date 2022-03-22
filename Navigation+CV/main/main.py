import apriltag
import cv2
import numpy as np
import paho.mqtt.client as mqtt
from util import calculate_angle
from util import detect_block
from util import intermediates

blocks_to_attempt = 4

# Load camera properties 
mtx = np.fromfile('distortion/cameramatrix.dat', dtype=float)
dist = np.fromfile('distortion/distortionmatrix.dat')
newmtx = np.fromfile('distortion/newcameramatrix.dat')
mtx = np.reshape(mtx, (3, 3))
dist = np.reshape(dist, (1,5))
newmtx = np.reshape(newmtx, (3,3))

# Number of frames between sending information to the Arduino
comm_interval = 100

mqttBroker = "broker.hivemq.com"
# Alternative message brokers:
# mqttBroker = "public.mqtthq.com" 
# mqttBroker = "test.mosquitto.org"
# mqttBroker =  "public.mqtthq.com" 
client = mqtt.Client("Python")
client.connect(mqttBroker) 

stream = cv2.VideoCapture('http://localhost:8081/stream/video.mjpeg')

# Features of the arena 
starting_location = (532, 175)
intersection_1 = (421, 275)
ramp = (288, 396)
intersection_2 = (156, 522)
original_forward_path = [intersection_1, ramp, intersection_2, detect_block(stream)] # Archive original path
bottom_ramp = intermediates(intersection_2, ramp, 4)[1]

drop_off_locations = [(380, 151), (315, 211), (475, 386), (536, 325)] # Corrected for parallex error 
# Blue (top), Blue (bottom), Red (bottom), Red (top) - 'bottom' is drop off closer to ramp, 'top' is white boxes closer to starting point
bottom_drop_off = intermediates(original_forward_path[0], starting_location, 7)[0] # Checkpoint before dropping off in the 'bottom' boxes
top_drop_off = (bottom_drop_off[0] + 66, bottom_drop_off[1] - 60) # Checkpoint before dropping off in the 'top' boxes

frame_counter = 0
# Blue and Red blocks picked up
blue = 0
red = 0

while (red+blue) < blocks_to_attempt:
    if red+blue == 0:
        # Initialise path for the first block
        destinations = [intersection_1, ramp, intersection_2, detect_block(stream)]
    else:
        # Initialise path for the further blocks
        destinations = [ramp, intersection_2, detect_block(stream)]
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
                front_edge_length = 0.093 
                translation_distance = ((translation_pixel_distance / front_edge_pixel_distance) * front_edge_length) 
                # Angle is positive for turn left and negative for turn right 
                angle = calculate_angle(robot_vector, translation_vector)
                
                # Send angle + distance distance every 100 frames - negative values when the next checkpoint is the block
                if frame_counter % comm_interval == 0:
                    if len(destinations) == 1:
                        information = str(int(angle)) + ";" + str(round((-translation_distance), 2)) 
                        client.publish("IDP211", information)
                    else:
                        information = str(int(angle)) + ";" + str(round(translation_distance, 2)) 
                        client.publish("IDP211", information)
                
                # Regardless of frame_count, send the first distance below the threshold for arduino to break out
                if translation_distance <= 0.10:
                    if len(destinations) == 1:
                        information = str(int(angle)) + ";" + str(round((-translation_distance), 2)) 
                        client.publish("IDP211", information)
                        if abs(angle) < 3:
                            del destinations[0] # Start listening here
                    else: 
                        information = str(int(angle)) + ";" + str(round((translation_distance), 2)) 
                        client.publish("IDP211", information)
                        del destinations[0] 

        frame_counter += 1

    block_colour = None
    picked_up = None

    # Listen to the Arduino for colour + whether the block has been successfully picked up

    def on_message(client, userdata, msg):
        global block_colour
        if (msg.payload.decode() == 'red') or (msg.payload.decode() == 'blue'):
            block_colour = msg.payload.decode()

    client.subscribe('IDP211')
    client.on_message = on_message

    while block_colour == None: 
        client.loop()
    
    def on_message(client, userdata, msg):
        global picked_up
        picked_up = msg.payload.decode()

    client.subscribe('IDP211')
    client.on_message = on_message
    
    # Rescan for the block and provide reapproach navigation if the signal 'reapproach' is sent
    while True:
        client.loop()
        if picked_up == 'reapproach': 
            picked_up = None
            r, f = stream.read()
            f = cv2.undistort(f, mtx, dist, None, newmtx)
            f = f[:, 200:800]
            destinations = [detect_block(stream)] 
            frame_counter = 0
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
                            information = str(int(angle)) + ";" + str(round((-translation_distance), 2)) 
                            client.publish("IDP211", information)
                        
                        # Regardless of frame_count, send the first distance below the threshold for arduino to break out
                        if translation_distance <= 0.10:
                            information = str(int(angle)) + ";" + str(round((-translation_distance), 2)) 
                            client.publish("IDP211", information)
                            if abs(angle) < 3:
                                del destinations[0] 

        if picked_up == 'pickedup':
            break

    # Set a return path based on colour + previously placed blocks
    if block_colour == 'blue':
        if blue == 1:
            destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], bottom_drop_off, intermediates(bottom_drop_off, drop_off_locations[0])[0], drop_off_locations[0]]
            del drop_off_locations[0] 
        elif blue == 0:
            destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], top_drop_off, intermediates(top_drop_off, drop_off_locations[0])[0], drop_off_locations[0]]
            del drop_off_locations[0] 
        # Mistaken colour detection - fail safe
        elif blue == 2:
            if red == 1:
                destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], bottom_drop_off, intermediates(bottom_drop_off, drop_off_locations[-1])[0], drop_off_locations[-1]]
                del drop_off_locations[-1] 
            elif red == 0:
                destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], top_drop_off, intermediates(top_drop_off, drop_off_locations[-1])[0], drop_off_locations[-1]]
                del drop_off_locations[-1] 
        elif blue == 3:
            destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], bottom_drop_off, intermediates(bottom_drop_off, drop_off_locations[-1])[0], drop_off_locations[-1]]
            del drop_off_locations[0] 
        blue += 1
    
    else: # must be red
        if red == 1:
            destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], bottom_drop_off, intermediates(bottom_drop_off, drop_off_locations[-1])[0], drop_off_locations[-1]]
            del drop_off_locations[-1]
        elif red == 0:
            destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], top_drop_off, intermediates(top_drop_off, drop_off_locations[-1])[0], drop_off_locations[-1]]
            del drop_off_locations[-1]
        # Mistaken colour detection - fail safe
        elif red == 2:
            if blue == 1:
                destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], bottom_drop_off, intermediates(bottom_drop_off, drop_off_locations[0])[0], drop_off_locations[0]]
                del drop_off_locations[0] 
            elif blue == 0:
                destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], top_drop_off, intermediates(top_drop_off, drop_off_locations[0])[0], drop_off_locations[0]]
                del drop_off_locations[0] 
        elif red == 3:
            destinations = [original_forward_path[-2], bottom_ramp, original_forward_path[-3], bottom_drop_off, intermediates(bottom_drop_off, drop_off_locations[0])[0], drop_off_locations[0]]
            del drop_off_locations[0] 
        red += 1
    
    # Navigate through the drop off path
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
                center_front_edge = (int((ptB[0] + ptA[0])/2), int((ptB[1] + ptA[1])/2))

                # Draw the center (x, y)-coordinates of the AprilTag
                (cX, cY) = (int(r.center[0]), int(r.center[1]))
                
                x_comp = ((ptB[0] + ptA[0])/ 2) - cX
                y_comp = cY - ((ptB[1] + ptA[1])/2)
                robot_vector = np.array([x_comp, y_comp, 0])
                robot_vector = robot_vector/np.linalg.norm(robot_vector)

                translation_x = destinations[0][0] - center_front_edge[0] # based on the block position
                translation_y = center_front_edge[1] - destinations[0][1] # based on the block position
                translation_vector = np.array([translation_x, translation_y, 0])
                translation_pixel_distance = np.linalg.norm(translation_vector) # From center
                translation_vector = translation_vector/np.linalg.norm(translation_vector) 

                # Basic pixel to distance calibration
                front_edge = np.array([(ptB[0] - ptA[0]), (ptB[1] - ptA[1])])
                front_edge_pixel_distance = np.linalg.norm(front_edge) 
                front_edge_length = 0.127 #TEST
                translation_distance = ((translation_pixel_distance / front_edge_pixel_distance) * front_edge_length) 
                # Angle is positive for turn left and negative for turn right 
                angle = calculate_angle(robot_vector, translation_vector)
                
                # Send angle + distance distance every 100 frames - negative distances if the next checkpoint is drop off point
                if frame_counter % comm_interval == 0:
                    if len(destinations) == 1:
                        information = str(int(angle)) + ";" + str(round((-translation_distance), 2)) 
                        client.publish("IDP211", information)
                    else:
                        information = str(int(angle)) + ";" + str(round(translation_distance, 2)) 
                        client.publish("IDP211", information)
                # Regardless of frame_count, send the first distance below the threshold for arduino to break out
                if translation_distance <= 0.10 and len(destinations) > 1: ###
                    information = str(int(angle)) + ";" + str(round(translation_distance, 2)) 
                    client.publish("IDP211", information)
                    del destinations[0]
                
                if round(translation_distance, 2) <= 0.02 and len(destinations) == 1:
                    information = str(int(angle)) + ";" + str(round(-translation_distance, 2)) 
                    client.publish("IDP211", information)
                    del destinations[0]

        frame_counter += 1

    print('Block done!')

# Return to home code
destinations = [starting_location]

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
            front_edge_length = 0.093 
            translation_distance = ((translation_pixel_distance / front_edge_pixel_distance) * front_edge_length) 
            # Angle is positive for turn left and negative for turn right 
            angle = calculate_angle(robot_vector, translation_vector)
            
            # Send angle + distance distance every 100 frames                       
            if frame_counter % comm_interval == 0:
                information = str(int(angle)) + ";" + str(round(translation_distance, 2)) 
                client.publish("IDP211", information)
            
            # Regardless of frame_count, send the first distance below the threshold for arduino to break out
            if translation_distance <= 0.05:
                information = str(int(angle)) + ";" + str(round((translation_distance), 2))
                client.publish("IDP211", information)
                del destinations[0] 

    frame_counter += 1

print('Competition done!')

while True:
    information = 'stop'
    client.publish("IDP211", information)
