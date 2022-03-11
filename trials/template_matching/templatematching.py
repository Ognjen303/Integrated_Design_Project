import cv2
import numpy as np

files = ["/Users/adhi/Desktop/IDP/template_matching/scene1.png", "/Users/adhi/Desktop/IDP/template_matching/scene2.png", 
"/Users/adhi/Desktop/IDP/template_matching/scene3.png","/Users/adhi/Desktop/IDP/template_matching/scene4.png",
"/Users/adhi/Desktop/IDP/template_matching/scene5.png", "/Users/adhi/Desktop/IDP/template_matching/scene6.png"]

#fails for scene4 --> needs rotation/scale invariance - could add several templates

template = cv2.imread("/Users/adhi/Desktop/IDP/template_matching/block.png")
h, w, _ = template.shape

for name in files:
    img = cv2.imread(name)
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    res = cv2.matchTemplate(img,template,cv2.TM_CCOEFF_NORMED)

    # Specify a threshold
    threshold = 0.7
    loc = np.where( res >= threshold)

    for pt in zip(*loc[::-1]):
        cv2.rectangle(img, pt, (pt[0] + w, pt[1] + h), (0,255,255), 2)
    
    # Show the final image with the matched area.
    cv2.imshow('Detected',img)
    cv2.waitKey(0)