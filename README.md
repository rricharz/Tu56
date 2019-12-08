# Tu56

Simulation of TU56 DECtape, version 0.1

This unfinished version is operational, but supports only offline mode.
The connection to simh has not yet been implemented.

The TU 56 DECtape had a very fast tape transportation speed for reads and writes of
81-105 inches per second, and an average rotation speed of the reels of 500 rpm.

Start the program with

  ./tu56			for a 960x464 decorated window
  ./tu56 -full		for a full screen window

All switches and lights are funktional. Click either in the upper or lower half of a switch.

In order to transport tape in offline mode:

 - The right switch must be in the "local" position.
 - Click ond hold the center switch.
 
The following keys can be used to exit from tu56 and close the corresponding window. These keys are
especially useful in full screen mode where no close window button is available.

 - ESC
 - ctrl-C
 - ctrl-Q

**Contributors**

The pictures used to make the animated screen have been provided by David Gesswein
[Online PDP-8 Home Page](https://www.pdp8.net/tu56/tu56.shtml) and Henk Gooijen. Jon Brase
had the idea to use rotational motion blurring for the rotating tape reels.


**The usual disclaimer**

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
