import cv2 as cv
import argparse as ap

parser = ap.ArgumentParser()
parser.add_argument('--start', metavar='start', type=int, required=True)
parser.add_argument('--end', metavar='end', type=int, required=True)
parser.add_argument('--location', metavar='location', required=True)

args = vars(parser.parse_args())

cap = cv.VideoCapture(0)

for i in range(args['end'] - args['start']):
	ret, img = cap.read()
	cv.imwrite(args['location'] + str(i) + str(args['start']) + '.jpg', img)
	cv.imshow("Postview", img)
	cv.waitKey(1)
	
