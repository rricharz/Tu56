// tu56.c

#define GDK_DISABLE_DEPRECATION_WARNINGS

#include <cairo.h>
#include <math.h>
#include <gtk/gtk.h>

#define TIME_INTERVAL    40		// timer interval in msec

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
  double angle;
  int light[4], tape1, tape2, buttonstate[6];
  long counter;
  int reelAindex, reelCindex;
  double scale;
} glob;


static void do_drawing(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{      
  do_drawing(cr);

  return FALSE;
}

static void do_drawing(cairo_t *cr)
{
	int index;
	
	// printf("do_drawing, glob.counter = %ld\n", glob.counter);
	
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
		index = 1 + (glob.counter % 2);
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
		index = 1 + (glob.counter % 2);
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
}

static void do_logic()
{
	
	glob.counter++;
	
	glob.light[0] = (glob.buttonstate[0] == 1);
	glob.light[1] = (glob.buttonstate[2] == 1);
	glob.light[2] = (glob.buttonstate[3] == 1);
	glob.light[3] = (glob.buttonstate[5] == 1);
	
	glob.tape1 = (glob.buttonstate[1] != 0) && (glob.buttonstate[2] == 2);
	glob.tape2 = (glob.buttonstate[4] != 0) && (glob.buttonstate[5] == 2);
	
}

static gboolean on_timer_event(GtkWidget *widget)
{
	do_logic();	
	if (glob.tape1 || glob.tape2) gtk_widget_queue_draw(widget);
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
				gtk_widget_queue_draw(widget);
				return TRUE;
		}
		if ((x >= buttonx[4]) && (x <= buttonx[4] + BUTTONXSIZE) &&
			(y >= buttony[4]) && (y <= buttony[4] + BUTTONYSIZE)) {
				glob.buttonstate[4] = 0;
				do_logic();
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
					if ((i == 0) || (i == 3))	// 2 state buttons
						glob.buttonstate[i] = 1;
					else if (glob.buttonstate[i] != 0)  // 3 state button
						glob.buttonstate[i] = 0;
					else
						glob.buttonstate[i] = 1;
						
				}
				else {
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
  
  int argFullscreen = 0;
  int firstArg = 1;
  
  printf("tu56 version 0.1\n");
  
  while (firstArg < argc) {
	if (strcmp(argv[firstArg],"-full") == 0)
		argFullscreen = 1;                
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
  
  glob.angle = 0.0;
  glob.light[0] = 0;
  glob.light[1] = 0;
  glob.light[2] = 0;
  glob.light[3] = 0;
  glob.tape1 = 0;
  glob.tape2 = 0;
  glob.counter = 0;
  glob.reelAindex = -1;
  glob.reelCindex = -1;
  glob.buttonstate[0] = 0;
  glob.buttonstate[1] = 0;
  glob.buttonstate[2] = 2;
  glob.buttonstate[3] = 0;
  glob.buttonstate[4] = 0;
  glob.buttonstate[5] = 2;

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
        
  if (argFullscreen) {        
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
