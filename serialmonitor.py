import serial
import time
from datetime import datetime
from socketIO_client import SocketIO, LoggingNamespace
import signal
import sys

socketCon = None
serialCon = None

def read_serial():
    text   = serialCon.readline().decode()
    chunks = text.split(',')
    if len(chunks) == 3:
        tosend = { 'x': chunks[0].strip(), 'y':chunks[1].strip(), 'z':chunks[2].strip() }
        print(tosend)
        socketCon.emit('incomming', tosend)
        # socketCon.wait(seconds=1)
        time.sleep(0.25)
    else:
        print('Skipping: ',text)


def signal_handler(signal, frame):
        print('You pressed Ctrl+C!')
        if socketCon is not None:
            socketCon.disconnect()
        if serialCon is not None:
            serialCon.close()
        sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)


if __name__ == '__main__':

    print('Connecting to socket')
    socketCon = SocketIO('localhost', 5000)
    while not socketCon.connected:
        print("waiting for connection")
        time.sleep(2)

    print('Connecting to serial port')
    serialCon = serial.Serial("/dev/cu.usbmodem14241", 9600, timeout=10)
    while not serialCon.isOpen():
        print("waiting for connection")
        time.sleep(2)

    print('Starting...')
    while True:
        if socketCon is not None and serialCon is not None:
            read_serial()
        else:
            print("Socket or Serial not open yet")
            time.sleep(5)