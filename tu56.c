// tu56.c

#include <cairo.h>
#include <math.h>
#include <gtk/gtk.h>

#define TIME_INTERVAL    40		// timer interval in msec

#define LIGHT1X         130
#define LIGHT1Y			 83
#define REEL1X			 54
#define REEL1Y			524
#define REEL2X			520
#define REEL2Y			524


struct {
  cairo_surface_t *image;
  cairo_surface_t *light1on, *light1off, *reel[4];
  double angle;
  int light1;
  long counter; 
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
		index = 1 + (glob.counter % 2);
	}
	else {
		cairo_set_source_surface(cr, glob.light1off, LIGHT1X, LIGHT1Y);
		index = 0;
	}
	cairo_paint(cr);
	cairo_set_source_surface(cr, glob.reel[index], REEL1X, REEL1Y);
	cairo_paint(cr);
	cairo_set_source_surface(cr, glob.reel[index], REEL2X, REEL2Y);
	cairo_paint(cr);
	    
}

static void do_animation()
{
	cairo_t *reel;
	reel = cairo_create(glob.image);
	
	glob.counter++;
	
	if ((glob.counter % 32) == 0)
		glob.light1 = !glob.light1;
}

static gboolean on_timer_event(GtkWidget *widget)
{
	do_animation();
	
	gtk_widget_queue_draw(widget);
	return TRUE;
}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *darea;
  
  glob.image = cairo_image_surface_create_from_png("front2.png");
  glob.light1on = cairo_image_surface_create_from_png("light1on.png");
  glob.light1off = cairo_image_surface_create_from_png("light1off.png");
  glob.reel[0] = cairo_image_surface_create_from_png("reelA0.png");
  glob.reel[1] = cairo_image_surface_create_from_png("reelA1.png");
  glob.reel[2] = cairo_image_surface_create_from_png("reelA2.png");
  glob.reel[3] = cairo_image_surface_create_from_png("reelA3.png");
  
  glob.angle = 0.0;
  glob.light1 = 0;
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
