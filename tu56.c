// tu56.c

#include <cairo.h>
#include <math.h>
#include <gtk/gtk.h>

#define TIME_INTERVAL    40		// timer interval in msec

#define LIGHT1X         130
#define LIGHT1Y			 83
#define LIGHT2X         814
#define LIGHT2Y			 77
#define REELAX			 52
#define REELAY			524
#define REELBX			513
#define REELBY			525
#define REELCX			979
#define REELCY			524
#define REELDX		   1439
#define REELDY			526


struct {
  cairo_surface_t *image;
  cairo_surface_t *light1on, *lightoff, *light2on;
  cairo_surface_t *reelA[3], *reelB[3], *reelC[3], *reelD[3];
  double angle;
  int light1, light2, tape1, tape2;
  long counter;
  int reelAindex, reelCindex; 
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
	
	cairo_set_source_surface(cr, glob.image, 0, 0);
	cairo_paint(cr);
	
	if (glob.light1) {
		cairo_set_source_surface(cr, glob.light1on, LIGHT1X, LIGHT1Y);
	}
	else {
		cairo_set_source_surface(cr, glob.lightoff, LIGHT1X, LIGHT1Y);
	}
	cairo_paint(cr);
	
	if (glob.light2) {
		cairo_set_source_surface(cr, glob.light2on, LIGHT2X, LIGHT2Y);
	}
	else {
		cairo_set_source_surface(cr, glob.lightoff, LIGHT2X, LIGHT2Y);
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
	    
}

static void do_animation()
{
	cairo_t *reel;
	reel = cairo_create(glob.image);
	
	glob.counter++;
	
	if ((glob.counter % 32) == 0) {
		glob.light2 = !glob.light2;
	}
	
	glob.light1 = glob.light2;
	
	glob.tape1 = glob.light2;
	glob.tape2 = !glob.light2;
	
}

static gboolean on_timer_event(GtkWidget *widget)
{
	do_animation();
	
	gtk_widget_queue_draw(widget);
	return TRUE;
}

cairo_surface_t* readpng(char* s)
{
	cairo_surface_t *t = cairo_image_surface_create_from_png(s);
	if (cairo_surface_status(t)) {
		printf("Cannot load %s\n",s);
		exit(1);
	}
	return t;
}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *darea;
  
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
  
  glob.angle = 0.0;
  glob.light1 = 0;
  glob.light2 = 0;
  glob.tape1 = 0;
  glob.tape2 = 0;
  glob.counter = 0;
  glob.reelAindex = -1;
  glob.reelCindex = -1;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER (window), darea);

  g_signal_connect(G_OBJECT(darea), "draw", 
      G_CALLBACK(on_draw_event), NULL); 
  g_signal_connect(window, "destroy",
      G_CALLBACK (gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080); 
  gtk_window_set_title(GTK_WINDOW(window), "tu56");
  
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
