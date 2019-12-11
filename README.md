# tu56: DECtape TU 56 front panel emulator

Simulation of a TU 56 DECtape front panel, version 0.2

For the Raspberry Pi and other Linux systems

![tu56 front panel](front.png?raw=true "tu56 front panel")

This [video of a tu56 demo](https://youtu.be/Ye_s0w6C970) shows the tu56 in action on a PiDP-11 and an
attached tek4010 terminal emulator.

The TU 56 DECtape had a very fast tape transportation speed for reads and writes of
81-105 inches per second, and an average rotation speed of the reels of 500 rpm.

A driver to use tu56 with the PiDP-11 and 2.11 BSD is included. It uses the magtape TQ
driver, because currently no DECtape driver is available with 2.11 BSD. It also adds
realistic execution times to the TQ driver. For details
on how to use TQ tapes on SimH on the PiDP-11 with 2.11 BSD see
[Using the historical Unix 2.11 BSD operating system on the PiDP-11](https://github.com/rricharz/pidp11-2.11bsd.git) 

Start the program with

 ./tu56			    for a 960x464 decorated window
 
 ./tu56 -full		for a full screen window, scaled to your display size

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

 ./tu56 &
 
Make sure that both right switches are in the "remote" position (this is the default).
Then start the demo program with

 ./demo
 
There is currently no 2 way communication back from the panel to the "host". Therefore, at the moment the demo
just flips the left button to the write enable position if required. But the the right buttons need
to be in the remote position. Otherwise the panel does not listen to a "host".

If you abort the demo program while it's running you might end up with reels spinning forever. Just use

 rm /tmp/tu56status
 
in this case, or start the demo program again and let it go through the complete demo taking
approx. 35 seconds.

"test.c" is easy to understand. If you have a real TU 56 you might want to modify it a bit to make it more
realistic. I would be very interested in a more realistic demo.

**Using tu56 with SimH on the PiDP-11, running 2.11 BSD**

First, install the modified TQ driver in SimH as follows. It's a good idea to make a backup copy of /opt/pidp11
before doing that, in case something goes wrong.

 cd
 cd tu56/simh
 ./putsource

Building a new version of SimH will take a while. There is also a copy of the original pdp11_tq.c there if you
want to go back to the original driver. The new TQ driver adds realistic timing and generates a status byte in
/tmp/tu56status, which can be used by tu56.

You have to restart simh after installing the new driver. The easiest way to do this is to restart your
Raspberry Pi.

Also, don't forget to enable the TQ driver in your boot.ini, see
[Using the historical Unix 2.11 BSD operating system on the PiDP-11](https://github.com/rricharz/pidp11-2.11bsd.git).
You have to restart 2.11 BSD after changing boot.ini.

Now you are ready to use the tu56 front panel with the PiDP-11. Just start the front panel at any time. It does
not matter whether this is done before or after SimH and BSD are started. The only thing which is important is that
the right switch on the left drive on the tu56 front panel is set to the "REMOTE" position. Boot to 2.11 BSD, and
use commands like "tar cv filename" or "tar xv filename" to see the tape in action. It is much more fun to use
"tar" if you can see the tape in action.

**Contributors**

The pictures used to make the animated screen have been provided by David Gesswein
[Online PDP-8 Home Page](https://www.pdp8.net/tu56/tu56.shtml) and Henk Gooijen. Jon Brase
had the idea to use rotational motion blurring for the rotating tape reels.


**The usual disclaimer**

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
