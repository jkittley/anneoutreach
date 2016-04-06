#!/bin/bash

echo "Press CTRL and C to terminate. Do not close the window!"
echo "-------------------------------------------------------"

cd ~/www/ocean-scan/
git fetch --all
git reset --hard origin/master

echo "Done! Press enter to close window"
read
