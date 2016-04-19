# Ocean-Scan

### Arduino --[serial]--> Raspberry Pi --[web socket]--> Webserver (can also be on Pi) --[web socket]--> HTML5 Webpages

This repo contains a basic framework which allows JSON encoded data to be transfered from an Arduino (or other serial 
capable dev board) to locally hosted website (on a Raspberry Pi). It uses web sockets to provide realtime updates. This
 code was required as part of a larger project whcih can be found here: http://thing.farm/ocean-scan. 

## Basic Setup 
First of all I am going to assume you have a Raspberry Pi 3 with Raspbian installed and setup to connect to the internet.
Older models of the Pi will work, but if you intend to use the Pi's browser to display a page, it is worth using the 
latest model.

First job is to make sure the file system is expanded to use the full capacity of the SD Card
* Open terminal and type “raspi-config”. 
* Choose the “expand Filesystem” option and reboot the Pi

Launch command line and run the following to insure the system is up-to-date
* sudo apt-get update

Next make sure you have the Epiphany browser installed
* sudo apt-get install epiphany-browser

Check you have Python 3 and Pip installed
* sudo apt-get install python3-pip

Make a new directory for the web server and move into it
* mkdir ~/www 
* cd ~/www

Download the project code from github
* git clone https://github.com/jkittley/ocean-scan.git

Move into the directory 
* cd ocean-scan

Install all the python requirements
* sudo pip3 install -r requirements.txt

Copy all the files from the from useful_tools to the raspberry pi desktop 
* sudo cp useful_tools/*.sh ~/Desktop

You must then make the usefull tools executable
* sudo chmod +x ~/Desktop/*.sh

That's it, now we can launch the services.

You can either run: 
* python3 serialmonitor.py --prod &
* sudo python3 webserver.py —prod

or double click on the new icons on the desktop and choose execute in terminal

Once the server is up and running you can open the browser and go to localhost to see the website or from a remote 
machine you can enter the Raspberry Pi's IP address. The IP address can be discovered by double clicking on the 
"whats_my_ip" useful tool now on the desktop  


## Access Point Setup (Raspberry Pi 3 Only)
If you want people to be able to access the hosted web pages via WiFi you can setup the Pi3 to act as a local Access Point.
This section was modified from https://frillip.com/using-your-raspberry-pi-3-as-a-wifi-access-point-with-hostapd/

First install the two packages required
* sudo apt-get install dnsmasq hostapd

Now we need to set a static IP address for wlan0
* sudo nano /etc/dhcpcd.conf
* Add the following to the bottom of the file:

>interface wlan0  
>    static ip_address=172.24.1.1/24

Next we need to prevent wpa_supplicant from running and interfering with setting up wlan0 in access point mode.
* sudo nano /etc/network/interfaces
* Comment out the line that looks like:

>allow-hotplug wlan0  
>iface wlan0 inet manual  
>\#   wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf

Restart dhcpcd
* sudo service dhcpcd restart

Configure HostAPD
* sudo nano /etc/hostapd/hostapd.conf

```
\# This is the name of the WiFi interface we configured above.
interface=wlan0

\# Use the nl80211 driver with the brcmfmac driver.
driver=nl80211

\# This is the name of the network.
ssid=Ocean-Scan

\# Use the 2.4GHz band.
hw_mode=g

\# Use channel 6.
channel=6

\# Enable 802.11n.
ieee80211n=1

\# Enable WMM.
wmm_enabled=1

\# Enable 40MHz channels with 20ns guard interval.
ht_capab=[HT40][SHORT-GI-20][DSSS_CCK-40]

\# Accept all MAC addresses.
macaddr_acl=0

\# Use WPA authentication.
auth_algs=1

\# Require clients to know the network name.
ignore_broadcast_ssid=0

\# Use WPA2.
wpa=2

\# Use a pre-shared key.
wpa_key_mgmt=WPA-PSK

\# The network passphrase.
wpa_passphrase=raspberry

\# Use AES, instead of TKIP.
rsn_pairwise=CCMP
```

We can check if it's working at this stage by running sudo /usr/sbin/hostapd /etc/hostapd/hostapd.conf. 
If it's all gone well thus far, you should be able to see to the network Pi3-AP! If you try connecting to it, 
you will see some output from the Pi, but you won't receive and IP address until we set up dnsmasq in the next step. 
Use Ctrl+C to stop it.

We also need to tell hostapd where to look for the config file when it starts up on boot
* sudo nano /etc/default/hostapd
* find the line #DAEMON_CONF=""
* replace it with DAEMON_CONF="/etc/hostapd/hostapd.conf"

Configure DNSMASQ

First move the config file out of the way
* sudo mv /etc/dnsmasq.conf /etc/dnsmasq.conf.orig  

Now make a new one
* sudo nano /etc/dnsmasq.conf
* paste the following into the file

> interface=wlan0      # Use interface wlan0  
> bind-interfaces      # Bind to the interface to make sure we aren't sending things elsewhere  
> server=8.8.8.8       # Forward DNS requests to Google DNS  
> domain-needed        # Don't forward short names  
> bogus-priv           # Never forward addresses in the non-routed address spaces.  
> dhcp-range=172.24.1.50,172.24.1.150,12h # Assign IP addresses between 172.24.1.50 and 172.24.1.150 with a 12 hour lease time  

One of the last things that we need to do before we send traffic anywhere is to enable packet forwarding.
* sudo nano /etc/sysctl.conf
* remove the # from the beginning of the line containing net.ipv4.ip_forward=1
* save and reboot it:
* sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward" 

Start the services
* sudo service hostapd start  
* sudo service dnsmasq start

Reboot the Pi
* sudo reboot