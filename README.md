# tu56: DECtape TU 56 front panel emulator

Simulation of a TU 56 DECtape front panel, version 0.3

For the Raspberry Pi and other Linux systems.

New in version 0.3: Audio support on the HDMI screen for switches and reel motions, see below.

![tu56 front panel](front.png?raw=true "tu56 front panel")

This [video of a tu56 demo](https://youtu.be/Ye_s0w6C970) shows the tu56 in action on a PiDP-11 and an
attached tek4010 terminal emulator.

The TU 56 DECtape had a very fast tape transportation speed for reads and writes of
81-105 inches per second, and an average rotation speed of the reels of 500 rpm.

A driver to use tu56 with the PiDP-11 and PiDP-8 are included.

**Download the program**

I recommend using "git clone" to download the program. This allows for very easy upgrades.

```
  sudo apt-get update
  sudo apt-get upgrade
  sudo apt-get install git
  cd
  git clone --depth 1 git://github.com/rricharz/Tu56
  cd Tu56
```

To upgrade the program later, type

```
  cd
  cd Tu56
  git pull
```

If you want to recompile the program, proceed as follows

```
  cd
  cd Tu56
  sudo apt-get install libgtk-3-dev
  touch tu56.c
  make
```

Start the program with

```
  cd
  cd Tu56
  
  ./tu56	for a 960x464 decorated window 
  ./tu56 -full	for a full screen window, scaled to your display size
```

All switches and lights are funktional. Click either in the upper or lower half of a switch.

The following keys can be used to exit from tu56 and close the corresponding window. These keys are
especially useful in full screen mode where no close window button is available.

 - ESC
 - ctrl-C
 - ctrl-Q

**Transport tape in offline mode**

 - The right switch must be in the "local" position.
 - Click and hold the center switch.
 
**Demo of tape copy**

There is a small demo program which demonstates a data transfer between drive 0 and drive1. It is also
a test for the communication between a simulated "host" (the demo program in this case) and tu56.

Start tu56 in background (with the &) using

```
  ./tu56 &
```
 
Make sure that both right switches are in the "remote" position (this is the default).
Then start the demo program with

```
  sudo ./demo
```
 
Sudo might be required if SimH has written to the status byte file before. There is currently no 2 way communication back from the panel to the "host". Therefore, at the moment the demo
just flips the left button to the write enable position if required. But the the right buttons need
to be in the remote position. Otherwise the panel does not listen to a "host".

If you abort the demo program while it's running you might end up with reels spinning forever. Just use

```
 sudo rm /tmp/tu56status
```
 
in this case, or start the demo program again and let it go through the complete demo taking
approx. 35 seconds.

"test.c" is easy to understand. If you have a real TU 56 you might want to modify it a bit to make it more
realistic. I would be very interested in a more realistic demo.

**Turning on realistic sounds**

I recommend using a Raspberry Pi 4 to achieve an optimal synchronization between the audio and video
streams.

Install "mpg321" using

```
 sudo apt-get update
 sudo apt-get upgrade
 sudo apt-get install mpg321
```

Test audio as follows

```
 cd
 cd Tu56
 mpg321 sound/switch.mp3
```

Run "tu56" as follows:

```
 cd
 cd Tu56
 tu56 -audio
```

"tu56" will only find the necessary sound files if Tu56 is the current directory.


**Installing the proper driver in SimH**

A slightly modified tape driver needs to be installed in SimH. This driver writes the necessary
status bits into a little status file, where it can be read by tu56.

There are currently 3 drivers available:

 - a DECtape driver for the PiDP-8
 - a DECtape driver for the PiDP-11
 - a magtape driver for the PiDP-11

**Instructions for different operating systems**

 - [Using tu56 with 2.11 BSD on the PiDP-11](bsd.txt)
 - [Using tu56 with RSX-11 on the PiDP-11](rsx11.txt)
 - [Using tu56 on the PiDP-8](pidp8.txt)

Improved instructions are very much appreciated. Please send improved versions of these
instructions, and instructions for other operating systems to rricharz77@gmail.com


**Contributors**

The pictures used to make the animated screen have been provided by David Gesswein
[Online PDP-8 Home Page](https://www.pdp8.net/tu56/tu56.shtml) and Henk Gooijen. Jon Brase
had the idea to use rotational motion blurring for the rotating tape reels. Johnny Bilquist
has helped me to understand many important details of DECtapes and magtapes and how they are
used on the PDP-11. Neal G. has helped to setup a RSX-11 test system, and Warren Young to
setup a PDP-8 test system.


**The usual disclaimer**

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
