import cv2 as cv
import argparse as ap

from os import listdir
from os.path import isfile, join

faceCascade = cv.CascadeClassifier('haarcascade_frontalface_default.xml')
eyeCascade = cv.CascadeClassifier('parojosG.xml')

parser = ap.ArgumentParser()
parser.add_argument('--input', metavar='input', required=True)
parser.add_argument('--output', metavar='output', required=True)
parser.add_argument('--start', metavar='start', type=int, required=True)

args = vars(parser.parse_args())

files = [f for f in listdir(args['input']) if isfile(join(args['input'], f))]

eyeCounter = 0
percent = 0
per_percent = len(files) / 100
nctr = 0
for f in files:

	nctr = nctr + 1
	if nctr % per_percent is 0:
		percent = percent + 1
		print("Percent complete: " + str(percent))

	img_color = cv.imread(join(args['input'], f))
	img = cv.cvtColor(img_color, cv.COLOR_BGR2GRAY)
	if img is None:
		print("Failed in reading {}!" % join(args['input'], f))
		exit()
	faces = faceCascade.detectMultiScale(img, 1.3, 5)
	for (x, y, w, h) in faces:
		h = 2 * h / 3
		cv.rectangle(img_color, (x, y), (x + w, y + h), (255, 0, 0), 2)
		roi = img[y:y + h, x:x + w]
		roi_c = img_color[y:y + h, x:x + w]

		if roi is None:
			continue

		eyes = eyeCascade.detectMultiScale(roi)

		for (ex, ey, ew, eh) in eyes:
			if ew < 24 or eh < 24:
				continue

			cv.rectangle(img_color, (ex, ey), (ex + ew, ey + eh), (0, 255, 0), 2)
			roi_eye = roi[ey:ey + eh, ex:ex + ew]
			roi_eye_c = roi_c[ey:ey + eh, ex:ex + ew]

			cv.imwrite(args['output'] + str(eyeCounter + args['start']) + '.jpg', roi_eye_c)

			eyeCounter = eyeCounter + 1

		

		cv.imshow('Face', img_color)
		cv.waitKey(1)

print("Saved " + str(eyeCounter) + " faces to " + args['input'])
