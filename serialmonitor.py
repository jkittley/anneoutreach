import glob
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


def list_serial_ports():
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


if __name__ == '__main__':

    ports = list_serial_ports()
    port = None;

    # Auto pick a port
    i= 1;
    for p in ports:
        print(i, p)
        i+=1
        if 'usbmodem' in p:
            print("Auto selected: ", p)
            port = p

    # Manual selection
    while port is None:
        answer = input('Please select a port')
        if answer == 'E':
            sys.exit(0)
        try:
            port = ports[int(answer)]
        except:
            print('Invalid choice please try again.')

    # Socket
    print('Connecting to socket')
    socketCon = SocketIO('localhost', 5000)
    while not socketCon.connected:
        print("waiting for connection")
        time.sleep(2)

    # Serial
    print('Connecting to serial port')
    serialCon = serial.Serial(port, 9600, timeout=10)
    while not serialCon.isOpen():
        print("waiting for connection")
        time.sleep(2)

    # Read Serial loop
    print('Starting...')
    while True:
        if socketCon is not None and serialCon is not None:
            read_serial()
        else:
            print("Socket or Serial not open yet")
            time.sleep(5)