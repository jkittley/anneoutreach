#!/bin/bash
echo "Press CTRL and C to terminate. Do not close the window!"
echo "-------------------------------------------------------"

sudo python3 ~/www/ocean-scan/webserver.py --prod

echo "Press enter to close window"
read