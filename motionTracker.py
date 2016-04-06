import cv2
import imutils
import numpy as np
from collections import deque
import glob, serial, time, signal, sys, json, click
from datetime import datetime
import random
import requests
from socketIO_client import SocketIO, LoggingNamespace
from config import *
from termcolor import colored


class MotionTracker:

    server_settings = None

    # Helper functions to colour text output to command line
    def concat_args(self, *arg):
        s = ""
        for a in arg:
            s = s + " " + str(a)
        return s

    # Display a message
    def msg(self, *arg):
        print(colored(self.concat_args(*arg), 'yellow'))

    def errmsg(self, *arg):
        print(colored(self.concat_args(*arg), 'red'))

    def infomsg(self, *arg):
        print(colored(self.concat_args(*arg), 'blue'))

    # Test for web server
    def test_for_webserver(self):
        url = self.server_settings['protocol'] + "://" + self.server_settings['host']+":"+str(self.server_settings['port'])
        self.infomsg('Testing connection to: ', url)
        try:
            r = requests.get(url)
        except requests.exceptions.ConnectionError:
            return False

        if r.status_code == 200:
            return True
        else:
            return False

    # Configure web socket
    def config_websocket(self):
        # Test web server exists
        server_exists = self.test_for_webserver()
        if not server_exists:
            self.errmsg("Failed to connect to web server, is it running?")
            sys.exit(0)
        # Connect to web socket
        self.infomsg('Connecting to socket')
        self.socketCon = SocketIO('localhost', self.server_settings['port'])
        while not self.socketCon.connected:
            self.infomsg("Waiting for connection")
            time.sleep(CONNECTION_WAIT)

    def video_track(self, video_src, video_out):
        buffer = 64
        colourLower = (29, 86, 6)
        colourUpper = (84, 255, 255)
        pts = deque(maxlen=buffer)

        camera = cv2.VideoCapture(video_src)

        while True:
            (grabbed, frame) = camera.read()
            # resize the frame, blur it, and convert it to the HSV
            frame = imutils.resize(frame, width=600)
            # blurred = cv2.GaussianBlur(frame, (11, 11), 0)
            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            # construct a mask for the color "green", then perform a series of dilations and erosions to remove any
            # small blobs left in the mask
            mask = cv2.inRange(hsv, colourLower, colourUpper)
            mask = cv2.erode(mask, None, iterations=2)
            mask = cv2.dilate(mask, None, iterations=2)

            if video_out:
                cv2.imshow("Mask", mask)

            # find contours in the mask and initialize the current (x, y) center of the ball
            cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,
            cv2.CHAIN_APPROX_SIMPLE)[-2]
            center = None
            # only proceed if at least one contour was found
            if len(cnts) > 0:
                # find the largest contour in the mask, then use it to compute the minimum enclosing circle and centroid
                c = max(cnts, key=cv2.contourArea)
                ((x, y), radius) = cv2.minEnclosingCircle(c)
                M = cv2.moments(c)
                center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))

                print center

                # only proceed if the radius meets a minimum size
                if video_out and radius > 10:
                    # draw the circle and centroid on the frame, then update the list of tracked points
                    cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 255), 2)
                    cv2.circle(frame, center, 5, (0, 0, 255), -1)

            # update the points queue
            pts.appendleft(center)

            if video_out:
                # loop over the set of tracked points
                for i in xrange(1, len(pts)):
                    # if either of the tracked points are None, ignore them
                    if pts[i - 1] is None or pts[i] is None:
                        continue
                    # otherwise, compute the thickness of the line and draw the connecting lines
                    thickness = int(np.sqrt(buffer / float(i + 1)) * 2.5)
                    cv2.line(frame, pts[i - 1], pts[i], (0, 0, 255), thickness)

                # show the frame to our screen
                cv2.imshow("Frame", frame)
                key = cv2.waitKey(1) & 0xFF

                # if the 'q' key is pressed, stop the loop
                if key == ord("q"):
                    break

        # cleanup the camera and close any open windows
        camera.release()
        cv2.destroyAllWindows()



    # Main function called when script executed
    def __init__(self, servermode, video_src, output):
        self.msg(">>>> Motion Tracking Service <<<<", video_src)
        # Load settings
        self.server_settings = WEB_SERVER[servermode]
        # Config websocket
        # self.config_websocket()
        # Start Video Tracking
        self.video_track(video_src, output)




# -----------------------------------------------------------------------------

@click.command()
@click.option('--src', default=0, help='Video source id')
@click.option('--prod', is_flag=True, default=False, help='Switch to production mode')
@click.option('--output', is_flag=True, default=False, help='Show Video Output')
def main(src, prod, output):
    if not prod:
        servermode = 'local'
    else:
        servermode = 'prod'
    sm = MotionTracker(servermode, src, output)

if __name__ == '__main__':
    main()


# -----------------------------------------------------------------------------

