# anneoutreach

This repo contains a basic framework which allows JSON encoded serial data outputted from an Arduino (or other serial 
capable dev board) to be passed to a browser via a web socket on local hardware. 



This repo holds the code associated with the post: http://thing.farm/ocean-scan. For full details about the projects 
please read the project page, however in short 



## Setup 
First of all I am going to assume you have a Raspberry Pi 3 with Raspbian installed and setup to connect to the internet.
Older models of the Pi will work, but if you intend to use the Pi's browser to display the page it is worth using the 
latest model.

Launch command line and run the following to insure the system is up-to-date
> sudo apt-get update

Next make sure you have the Epiphany browser installed
sudo apt-get install epiphany-browser

Check you have Python 3 and Pip installed
sudo apt-get install python3-pip

Move to the user directory
cd ~/

Make a new directory for the web server and move into it
mkdir www 
cd www

Download the project code from github
git clone https://github.com/jkittley/anneoutreach.git

Move into the directory 
cd ocean-scan

Install all the python requirements
pip3 install -r requirements.txt

Copy all the files from the from useful_tools to the raspberry pi desktop 
cp useful_tools/*.sh ~/Desktop

You must then make the usefull tools executable
chmod +x ~/Desktop/*.sh

That's it, now we can launch the services.

Yo can either run: 
>python3 serialmonitor.py --prod &
>sudo python3 webserver.py —prod

or double click on the new icons on the desktop and choose execute in terminal

Once the server is up and running you can open the browser and go to localhost to see the website or from a remote 
machine you can enter the Raspberry Pi's IP address. The IP address can be discovered by double clicking on the 
"whats_my_ip" usefull tool now on the desktop  



