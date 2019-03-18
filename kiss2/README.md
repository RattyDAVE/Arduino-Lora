This software lets you use an arduino as an interface between a Linux system and some radio.
In the example code (mkiss.ino) I use the "RadioHead"-libray to use a LoRa radio to transmit AX.25 over.
Should also work with e.g. the RFM69HW or any other radio.

Required steps:
add a line to /etc/ax25/axports:
```
ax0	CALLSGN	9600	254	1	AX.25 over LoRa
```
ax0 is the interface name, CALLSGN is an AX.25 call-sign, 9600 is the speed to use when talking to the arduino and 254 is the maximum packet size that the radio can handle.

Then invoke:
```
kissattach /dev/ttyUSB0 ax0
```
This will connect /dev/ttyUSB0 (the serial device to which the Arduino is connected) to the network interface called ax0 (see axports).
In Debian the kissattach software can be found in the ax25-tools package.



```
modprobe ax25
modprobe mkiss
/usr/sbin/kissattach /dev/ttyUSB0 ax0 44.131.255.1
/usr/sbin/kissparms  -p ax0  -t 500  -s 200    -r 32       -l 100  -f n
/sbin/route add -net 44.0.0.0 netmask 255.0.0.0 dev ax0
```


```
/bin/ping -i 10 44.136.8.58   # (ping packets go out on RF each 10 secs, control-C to stop)

/usr/bin/axcall aprs k7xyz via sea   # (Connect packets go out on RF)
/usr/bin/axlisten -c -a  # (listens for packets and displays then in the term window, Control-C to stop)
```
