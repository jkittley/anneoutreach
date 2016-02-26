from datetime import datetime
import json
import random
from flask import Flask, render_template
from flask_socketio import SocketIO, send, emit
import math
import asyncio
import concurrent
import time

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/test/')
def test():
    x = math.floor(random.random() * 20) + 1
    y = math.floor(random.random() * 20) + 1
    z = math.floor(random.random() * 20) + 1
    send_data(x, y, z);
    return "Test data sent"

#
# Message receivers
#

# All messages with unknown event
@socketio.on('message')
def handle_message(message):
    print('received message: ' + message)


# Test event
@socketio.on('incomming')
def handle_json(json):
    send_data(json['x'], json['y'], json['z'])
    print('received json: ' + str(json))

#
# Send Message
#


def send_data(x, y, z):
    t = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    socketio.emit('newdata', { 't': t, 'x': x, 'y':y, 'z':z })


#
# Start App
#

if __name__ == '__main__':
    app.debug = True
    socketio.run(app)





