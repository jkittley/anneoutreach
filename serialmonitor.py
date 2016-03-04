import glob, serial, time, signal, sys, json, click
from datetime import datetime
import requests
from socketIO_client import SocketIO, LoggingNamespace
from config import *
from termcolor import colored

class SerialMonitor:

    socketCon = None
    serialCon = None

    def concat_args(self, *arg):
        s = []
        for a in arg:
            s.append(str(a.__str__()))
        return s

    def errmsg(self, *arg):
        print(colored(self.concat_args(arg), 'red'))

    def infomsg(self, *arg):
        print(colored(self.concat_args(arg), 'blue'))

    def optionmsg(self, *arg):
        print(colored(self.concat_args(arg), 'blue'))

    def read_loop(self):
        # Read Serial loop
        while True:
            if self.socketCon is not None and self.serialCon is not None:
                self.read_serial()
                time.sleep(SAMPLE_INTERVAL / 1000)
            else:
                self.infomsg("Socket or Serial not open yet")
                time.sleep(5)

    # Read in a line, check that is decodes as JSON and then forward to web socket
    def read_serial(self):
        serialLine = self.serialCon.readline().decode()
        try:
            jsonStr = json.loads(serialLine)
        except ValueError:
            self.errmsg("invalid Json string:", serialLine)
            return
        self.socketCon.emit('new_serial_data', jsonStr)

    # Detect when script terminated and close socket
    def signal_handler(self, given_signal):
        try:
            if self.serialCon is not None:
                self.serialCon.close()
        except serial.serialutil.SerialException:
            pass

        self.infomsg('Closing connections...')
        if self.socketCon is not None:
            self.socketCon.disconnect()

        self.infomsg('Exited')
        sys.exit(0)

    # List all available serial ports
    def list_serial_ports(self):
        # Find ports
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')
        # Test all discovered ports and only return those where a connection could be established
        result = []
        for port in ports:
            if self.test_port(port):
                result.append(port)
        return result

    # Test if a connection to a port can be established
    def test_port(self, port):
        try:
            s = serial.Serial(port)
            s.close()
            return True
        except (OSError, serial.SerialException):
            return False

    # Manual input for port
    def manual_port_picker(self, ports):
        # Print option
        port_number = 1
        for port in ports:
            self.optionmsg(port_number, port)
            port_number+=1
        self.optionmsg('E', "to exit")
        # Wait for answer
        while True:
            try:
                answer = input('Please select a port: ')
                if answer.lower() == 'e':
                    sys.exit(0)
                return ports[int(answer)-1]
            except (IndexError, UnicodeDecodeError, ValueError):
                self.errmsg('Invalid choice please try again.')

    # Automatic port picker
    def auto_port_picket(self, ports):
        port_number = 1
        for port in ports:
            port_number += 1
            if 'usbmodem' in port:
                self.infomsg("Auto selected: ", port)
                return port

    # Test for web server
    def test_for_webserver(self):
        url = WEB_SERVER_ADDR+":"+str(WEB_SERVER_PORT)
        self.infomsg('Testing connection to: ', url)
        try:
            r = requests.get(url)
        except requests.exceptions.ConnectionError:
            return False

        if r.status_code == 200:
            return True
        else:
            return False


    # Main function called when script executed
    def __init__(self, autooff):

        # List ports
        ports = self.list_serial_ports()
        port = None

        self.infomsg(">>>> Serial Port Monitor <<<<")
        # Auto pick a port
        if not autooff:
            port = self.auto_port_picket(ports)

        # If no port auto picked or auto pick off then manual pick
        if port is None:
            port = self.manual_port_picker(ports)

        # Test web server exists
        server_exists = self.test_for_webserver()
        if not server_exists:
            self.errmsg("Failed to connect to web server, is it running?")
            sys.exit(0)

        # Connect to web socket
        self.infomsg('Connecting to socket')
        self.socketCon = SocketIO('localhost', WEB_SERVER_PORT)
        while not self.socketCon.connected:
            self.infomsg("Waiting for connection")
            time.sleep(CONNECTION_WAIT)

        # Serial
        self.infomsg('Connecting to serial port')
        self.serialCon = serial.Serial(port, SERIAL_RATE, timeout=CONNECTION_TIMEOUT)
        while not self.serialCon.isOpen():
            self.infomsg("waiting for connection")
            time.sleep(CONNECTION_WAIT)

        # Run serial loop
        self.infomsg('Starting...')
        self.read_loop()
        return

# -----------------------------------------------------------------------------


@click.command()
@click.option('--autooff', is_flag=True, default=False, help='Turn off auto port selection')
def main(autooff):
    sm = SerialMonitor(autooff)
    # signal.signal(signal.SIGINT, sm.signal_handler)

if __name__ == '__main__':
    main()


# -----------------------------------------------------------------------------
