import cv2
import numpy as np

#Testing alternative method for block detection - not translation/rotation invariant though.

def confidence(img, template):
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    template = cv2.cvtColor(template, cv2.COLOR_BGR2GRAY)
    res = cv2.matchTemplate(img, template, cv2.TM_CCOEFF_NORMED)
    conf = res.max()
    return np.where(res == conf), conf

files = ["scene1.png", "scene2.png", "scene3.png","scene4.png", "scene5.png", "scene6.png"]

template = cv2.imread("block.png")

h, w, _ = template.shape

for name in files:
    img = cv2.imread(name)
    ([y], [x]), conf = confidence(img, template)
    cv2.rectangle(img, (x, y), (x + w, y + h), (0, 0, 255), 2)
    text = f'Confidence: {round(float(conf), 2)}'
    cv2.putText(img, text, (x, y), 1, cv2.FONT_HERSHEY_PLAIN, (0, 0, 0), 2)
    cv2.imshow(name, img)
    
cv2.imshow('Template', template)
cv2.waitKey(0)
