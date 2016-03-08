#!/bin/bash

echo "Press CTRL and C to terminate. Do not close the window!"
echo "-------------------------------------------------------"

source ~/wwww/venv/bin/activate
sudo python3 ~/www/ocean-scan/serialmonitor.py --prod

echo "Press enter to close window"
read
