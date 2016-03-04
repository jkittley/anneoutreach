import json
import random
import asyncio
import concurrent
import time
from datetime import datetime
from flask import Flask, render_template
from flask_socketio import SocketIO, send, emit
from config import *

app = Flask(__name__)
app.config['SECRET_KEY'] = SECRET_KEY
socketio = SocketIO(app)

#
# URL Routing
#

# Index Page
@app.route('/')
def index():
    return render_template('index.html')

# Surface plot
@app.route('/surface')
def surface():
    return render_template('surfaceVis.html')

#
# Message receivers
#

# All messages with unknown event
@socketio.on('message')
def handle_message(message):
    print('received message: ' + message)


# Test event
@socketio.on('new_serial_data')
def handle_json(json):
    send_json(json)
    print('received json: ' + str(json))

#
# Send Message
#

def send_json(jsondata):
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    socketio.emit('update', { 'timestamp': timestamp, 'data': jsondata })

#
# Start App
#

if __name__ == '__main__':
    app.debug = True
    socketio.run(app, port=WEB_SERVER_PORT)





