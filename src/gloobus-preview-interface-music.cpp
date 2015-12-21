#include "gloobus-preview-interface-music.h"
/*
// =========================== ~iMUSIC ======================================== //
iMusic::iMusic()
	:playing(false)
{
	g_debug("Creating iMusic");
}
iMusic::~iMusic()
{
	g_debug("Destroying iMusic");
	g_source_remove(timeout_id);
    g_object_unref(cover);
    g_object_unref(reflection);
    g_object_unref(pix_play);
    g_object_unref(pix_pause);
}

// =============================== LOAD ======================================= //
bool iMusic::load			( void )
{
	g_debug("Loading iMusic");
    GdkPixbuf* temp = get_pixbuf();
	pix_cover = ui->pixbuf_scale(temp,ICON_SIZE,ICON_SIZE, true);
	pix_reflection = ui->pixbuf_reflect(pix_cover);
	g_object_unref(temp);

	string path;
	path = (string)PACKAGE_DATA_PATH + "/media-play.svg";
	pix_play = gdk_pixbuf_new_from_file(path.c_str(), NULL);
	path = (string)PACKAGE_DATA_PATH + "/media-pause.svg";
	pix_pause = gdk_pixbuf_new_from_file(path.c_str(), NULL);
	return true;
}
// =============================== END ======================================= //
void iMusic::end			( void )
{
	g_debug("Ending iMusic");
	stop();
}

int	iMusic::get_width( void )
{
	return 450;
}
int iMusic::get_height( void )
{
	return ICON_SIZE*2+TOOLBAR_HEIGHT;
}

// ================================== DRAW  =================================== //
void iMusic::draw (GtkContainer * container)
{
	cover		= gtk_image_new_from_pixbuf (pix_cover);
	reflection	= gtk_image_new_from_pixbuf (pix_reflection);
	gtk_fixed_put(GTK_FIXED(container),cover, 25, 50);
	gtk_fixed_put(GTK_FIXED(container),reflection,25, 50 + ICON_SIZE);

	string text = get_song_name() + "\n\n" + get_song_artist() + "\n\n" + get_song_album();
	draw_properties ( container, text, 200 , 75 );
	
	draw_controls(container);
	
	playing = true;
	play();
	timeout_id = g_timeout_add (200, &iMusic::draw_position, this);

	gtk_widget_show_all( GTK_WIDGET(container) );
}

// ================================== DRAW CONTROLS =================================== //
void iMusic::draw_controls(GtkContainer * container)
{
		GdkColor color;
		gdk_color_parse ("white", &color);
		
		button			= gtk_image_new_from_pixbuf(pix_pause);
		button_box	= gtk_event_box_new();
		gtk_event_box_set_visible_window(GTK_EVENT_BOX(button_box),false);
		gtk_container_add(GTK_CONTAINER(button_box),button);

		gtk_fixed_put( GTK_FIXED(container),button_box, get_width()/2-16 , get_height()-80 );

		duration		= gtk_label_new("--:--");
		current_time	= gtk_label_new("00:00");
		gtk_widget_modify_fg (duration, GTK_STATE_NORMAL, &color);
		gtk_widget_modify_fg (current_time, GTK_STATE_NORMAL, &color);
		seekbar = gtk_hscale_new_with_range		(0, 100, 1);
		gtk_scale_set_draw_value				(GTK_SCALE(seekbar), false); //Do not draw the numbers

		hbox = gtk_hbox_new   	(false,3);
		gtk_widget_set_size_request( hbox, get_width(),  35);
		gtk_box_pack_start(GTK_BOX (hbox), current_time	, false, false, 2);
		gtk_box_pack_start(GTK_BOX (hbox), seekbar		, true, true, 2);
		gtk_box_pack_start(GTK_BOX (hbox), duration		, false, false, 2);
		gtk_fixed_put( GTK_FIXED(container),hbox, 0 , get_height() - 35 );
		gtk_widget_show_all( hbox );

		//When the seek bar is moved, change the position
		g_signal_connect (G_OBJECT (seekbar), "change-value",
			G_CALLBACK (&iMusic::seekbar_change_cb), this);
		g_signal_connect (G_OBJECT (seekbar), "button-press-event",
			G_CALLBACK (&iMusic::seekbar_press_cb), this);
		g_signal_connect (G_OBJECT (seekbar), "button-release-event",
			G_CALLBACK (&iMusic::seekbar_release_cb), this);
		g_signal_connect (G_OBJECT (button_box), "button-press-event",
			G_CALLBACK (&iMusic::button_play_pause), this);
}

//============================== PLAY PAUSE ========================= //
void iMusic::play_pause( void )
{
	g_debug("Play Pause clicked");
		
	if(playing) {
		pause();
		gtk_image_set_from_pixbuf(GTK_IMAGE(button) , pix_play );
		playing = false;
	} else {
		play();
		gtk_image_set_from_pixbuf(GTK_IMAGE(button) , pix_pause );
		playing = true;
	}
}

void iMusic::draw_fullscreen ( GtkContainer * container )
{
	int x = ( ui->get_screen_width()-get_width() ) / 2;
	int y = ( ui->get_screen_height()-get_height() ) / 2;
	gtk_fixed_move( GTK_FIXED(container),cover, x+25, y+50 );
	gtk_fixed_move( GTK_FIXED(container),reflection,x+25, y+50 + ICON_SIZE );
	gtk_fixed_move( GTK_FIXED(container),button_box, x+get_width()/2-16 , y+get_height()-80 );
	gtk_fixed_move( GTK_FIXED(container),hbox, x , y+get_height() - 35 );
	gtk_fixed_move( GTK_FIXED(container), m_info , x+200 , y+75 );
}

void iMusic::draw_unfullscreen ( GtkContainer * container )
{
	gtk_fixed_move( GTK_FIXED(container),cover, 25, 50 );
	gtk_fixed_move( GTK_FIXED(container),reflection,25, 50 + ICON_SIZE );
	gtk_fixed_move( GTK_FIXED(container),button_box, get_width()/2-16 , get_height()-80 );
	gtk_fixed_move( GTK_FIXED(container),hbox, 0 , get_height() - 35 );
	gtk_fixed_move( GTK_FIXED(container), m_info , 200 , 75 );
}

void iMusic::button_play_pause(GtkWidget *widget,
	GdkEventButton *event,
	gpointer data)
{
	((iMusic*)data)->play_pause();
};

gboolean iMusic::seekbar_press_cb(GtkWidget *widget,
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

gboolean iMusic::seekbar_release_cb(GtkWidget *widget,
	GdkEventButton *event,
	gpointer data)
{
	event->button = 2;
	return false;
};

gboolean iMusic::seekbar_change_cb(GtkRange * range,
	GtkScrollType scroll,
	gdouble value,
	gpointer data)
{
	((iMusic*)data)->seek(value);
	return false;
}

gboolean iMusic::draw_position( gpointer data )
{
	iMusic * music = (iMusic*)data;
	gtk_range_set_value(GTK_RANGE(music->seekbar) , music->get_position());
	gtk_label_set_text(GTK_LABEL(music->duration), music->get_duration().c_str());
	gtk_label_set_text(GTK_LABEL(music->current_time), music->get_current_time().c_str());
	return true;
}
*/
