
# The server address
WEB_SERVER_ADDR = 'http://localhost'

# The port used by the web server.
# On the Pi this should be 80 and the app run as sudo, when testing locally however use 8000
WEB_SERVER_PORT = 8000
# WEB_SERVER_PORT = 80

# Flasks secrity key
SECRET_KEY = 'MY_SECRET_KEY'

# Interval between samples (in milliseconds)
SAMPLE_INTERVAL = 250

# Serial rate
SERIAL_RATE = 9600

# Time to wait for connection to establish (in seconds)
CONNECTION_TIMEOUT = 10

# After connection established time to wait for init (in seconds)
CONNECTION_WAIT = 2