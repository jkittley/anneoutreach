#!/bin/bash

echo "Press CTRL and C to terminate. Do not close the window!"
echo "-------------------------------------------------------"

cd ~/www/ocean-scan/
git fetch --all
git reset --hard origin/master

echo "Updates downloaded"
echo "Moving useful tools"

sudo cp -i useful_tools/* ~/Desktop/
sudo chmod +x ~/Desktop/*.sh

echo "Done! Press enter to close window"
read
