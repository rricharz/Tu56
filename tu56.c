/*
 * tu56.c
 *
 * A visual display of the DECtape TU56 front panel
 * 
 * for the Raspberry Pi and other Linux systems
 * 
 * Copyright 2019  rricharz
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#define TSTATE_ONLINE		 1
#define TSTATE_DRIVE1		 2
#define TSTATE_BACKWARDS	 4
#define TSTATE_SEEK		 8
#define TSTATE_READ		16
#define TSTATE_WRITE		32

#define GDK_DISABLE_DEPRECATION_WARNINGS

#include <cairo.h>
#include <math.h>
#include <gtk/gtk.h>

// The TU56 tape status is checked every TIME_INTERVAL milliseconds
// If the status changes, the display is updated immediately
//
// Blurred rotational motion pictures of the reels are only updated
// every TIME_INTERVAL * C_ANIMATION, to reduce CPU usage
//
// Tape motion on and off times are extended to a minimal time of
// TIME_INTERVAL * C_TAPE to make them visible 

#define TIME_INTERVAL    25		// timer interval in msec
#define C_ANIMATION		  4		// blurred tape animation cycles
#define C_TAPE            8     // tape on off minimum cycles

#define REELAX			 52
#define REELAY			524
#define REELBX			513
#define REELBY			525
#define REELCX			979
#define REELCY			524
#define REELDX		   1439
#define REELDY			526

int lightx[4]  = {131, 812, 1037, 1725};
int lighty[4]  = {79, 77, 77, 77};
int buttonx[6] = {198, 546, 741, 1099, 1453, 1649};
int buttony[6] = {78, 78, 78, 78, 78, 78};

#define BUTTONXSIZE		68
#define BUTTONYSIZE		150


struct {
  cairo_surface_t *image;
  cairo_surface_t *light1on, *lightoff, *light2on;
  cairo_surface_t *reelA[3], *reelB[3], *reelC[3], *reelD[3], *button[3];
  int light[4], tape1, tape2, buttonstate[6];
  int reelAindex, reelCindex;
  double scale;
  int remote_status, last_remote_status, reelsound, updated;
  int argFullscreen, argAudio;
} glob;

int getStatus()
{
	static FILE *statusFile = 0;
	char *fname = "/tmp/tu56status";
	int st;
	
	// file needs to be opened again each time to read new status
	statusFile = fopen(fname, "r");
		
	if (statusFile != 0) {
		st = getc(statusFile) - 32;
		fclose(statusFile);
		if (st >= 0)
			return st;
		else
			return 0;
	}
	else return 0;
}

static void do_drawing(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{      
  do_drawing(cr);

  return FALSE;
}

static void do_drawing(cairo_t *cr)
{
	static int animation_counter = 0;
	
	int index;
	
	animation_counter++;
	
	// printf("do_drawing, animation_counter = %ld\n", animation_counter);
	
	cairo_scale(cr,glob.scale,glob.scale);
	
	cairo_set_source_surface(cr, glob.image, 0, 0);
	cairo_paint(cr);
	
	if (glob.light[0]) {
		cairo_set_source_surface(cr, glob.light1on, lightx[0], lighty[0]);
	}
	else {
		cairo_set_source_surface(cr, glob.lightoff, lightx[0], lighty[0]);
	}
	cairo_paint(cr);
	
	if (glob.light[1]) {
		cairo_set_source_surface(cr, glob.light2on, lightx[1], lighty[1]);
	}
	else {
		cairo_set_source_surface(cr, glob.lightoff, lightx[1], lighty[1]);
	}
	cairo_paint(cr);
	
	if (glob.light[2]) {
		cairo_set_source_surface(cr, glob.light1on, lightx[2], lighty[2]);
	}
	else {
		cairo_set_source_surface(cr, glob.lightoff, lightx[2], lighty[2]);
	}
	cairo_paint(cr);
	
	if (glob.light[3]) {
		cairo_set_source_surface(cr, glob.light2on, lightx[3], lighty[3]);
	}
	else {
		cairo_set_source_surface(cr, glob.lightoff, lightx[3], lighty[3]);
	}
	cairo_paint(cr);
	
	if (glob.tape1) {
		index = 1 + (animation_counter % 2);
	}
	else {
		index = 0;
	}
	if (index != glob.reelAindex) {
		cairo_set_source_surface(cr, glob.reelA[index], REELAX, REELAY);
		cairo_paint(cr);
		cairo_set_source_surface(cr, glob.reelB[index], REELBX, REELBY);
		cairo_paint(cr);
	}
	glob.reelAindex = index;
	
	if (glob.tape2) {
		index = 1 + (animation_counter % 2);
	}
	else {
		index = 0;
	}
	if (index != glob.reelCindex) {
		cairo_set_source_surface(cr, glob.reelC[index], REELCX, REELCY);
		cairo_paint(cr);
		cairo_set_source_surface(cr, glob.reelD[index], REELDX, REELDY);
		cairo_paint(cr);
	}
	glob.reelCindex = index;
	
	for (int i = 0; i < 6; i++) {	
		cairo_set_source_surface(cr, glob.button[glob.buttonstate[i]], buttonx[i], buttony[i]);
		cairo_paint(cr);
	}
	
	if (((glob.tape1 == 0) && (glob.tape2 == 0)) && (glob.reelsound != 0)) {
			glob.reelsound = 0;
			// printf("turn reel sound off , tape1=%d, tape2=%d\n", glob.tape1, glob.tape2);
			if (glob.argAudio) system("pkill mpg321");
		}	 
}

static void do_logic()
{	
	glob.light[0] = 0;
	glob.light[1] = 0;
	glob.light[2] = 0;
	glob.light[3] = 0;
	
	glob.tape1 = (glob.buttonstate[1] != 0) && (glob.buttonstate[2] == 2);
	glob.tape2 = (glob.buttonstate[4] != 0) && (glob.buttonstate[5] == 2);
	
	if ((glob.buttonstate[2] == 1) || (glob.buttonstate[5] == 1)) {
		// if remote is set in a drive
		glob.remote_status = getStatus();
		if (glob.last_remote_status != glob.remote_status)
			printf("remote state = 0x%02x\n", glob.remote_status);
		
		// set the lights and motors
		// for now it also flips the write enable button if necessary
		if ((glob.remote_status & TSTATE_DRIVE1) && (glob.buttonstate[5] == 1)) {
			if (glob.remote_status & TSTATE_WRITE)
				glob.buttonstate[3] = 1;
			glob.light[2] = (glob.remote_status & TSTATE_WRITE);
			glob.light[3] = (glob.remote_status & TSTATE_ONLINE);
			glob.tape2    = (glob.remote_status & (TSTATE_READ | TSTATE_WRITE | TSTATE_SEEK));
		}
		else if (glob.buttonstate[2] == 1) {
			if (glob.remote_status & TSTATE_WRITE)
				glob.buttonstate[0] = 1;
			glob.light[0] = (glob.remote_status & TSTATE_WRITE);
			glob.light[1] = (glob.remote_status & TSTATE_ONLINE);
			glob.tape1    = (glob.remote_status & (TSTATE_READ | TSTATE_WRITE | TSTATE_SEEK));
		}
	}
	else
		glob.remote_status = 0;
	
	if (((glob.tape1 != 0) || (glob.tape2 != 0)) && (glob.reelsound == 0)) {
			glob.reelsound = 1;
			// printf("turn reel sound on , tape1=%d, tape2=%d\n", glob.tape1, glob.tape2);
			if (glob.argAudio) system("/usr/bin/mpg321 -q -l 0 sound/reels.mp3 2> /dev/null &");
		}
}

void extend_short_tape_events()
// extend short tape on/off events to make them visible
{
	static int last_tape1 = -1;
	static int last_tape2 = -1;	
	static int tape1_counter = 0;
	static int tape2_counter = 0;
	
	if (last_tape1 != glob.tape1) {
		if (tape1_counter == 0) {
			// start counting
			tape1_counter = C_TAPE + 1;
		}
		else if (tape1_counter == 1) {
			// stop counting
			tape1_counter = 0;
			glob.updated = 1;
		}
		else {
			// do not yet change tape status
			glob.tape1 = last_tape1;
			// printf("Extending tape 1 change\n");
		}
	}
	if (tape1_counter > 0) tape1_counter--;
	last_tape1 = glob.tape1;
	
	if (last_tape2 != glob.tape2) {
		if (tape2_counter == 0) {
			// start counting
			tape2_counter = C_TAPE + 1;
		}
		else if (tape2_counter == 1) {
			// stop counting
			tape2_counter = 0;
			glob.updated = 1;
		}
		else {
			// do not yet change tape status
			glob.tape2 = last_tape2;
			// printf("Extending tape 2 change\n");
		}
	}
	if (tape2_counter > 0) tape2_counter--;
	last_tape2 = glob.tape2;
}

static gboolean on_timer_event(GtkWidget *widget)
{
	static int timer_counter = 0;
	
	timer_counter++;
	
	do_logic();
	glob.updated = 0;
	extend_short_tape_events();	
	if ((glob.last_remote_status != glob.remote_status) || glob.updated)
		gtk_widget_queue_draw(widget);
	else if (((timer_counter % C_ANIMATION) == 0) && (glob.tape1 || glob.tape2))
		gtk_widget_queue_draw(widget);
	glob.last_remote_status = glob.remote_status;
	return TRUE;
}

static gboolean on_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	// event-button = 1: means left mouse button; button = 3 means right mouse button    
    // printf("on_button_release_event called, button %d, x = %d, y= %d\n", (int)event->button, (int)event->x, (int)event->y);
	int x = (int)((double)event->x / glob.scale);
    int y = (int)((double)event->y / glob.scale);
    if (event->button == 1) {
		if ((x >= buttonx[1]) && (x <= buttonx[1] + BUTTONXSIZE) &&
			(y >= buttony[1]) && (y <= buttony[1] + BUTTONYSIZE)) {
				glob.buttonstate[1] = 0;
				do_logic();
				if (glob.argAudio) system("/usr/bin/mpg321 -q sound/switch.mp3 2> /dev/null &");
				gtk_widget_queue_draw(widget);
				return TRUE;
		}
		if ((x >= buttonx[4]) && (x <= buttonx[4] + BUTTONXSIZE) &&
			(y >= buttony[4]) && (y <= buttony[4] + BUTTONYSIZE)) {
				glob.buttonstate[4] = 0;
				do_logic();
				if (glob.argAudio) system("/usr/bin/mpg321 -q sound/switch.mp3 2> /dev/null &");
				gtk_widget_queue_draw(widget);
				return TRUE;
		}
	}
}

static gboolean on_button_click_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	// event-button = 1: means left mouse button; button = 3 means right mouse button    
    // printf("on_button_click_event called, button %d, x = %d, y= %d\n", (int)event->button, (int)event->x, (int)event->y);
    
    int x = (int)((double)event->x / glob.scale);
    int y = (int)((double)event->y / glob.scale);
    if (event->button == 1) {
		for (int i = 0; i < 6; i++) {
			if ((x >= buttonx[i]) && (x <= buttonx[i] + BUTTONXSIZE) &&
				(y >= buttony[i]) && (y <= buttony[i] + BUTTONYSIZE)) {
					
				if (y <= buttony[i] + (BUTTONYSIZE / 2)) {
					if (glob.argAudio) system("/usr/bin/mpg321 -q sound/switch.mp3 2> /dev/null &");
					if ((i == 0) || (i == 3))	// 2 state buttons
						glob.buttonstate[i] = 1;
					else if (glob.buttonstate[i] != 0)  // 3 state button
						glob.buttonstate[i] = 0;
					else
						glob.buttonstate[i] = 1;
						
				}
				else {
					if (glob.argAudio) system("/usr/bin/mpg321 -q sound/switch.mp3 2> /dev/null &");
					if ((i == 0) || (i == 3))	// 2 state buttons
					glob.buttonstate[i] = 2;
					else if (glob.buttonstate[i] != 0)  // 3 state button
						glob.buttonstate[i] = 0;
					else
						glob.buttonstate[i] = 2;
				}
				do_logic();
				gtk_widget_queue_draw(widget);
				// printf("button state %d: %d\n",i,glob.buttonstate[i]);
				return TRUE;
			} 
		}
	}
    return TRUE;
}

static void on_quit_event()
{
	system("pkill mpg321");
	gtk_main_quit();
    exit(0);
}

static void on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    int ch;
    // printf("key pressed, state =%04X, keyval =%04X\n", event->state, event->keyval);
    if ((event->state == 0x4) && (event->keyval == 0x0063)) // ctrl-c
		on_quit_event();
	else if ((event->state == 0x4) && (event->keyval == 0x0071)) // ctrl-q
		on_quit_event();
	else if ((event->state == 0x0) && (event->keyval == 0xFF1B)) // esc
		on_quit_event();	
}        

cairo_surface_t* readpng(char* s)
{
	cairo_surface_t *t = cairo_image_surface_create_from_png(s);
	if ((t == 0) || cairo_surface_status(t)) {
		printf("Cannot load %s\n",s);
		exit(1);
	}
	return t;
}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *darea;
  
  glob.argFullscreen = 0;
  glob.argAudio = 0;
  int firstArg = 1;
  
  printf("tu56 version 0.5\n");
  
  system("pkill mpg321");
  
  while (firstArg < argc) {
	if (strcmp(argv[firstArg],"-full") == 0)
		glob.argFullscreen = 1;
	else if (strcmp(argv[firstArg],"-audio") == 0)
		glob.argAudio = 1;            
    else {
        printf("tu56: unknown argument %s\n", argv[firstArg]);
        exit(1);
    }
    firstArg++;
  }
  
  glob.image     = readpng("front.png");
  glob.light1on  = readpng("light1on.png");
  glob.light2on  = readpng("light2on.png");
  glob.lightoff  = readpng("lightoff.png");
  glob.reelA[0]   = readpng("reelA0.png");
  glob.reelA[1]   = readpng("reelA1.png");
  glob.reelA[2]   = readpng("reelA2.png");
  glob.reelB[0]   = readpng("reelB0.png");
  glob.reelB[1]   = readpng("reelB1.png");
  glob.reelB[2]   = readpng("reelB2.png");
  glob.reelC[0]   = readpng("reelC0.png");
  glob.reelC[1]   = readpng("reelC1.png");
  glob.reelC[2]   = readpng("reelC2.png");
  glob.reelD[0]   = readpng("reelD0.png");
  glob.reelD[1]   = readpng("reelD1.png");
  glob.reelD[2]   = readpng("reelD2.png");
  glob.button[0]  = readpng("button0.png");
  glob.button[1]  = readpng("button1.png");
  glob.button[2]  = readpng("button2.png");
  
  glob.light[0] = 0;
  glob.light[1] = 0;
  glob.light[2] = 0;
  glob.light[3] = 0;
  glob.tape1 = 0;
  glob.tape2 = 0;
  glob.reelAindex = -1;
  glob.reelCindex = -1;
  glob.buttonstate[0] = 0;
  glob.buttonstate[1] = 0;
  glob.buttonstate[2] = 1;
  glob.buttonstate[3] = 0;
  glob.buttonstate[4] = 0;
  glob.buttonstate[5] = 1;
  
  glob.last_remote_status = 0;
  glob.last_remote_status = 0;
  glob.reelsound = 0;
  glob.updated = 0;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  
  // set the background color
  GdkColor color;
  color.red   = 0;
  color.green = 0;
  color.blue  = 0;
  gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);

  darea = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER (window), darea);

  g_signal_connect(G_OBJECT(darea), "draw", 
      G_CALLBACK(on_draw_event), NULL); 
  g_signal_connect(window, "destroy",
      G_CALLBACK (gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  
  GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(window));
  int screenWidth = gdk_screen_get_width(screen);
  int screenHeight = gdk_screen_get_height(screen);
  printf("Screen dimensions: %d x %d\n", screenWidth, screenHeight);
        
  if (glob.argFullscreen) {        
    // DISPLAY UNDECORATED FULL SCREEN WINDOW
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
	gtk_window_fullscreen(GTK_WINDOW(window));
	gtk_window_set_keep_above(GTK_WINDOW(window), FALSE);
	glob.scale = screenWidth / 1920.0;
  }
 
  else {
    // DISPLAY DECORATED WINDOW
    gtk_window_set_decorated(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 960, 464);
    glob.scale = 0.5;                
  } 
  
  gtk_window_set_title(GTK_WINDOW(window), "tu56");
  
  g_signal_connect(G_OBJECT(window), "button-press-event", G_CALLBACK(on_button_click_event), NULL);
  g_signal_connect(G_OBJECT(window), "button-release-event", G_CALLBACK(on_button_release_event), NULL);
  g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(on_key_press), NULL);
  
  if (TIME_INTERVAL > 0) {
		// Add timer event
		// Register the timer and set time in mS.
		// The timer_event() function is called repeatedly until it returns FALSE. 
		g_timeout_add(TIME_INTERVAL, (GSourceFunc) on_timer_event, (gpointer) window);
	}

  gtk_widget_show_all(window);

  gtk_main();

  cairo_surface_destroy(glob.image);

  return 0;
}
