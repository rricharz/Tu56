// tu56.c

#include <cairo.h>
#include <math.h>
#include <gtk/gtk.h>

#define TIME_INTERVAL    40		// timer interval in msec

#define LIGHT1X         130
#define LIGHT1Y			 83
#define LIGHT2X         814
#define LIGHT2Y			 77
#define REEL1X			 54
#define REEL1Y			524
#define REEL2X			520
#define REEL2Y			524


struct {
  cairo_surface_t *image;
  cairo_surface_t *light1on, *lightoff, *light2on, *reel[4];
  double angle;
  int light1, light2;
  long counter;
  int index; 
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
	
	cairo_set_source_surface(cr, glob.reel[glob.index], REEL1X, REEL1Y);
	cairo_paint(cr);
	cairo_set_source_surface(cr, glob.reel[glob.index], REEL2X, REEL2Y);
	cairo_paint(cr);
	    
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
	
	if (glob.light2) {
		glob.index = 1 + (glob.counter % 2);
	}
	else {
		glob.index = 0;
	}
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
	if (t == 0) {
		printf("Cannot find %s\n",s);
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
  glob.reel[0]   = readpng("reelA0.png");
  glob.reel[1]   = readpng("reelA1.png");
  glob.reel[2]   = readpng("reelA2.png");
  glob.reel[3]   = readpng("reelA3.png");
  
  glob.angle = 0.0;
  glob.light1 = 0;
  glob.light2 = 0;
  glob.counter = 0;

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
