#include "gloobus-preview-interface-video.h"
/*
//TODO: do gstreamer has some signals? better using signals insted of timeout

// @param obj This is the pointer to the class that will call the function load2
//
// =========================== LAUNCH MEMBER FUNCTION ======================================== //
// Trick to launch a the play function, after the widget is created <g_idle_add> 
static gboolean LaunchMemberFunction(void *obj)
{
    iMovie *myObj = reinterpret_cast<iMovie *>(obj);
    myObj->play();
    myObj->create_timeout();
    return FALSE;
};

iMovie::iMovie()
	:playing(false)
{
	g_debug("Creating iMovie");
}

iMovie::~iMovie()
{
	g_debug("Destroying iMovie");
	g_source_remove(m_timeout_id);
}

void iMovie::end()
{
	g_debug("Ending iMovie");
	stop();
}

bool iMovie::load()
{
	g_debug("Loading iMovie");

	string path;
	path = (string)PACKAGE_DATA_PATH + "/media-play.svg";
	pix_play = gdk_pixbuf_new_from_file(path.c_str(), NULL);
	path = (string)PACKAGE_DATA_PATH + "/media-pause.svg";
	pix_pause = gdk_pixbuf_new_from_file(path.c_str(), NULL);
	path = (string)PACKAGE_DATA_PATH + "/view-fullscreen.svg";
	pix_fullscreen = gdk_pixbuf_new_from_file(path.c_str(), NULL);

	return true;
}

int  iMovie::get_width	()
{
	return 720;
}

int  iMovie::get_height	(){
	return 400 + TOOLBAR_HEIGHT;
}

void iMovie::draw ( GtkContainer * container )
{
	draw_movie(container);
	draw_controls(container);
	playing = true;
}

// ======================= DRAW MOVIE ================================== //
void iMovie::draw_movie(GtkContainer * container)
{
	GdkColormap *colormap;   			// in this Main Function
	GdkScreen 	*screen;
	screen 	 = gdk_screen_get_default 	   (); 
	colormap = gdk_screen_get_rgb_colormap(screen);
	gtk_widget_set_default_colormap	( colormap ); 
	
	gtk_widget_push_colormap(colormap);
	m_container = GTK_WIDGET(container);
	
	m_drawable = gtk_drawing_area_new();
	gtk_widget_set_size_request (m_drawable, get_width(), get_height()-TOOLBAR_HEIGHT);

	gtk_fixed_put(GTK_FIXED(m_container),m_drawable,0,0);
	gtk_widget_show (m_drawable);

	//Play when the widgets are created
	g_idle_add(LaunchMemberFunction,this);		//Wait till the windows are created and displayed before
												//Do the movie sync									
	gtk_widget_pop_colormap();
	
	//draw_properties ( container, get_file_info_string(), 25 + SHADOW_WIDTH, get_height() + SHADOW_WIDTH + HEADER_HEIGHT - 5); //35 = seek bar
}

// ================================== DRAW CONTROLS =================================== //
void iMovie::draw_controls(GtkContainer * container)
{
	GdkColor color;
	gdk_color_parse ("white", &color);

	btn_control			= gtk_image_new_from_pixbuf(pix_pause);
	btn_fullscreen		= gtk_image_new_from_pixbuf(pix_fullscreen);
	btn_control_box		= gtk_event_box_new();
	btn_fullscreen_box	= gtk_event_box_new();
	gtk_event_box_set_visible_window(GTK_EVENT_BOX(btn_control_box),false);
	gtk_event_box_set_visible_window(GTK_EVENT_BOX(btn_fullscreen_box),false);
	gtk_container_add(GTK_CONTAINER(btn_control_box), btn_control);
	gtk_container_add(GTK_CONTAINER(btn_fullscreen_box), btn_fullscreen);

	gtk_fixed_put( GTK_FIXED(container),btn_control_box, get_width()/2-32 , get_height()-64 );
	gtk_widget_show_all( btn_control_box );
	gtk_fixed_put( GTK_FIXED(container),btn_fullscreen_box, get_width()/2 , get_height()-64 );
	gtk_widget_show_all( btn_fullscreen_box );

	duration_label		= gtk_label_new("--:--");
	current_time_label	= gtk_label_new("00:00");
	gtk_widget_modify_fg( duration_label	, GTK_STATE_NORMAL, &color);
	gtk_widget_modify_fg( current_time_label, GTK_STATE_NORMAL, &color);
	seekbar = gtk_hscale_new_with_range( 0, 100, 1 );
	gtk_scale_set_draw_value(GTK_SCALE(seekbar), false);	 //Do not draw the numbers

	hbox = gtk_hbox_new   	(false,3);
	gtk_widget_set_size_request( hbox, get_width(),  32);
	gtk_box_pack_start(GTK_BOX (hbox), current_time_label	, false, false, 2);
	gtk_box_pack_start(GTK_BOX (hbox), seekbar				, true, true, 2);
	gtk_box_pack_start(GTK_BOX (hbox), duration_label		, false, false, 2);
	gtk_fixed_put( GTK_FIXED(container),hbox, 0 , get_height() - 32 );
	gtk_widget_show_all( hbox );

		//When the seek bar is moved, change the position
		g_signal_connect (G_OBJECT (seekbar), "change-value",
			G_CALLBACK (&iMovie::seekbar_change_cb), this);
		g_signal_connect (G_OBJECT (seekbar), "button-press-event",
			G_CALLBACK (&iMovie::seekbar_press_cb), this);
		g_signal_connect (G_OBJECT (seekbar), "button-release-event",
			G_CALLBACK (&iMovie::seekbar_release_cb), this);
		g_signal_connect (G_OBJECT (btn_control_box), "button-press-event",
			G_CALLBACK (&iMovie::button_play_pause), this);
		g_signal_connect (G_OBJECT (btn_fullscreen_box), "button-press-event",
			G_CALLBACK (&GloobusUI::fullscreen_cb), this);
}

// ============================ DRAW FLOATING CONTROLS ================================ //
void iMovie::draw_toolbar	( GtkWidget * container )
{
    g_debug("iMovie toolbar buttons\n");

	//Fullscreen button
	string fullbuttonstr			= 	(string)PACKAGE_DATA_PATH + "/view-restore.svg";
	GtkWidget* fullscreen_button	= 	gtk_image_new_from_file(fullbuttonstr.c_str());
	GtkWidget* fullscreen_box		= 	gtk_event_box_new();
							  			gtk_event_box_set_visible_window(GTK_EVENT_BOX(fullscreen_box),false);
							  			gtk_container_add(GTK_CONTAINER(fullscreen_box),fullscreen_button);
	
	//Launch Default App buton
	string defaultappstr			= 	(string)PACKAGE_DATA_PATH + "/media-pause.svg";
	m_play_pause_button_toolbar	    = 	gtk_image_new_from_file(defaultappstr.c_str());
	GtkWidget* defaultapp_box		= 	gtk_event_box_new();
							  			gtk_event_box_set_visible_window(GTK_EVENT_BOX(defaultapp_box),false);
							  			gtk_container_add(GTK_CONTAINER(defaultapp_box),m_play_pause_button_toolbar);

	GtkWidget *vbox		= gtk_vbox_new	  (false, 3);
	GtkWidget *hbox 	= gtk_hbox_new    (false, 3);
	
	gtk_box_pack_start 		( GTK_BOX (hbox),   gtk_label_new(""), 		true, 	true, 	2);
	gtk_box_pack_start 		( GTK_BOX (hbox),   defaultapp_box	, 		false, 	false, 	2);
	gtk_box_pack_start 		( GTK_BOX (hbox),   fullscreen_box	, 		false, 	false, 	2);
	gtk_box_pack_start 		( GTK_BOX (hbox),   gtk_label_new(""), 		true, 	true, 	2);	

	gtk_box_pack_start 		( GTK_BOX (vbox),   gtk_label_new(""), 		false, 	false, 	5);
	gtk_box_pack_start 		( GTK_BOX (vbox),   hbox, 					false, 	false, 	5);	


	g_signal_connect( G_OBJECT(fullscreen_box), "button-press-event",
		G_CALLBACK (&iMovie::fullscreen_trick), this);
	g_signal_connect( G_OBJECT(defaultapp_box), "button-press-event",
		G_CALLBACK (&iMovie::button_play_pause), this);

	//gtk_box_pack_start ( GTK_BOX(container), hbox, false,false,2);
	gtk_container_add ( GTK_CONTAINER(container), vbox);

	gtk_widget_show_all(container);
   
}

// ================================== HIDE CONTROLS =================================== //
void iMovie::hide_controls		( void )
{
	gtk_widget_hide(hbox);
}
// ================================== SHOW CONTROLS =================================== //
void iMovie::show_controls 		( void )
{
	gtk_widget_show_all(hbox);
}

// =========================== CREATE TIMEOUT =============================== //
void iMovie::create_timeout()
{
	 m_timeout_id = g_timeout_add(200, &iMovie::draw_position, this);	
}
//============================== PLAY PAUSE ========================= //
void iMovie::play_pause( void )
{
	g_debug("Play Pause clicked");
		
	if(playing) {
		pause();
		gtk_image_set_from_pixbuf(GTK_IMAGE(btn_control) , pix_play );
		playing = false;
	} else {
		play();
		gtk_image_set_from_pixbuf(GTK_IMAGE(btn_control) , pix_pause );
		playing = true;
	}

}

void iMovie::button_play_pause(GtkWidget *widget,
	GdkEventButton *event,
	gpointer data)
{
	((iMovie*)data)->play_pause();
};

gboolean iMovie::seekbar_press_cb(GtkWidget *widget,
	GdkEventButton *event,
	gpointer data)
{
	/* HACK: we want the behaviour you get with the middle button, so we
	 * mangle the event.  clicking with other buttons moves the slider in
	 * step increments, clicking with the middle button moves the slider to
	 * the location of the click.
	 * thanks to totem devs :D
	 *//*
	event->button = 2;
	return false;
};

gboolean iMovie::seekbar_release_cb(GtkWidget *widget,
	GdkEventButton *event,
	gpointer data)
{
	event->button = 2;
	return false;
};

gboolean iMovie::seekbar_change_cb(GtkRange * range,
	GtkScrollType scroll,
	gdouble value,
	gpointer data)
{
	((iMovie*)data)->seek(value);
	return false;
}

gboolean iMovie::draw_position( gpointer data )
{
	iMovie * movie = (iMovie*)data;
	gtk_range_set_value(GTK_RANGE(movie->seekbar), movie->get_position());
	gtk_label_set_text(GTK_LABEL(movie->duration_label), movie->get_duration().c_str());
	gtk_label_set_text(GTK_LABEL(movie->current_time_label), movie->get_current_time().c_str());
	return true;
}
*/
