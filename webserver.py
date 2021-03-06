import json
import random
import asyncio
import concurrent
import time
import os
import click
from unipath import Path
from datetime import datetime
from flask import Flask, render_template, request, redirect, url_for, abort
from flask_socketio import SocketIO, send, emit
from config import *

UPLOAD_FOLDER = 'static/img'
ALLOWED_EXTENSIONS = set(['jpg', 'jpeg'])

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
app.config['SECRET_KEY'] = SECRET_KEY
socketio = SocketIO(app)

#
# URL Routing
#

# Index Page
@app.route('/')
def index():
    menu = [
        {"name": "Time Series" , "url": "/time", "img": "timeseries.png" },
        {"name": "2D Surface" , "url": "/surface2D", "img": "surface2D.png" },
        {"name": "3D Surface" , "url": "/surface3D", "img": "surface3D.png" },
        {"name": "Spotlight" , "url": "/spotlight", "img": "spotlight.png" },
        {"name": "Trail" , "url": "/trail", "img": "trail.png" },
        {"name": "Heatmap" , "url": "/heatmap", "img": "heatmap.png" },
        {"name": "Contour 2D" , "url": "/contour2D", "img": "contour2D.png" },
        {"name": "Contour 3D" , "url": "/contour3D", "img": "contour3D.png" }
    ]
    return render_template('index.html', menu=menu)

# Time Series Page
@app.route('/time')
def timeseries():
    return render_template('timeseries.html')

# 2D Surface Plots
@app.route('/surface2D')
def surface2D():
    return render_template('surface2D.html')

# Surface plot
@app.route('/surface3D')
def surface3D():
    return render_template('surface3D.html')

# Spotlight plot
@app.route('/spotlight', methods=['GET', 'POST'])
def spotlight():
    saveRoot = os.path.join(os.path.dirname(os.path.realpath(__file__)), app.config['UPLOAD_FOLDER'])
    msg = None
    if request.method == 'POST':
        file = request.files['file']
        if file and allowed_file(file.filename):
            filename = "spotlight.jpg"
            file.save(Path(saveRoot).child(filename))
        else:
            msg = 'Upload failed - Filename not allowed'
    return render_template('spotlight.html', trail=False, msg=msg)


# Trail plot
@app.route('/trail', methods=['GET', 'POST'])
def trail():
    saveRoot = os.path.join(os.path.dirname(os.path.realpath(__file__)), app.config['UPLOAD_FOLDER'])
    msg = None
    if request.method == 'POST':
        file = request.files['file']
        if file and allowed_file(file.filename):
            filename = "trail.jpg"
            file.save(Path(saveRoot).child(filename))
        else:
            msg = 'Upload failed - Filename not allowed'
    return render_template('spotlight.html', trail=True, msg=msg)


# Trail plot
@app.route('/heatmap')
def heatmap():
    return render_template('heatmap.html')

# contour2D
@app.route('/contour2D')
def contour2D():
    return render_template('contour2D.html')

# contour3D
@app.route('/contour3D')
def contour3D():
    return render_template('contour3D.html')


# Default context vars for all templates
@app.context_processor
def inject_defaults():
    return { "table_width": TABLE_WIDTH, 'table_depth': TABLE_DEPTH, 'table_height': TABLE_HEIGHT }

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1] in ALLOWED_EXTENSIONS


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
    # print('received json: ' + str(json))

#
# Send Message
#

def send_json(jsondata):
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    # Only send json strings with x,y,z values to javacript
    data = { 'timestamp': timestamp, 'data': jsondata }
    if 'x' in jsondata and 'y' in jsondata:
        socketio.emit('update', data)
    else:
        socketio.emit('other', data)

#
# Start App
#

@click.command()
@click.option('--prod', is_flag=True, default=False, help='Switch to production mode')
def main(prod):
    if not prod:
        servermode = 'local'
    else:
        servermode = 'prod'
    app.debug = True
    server_settings = WEB_SERVER[servermode]
    socketio.run(app, port=server_settings['port'], host=server_settings['host'])

if __name__ == '__main__':
   main()





