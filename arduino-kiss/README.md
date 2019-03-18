The goal of this KISS TNC is to be able to use LoRa as the Physical Layer of an AX.25 Radio Network. I have created a simple shell to execute commands to setup your KISS TNC on Raspberry Pi.

```
apt-get install ax25-apps ax25-tools ax25-xtools

echo "lora1   G7EZW-0        19200    231     2       LoRa KISS_NO_CRC" >> /etc/ax25/axports

echo "[G7EZW-0 VIA lora1]" >> /etc/ax25/ax25d.conf
echo "NOCALL   * * * * * *    L" >> /etc/ax25/ax25d.conf
echo "default  * * * * * *    -       root /usr/sbin/ax25-node ax25-node" >> /etc/ax25/ax25d.conf
```

The command you can use to run the shell is:

```
$ sudo ./kiss.sh <tty port> <ax25 port name> <window>
```

./kiss.sh /dev/ttyUSB0 lora1 1


Currently, there's a known bug that truncates frame if you send more than two packets consecutively, to prevent this from happening you can set standard_window_size to 1.

Before running the shell, we need to make sure to set our KISS TNC in the config files:

```
# nano /etc/ax25/axports
```

In which you will insert a line that goes something like this:

```
# /etc/ax25/axports
#
# The format of this file is:
#
# name callsign speed paclen window description
#
lora   G7EZW-0        19200    231     2       LoRa KISS_NO_CRC
```

1. Portname can be any characters you like, for example, mine is "lora1"
2. Callsign is your amateur radio callsign, for LoRa, you can do any 6 Alphanumeric characters separated by a "-" with a number from 0-15, for example, mine is "YD0SHY-0"
3. Speed is the speed of the modem you use, for this modem it is running on 9600 baud
4. Paclen is the maximum packet length the modem can handle, for example, mine is 231. There's a reason the packet length is 231, since LoRa has 256 bytes buffer, 1 byte is used for length, 4 bytes are used for RadioHead addressing, 1 byte for KISS Command Byte, 21 bytes are used for AX.25 addressing (assuming 14 bytes (source + destination) and 7 bytes (digipeater), and 2 bytes for Control and PID bytes. This leads to only 256-(1+1+4+21+2) which is 227, however I put it on 231 since I am planning to re-write the RadioHead so that the 4 bytes LoRa addressing would no longer be needed.
5. Window is the maximum outstanding packet can be sent without acknowledge from the receiving end, this is done to prevent KISS TNC from receiving more than one packet consecutively.

Next what you need to do is add configuration files to AX.25 Daemon so that the Daemon can handle call request to your domain. To access the AX.25 Daemon Config, what you need to do is:

```
# nano /etc/ax25/ax25d.conf
```

In which you will insert a line that goes something like this:

```
[G7EZW-0 VIA lora1]
NOCALL   * * * * * *    L
default  * * * * * *    -       root /usr/sbin/ax25-node ax25-node
```

What the above configuration file means is that when a node calls for G7EZW-1 on port lora1 it will answer by executing the program /usr/sbin/ax25-node ax25-node, the asterisks before indicate the AX.25 parameters you wish to use when running the program, the asterisks representing a "set as a default mode", so it will follow any configurations you have set on AX.25 ports, you can see the default at /proc/sys/net/ax25/ax0 (ax0 can be ax1, etc. if you have more than one AX.25 port).
