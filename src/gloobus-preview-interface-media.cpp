/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** gloobus-preview
**
** gloobus-preview-interface-media.cpp - interface for audio/video plugins
**
** Copyright (c) 2010 Gloobus Developers
**
** Authors:
**    Alexandr Grigorcea <cahr.gr@gmail.com>
**
** This program is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License version 3, as published
** by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranties of
** MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
** PURPOSE.  See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

#include "gloobus-preview-interface-media.h"

// ========================================================================== //

iMedia::iMedia( void )
	:playing(false)
	,pix_play(0)
	,pix_pause(0)
	,timeout_id(0)
	,f_visible(false)
	,duration(0)
	,proxy(0)
	,handler_id(0)
	,watch_id(0)
	,cancellable_init(0)
	,cancellable(0)
{
	g_debug("Creating iMedia");
	activate_media_keys(); //TODO: is it the best place to do it?
}

iMedia::~iMedia(void)
{
	g_debug("Destroying iMedia");
	deactivate_media_keys();
}

void iMedia::draw_toolbar( void )
{
	GtkWidget *toolbar = ui->get_toolbar();
	GloobusTheme *theme = ui->get_theme();
	int isize = theme->get_toolbar_icon_size();

	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	GtkWidget *btn_fullscreen = ui->create_button("fullscreen", isize);
	
	duration_label = theme->get_label("--:--");
	position_label = theme->get_label("--:--");

	if(!pix_play)
		pix_play = theme->get_icon("play", isize);
	if(!pix_pause)
		pix_pause = theme->get_icon("pause", isize);

	control_image = gtk_image_new_from_pixbuf(pix_play);
	GtkWidget * btn_control = gtk_event_box_new();
	gtk_event_box_set_visible_window( GTK_EVENT_BOX(btn_control) , false );
	gtk_container_add( GTK_CONTAINER(btn_control) , control_image );

	seekbar = gtk_scale_new_with_range( GTK_ORIENTATION_HORIZONTAL, 0, 1, 1 );
	gtk_scale_set_draw_value(GTK_SCALE(seekbar), false);

	gtk_box_pack_start(GTK_BOX(hbox), btn_control, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), position_label, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), seekbar, true, true, 0);
	gtk_box_pack_start(GTK_BOX(hbox), duration_label, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), btn_fullscreen, false, false, 0);

	g_signal_connect( seekbar, "change-value", G_CALLBACK(seekbar_change_cb), this);
	g_signal_connect( btn_control, "button-press-event", G_CALLBACK(button_play_pause), this);
	g_signal_connect( btn_fullscreen, "button-press-event", G_CALLBACK(fullscreen_cb), this);

	gtk_container_add(GTK_CONTAINER(toolbar), hbox);
	gtk_widget_show_all(toolbar);
}

void iMedia::draw_floating_toolbar( void )
{
	GtkWidget *lpad = gtk_alignment_new(0,0,0,0);
	gtk_widget_set_size_request(lpad, 20,-1);
	GtkWidget *rpad = gtk_alignment_new(0,0,0,0);
	gtk_widget_set_size_request(rpad, 20,-1);
	
	GtkWidget *toolbar = ui->get_floating_toolbar();
	GloobusTheme *theme = ui->get_theme();
	int isize = theme->get_toolbar_icon_size();

	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	GtkWidget *btn_fullscreen = ui->create_button("fullscreen", isize);

	f_duration_label = theme->get_label("--:--");
	f_position_label = theme->get_label("--:--");

	if(!pix_play)
		pix_play = theme->get_icon("play", isize);
	if(!pix_pause)
		pix_pause = theme->get_icon("pause", isize);

	if(playing)
		f_control_image = gtk_image_new_from_pixbuf(pix_pause);
	else
		f_control_image = gtk_image_new_from_pixbuf(pix_play);
	GtkWidget * btn_control = gtk_event_box_new();
	gtk_event_box_set_visible_window( GTK_EVENT_BOX(btn_control) , false );
	gtk_container_add( GTK_CONTAINER(btn_control) , f_control_image );

	f_seekbar = gtk_scale_new_with_range( GTK_ORIENTATION_HORIZONTAL, 0, 1, 1 );
	gtk_scale_set_draw_value(GTK_SCALE(f_seekbar), false);

	gtk_box_pack_start(GTK_BOX(hbox), lpad, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), btn_control, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), f_position_label, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), f_seekbar, true, true, 0);
	gtk_box_pack_start(GTK_BOX(hbox), f_duration_label, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), btn_fullscreen, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), rpad, false, false, 0);

	g_signal_connect( f_seekbar, "change-value", G_CALLBACK(seekbar_change_cb), this);
	g_signal_connect( btn_control, "button-press-event", G_CALLBACK(button_play_pause), this);
	g_signal_connect( btn_fullscreen, "button-press-event", G_CALLBACK(fullscreen_cb), this);
	g_signal_connect( toolbar, "unrealize", G_CALLBACK(f_unrealize_cb), this);

	gtk_container_add(GTK_CONTAINER(toolbar), hbox);
	gtk_widget_show_all(hbox);

	f_visible = true;
	update_duration(get_duration());
	update_position(get_position());
}

void iMedia::update_position( gint sec )
{
	if( seekbar ) {
		gtk_range_set_value(GTK_RANGE(seekbar), sec );
		if(f_visible)
			gtk_range_set_value(GTK_RANGE(f_seekbar), sec );
	}
		
	if( position_label ) {
		gchar *str = sec_to_str( sec );
		gtk_label_set_text(GTK_LABEL(position_label), str);
		if(f_visible)
			gtk_label_set_text(GTK_LABEL(f_position_label), str);
		g_free(str);
	}
}

void iMedia::update_duration( gint sec )
{
	duration = sec;
	if( duration_label ) {
		gchar *str = sec_to_str( sec );
		gtk_label_set_text(GTK_LABEL(duration_label), str);
		if(f_visible)
			gtk_label_set_text(GTK_LABEL(f_duration_label), str);
		g_free(str);
	}
	if(seekbar && sec > 0) {
		gtk_range_set_range(GTK_RANGE(seekbar), 0, sec);
		if(f_visible)
			gtk_range_set_range(GTK_RANGE(f_seekbar), 0, sec);
	}
}

gchar * iMedia::sec_to_str( gint sec )
{
	gchar *str;
	if( sec <= 0) {
		str = g_strdup("00:00");
	} else if( sec >= 3600) {
		int h = sec / 3600;
		sec = sec % 3600;
		int m = sec / 60;
		sec = sec % 60;
		str = g_strdup_printf("%02d:%02d:%02d", h, m, sec);
	} else {
		int m = sec / 60;
		sec = sec % 60;
		str = g_strdup_printf("%02d:%02d", m, sec);
	}
	return str;
}

void iMedia::set_playing( bool p )
{
	if( p == playing )
		return;

	playing = p;

	if(playing) {
		gtk_image_set_from_pixbuf(GTK_IMAGE(control_image), pix_pause);
        block_slideshow(true);     //Block Slideshow till finished
		if(f_visible)
			gtk_image_set_from_pixbuf(GTK_IMAGE(f_control_image), pix_pause);
	} else {
		gtk_image_set_from_pixbuf(GTK_IMAGE(control_image), pix_play);
        block_slideshow(false);
		if(f_visible)
			gtk_image_set_from_pixbuf(GTK_IMAGE(f_control_image), pix_play);
	}
}

void iMedia::start_timeout( void )
{
	if( !timeout_id )
		timeout_id = g_timeout_add(333, &timeout_cb, this);
}

void iMedia::stop_timeout( void )
{
	if(timeout_id) {
		g_source_remove(timeout_id);
		timeout_id = 0;
	}
}

void iMedia::button_play_pause(GtkWidget *widget,
	GdkEventButton *event,
	gpointer data)
{
	iMedia* media = (iMedia*)data;
	media->set_playing(media->play_pause());
}

gboolean iMedia::seekbar_change_cb(GtkRange * range,
	GtkScrollType scroll,
	gdouble value,
	gpointer data)
{
	if (value >= 0)
		((iMedia*)data)->set_position(value);
	return false;
}

gboolean iMedia::timeout_cb( gpointer data )
{
	iMedia *media = (iMedia*)data;
	media->update_position( media->get_position() );

    if(media->get_position() == media->get_duration())
    {
        media->block_slideshow(true); //Stop blocking the slideshow
    }

	if(!media->duration)
		media->update_duration(media->get_duration());
	return true;
}

void iMedia::f_unrealize_cb( GtkWidget *widget, gpointer data )
{
	((iMedia*)data)->f_visible = false;
}

static void on_media_player_key_pressed (gpointer     data,
					 const gchar *key)
{
	iMedia *media = (iMedia*)data;

	if (g_strcmp0 ("Play", key) == 0)
		media->play_pause();
	else if (g_strcmp0 ("Previous", key) == 0)
		media->previous();
	else if (g_strcmp0 ("Next", key) == 0)
		media->next();
	else if (g_strcmp0 ("Stop", key) == 0)
		media->stop();
}

static void grab_media_player_keys_cb (GDBusProxy                 *proxy,
				       GAsyncResult               *res,
				       gpointer                    data)
{
	iMedia *media = (iMedia*)data;
	GVariant *variant;
	GError *error = NULL;

	variant = g_dbus_proxy_call_finish (proxy, res, &error);
	media->cancellable = NULL;

	if (variant == NULL) {
		if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
			g_warning ("Failed to call \"GrabMediaPlayerKeys\": %s", error->message);
		g_error_free (error);
		return;
	}
	g_variant_unref (variant);
}

static void grab_media_player_keys (gpointer                    data)
{
	iMedia *media = (iMedia*)data;
	GCancellable *cancellable;

	if (media->proxy == NULL)
		return;

	/* Only allow one key grab operation to happen concurrently */
	if (media->cancellable) {
		g_cancellable_cancel (media->cancellable);
	}

	cancellable = g_cancellable_new ();
	media->cancellable = cancellable;

	g_dbus_proxy_call (media->proxy,
			   "GrabMediaPlayerKeys",
			   g_variant_new ("(su)", "Gloobus-Preview", 0),
			   G_DBUS_CALL_FLAGS_NONE,
			   -1, cancellable,
			   (GAsyncReadyCallback) grab_media_player_keys_cb,
			   data);

	/* GDBus keeps a reference throughout the async call */
	g_object_unref (cancellable);
}

static gboolean on_window_focus_in_event (GtkWidget                  *window,
					  GdkEventFocus              *event,
					  gpointer                    data)
{
	grab_media_player_keys (data);

	return FALSE;
}

static void key_pressed (GDBusProxy                 *proxy,
			 gchar                      *sender_name,
			 gchar                      *signal_name,
			 GVariant                   *parameters,
			 gpointer                    data)
{
	char *app, *cmd;

	if (g_strcmp0 (signal_name, "MediaPlayerKeyPressed") != 0)
		return;
	g_variant_get (parameters, "(ss)", &app, &cmd);
	if (g_strcmp0 (app, "Gloobus-Preview") == 0) {
		on_media_player_key_pressed (data, cmd);
	}
	g_free (app);
	g_free (cmd);
}

static void got_proxy_cb (GObject                    *source_object,
			  GAsyncResult               *res,
			  gpointer                    data)
{
	iMedia *media = (iMedia*)data;
	GError *error = NULL;

	media->proxy = g_dbus_proxy_new_for_bus_finish (res, &error);
	media->cancellable_init = NULL;

	if (media->proxy == NULL) {
		if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
			g_warning ("Failed to contact settings daemon: %s", error->message);
		g_error_free (error);
		return;
	}

	grab_media_player_keys (data);

	g_signal_connect (G_OBJECT (media->proxy), "g-signal",
			  G_CALLBACK (key_pressed), data);
}

static void name_appeared_cb (GDBusConnection            *connection,
			      const gchar                *name,
			      const gchar                *name_owner,
			      gpointer                    data)
{
	iMedia *media = (iMedia*)data;
	GCancellable *cancellable;

	cancellable = g_cancellable_new ();
	media->cancellable_init = cancellable;

	g_dbus_proxy_new_for_bus (G_BUS_TYPE_SESSION,
				  G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
				  NULL,
				  "org.gnome.SettingsDaemon",
				  "/org/gnome/SettingsDaemon/MediaKeys",
				  "org.gnome.SettingsDaemon.MediaKeys",
				  cancellable,
				  (GAsyncReadyCallback) got_proxy_cb,
				  data);

	/* GDBus keeps a reference throughout the async call */
	g_object_unref (cancellable);
}

static void name_vanished_cb (GDBusConnection            *connection,
			      const gchar                *name,
			      gpointer                    data)
{
	iMedia *media = (iMedia*)data;

	if (media->proxy != NULL) {
		g_object_unref (media->proxy);
		media->proxy = NULL;
	}

	if (media->cancellable) {
		g_cancellable_cancel (media->cancellable);
	}
}

void iMedia::activate_media_keys( void )
{
	watch_id = g_bus_watch_name (G_BUS_TYPE_SESSION,
				     "org.gnome.SettingsDaemon",
				     G_BUS_NAME_WATCHER_FLAGS_NONE,
				     (GBusNameAppearedCallback) name_appeared_cb,
				     (GBusNameVanishedCallback) name_vanished_cb,
				     this, NULL);

	handler_id = g_signal_connect (G_OBJECT (ui->get_window()), "focus-in-event",
				       G_CALLBACK (on_window_focus_in_event), this);
}

void iMedia::deactivate_media_keys( void )
{
	if (cancellable_init) {
		g_cancellable_cancel (cancellable_init);
	}

	if (cancellable) {
		g_cancellable_cancel (cancellable);
	}

	if (proxy != NULL) {
		g_object_unref (proxy);
		proxy = NULL;
	}

	if (handler_id != 0) {
		g_signal_handler_disconnect (G_OBJECT (ui->get_window()), handler_id);
		handler_id = 0;
	}

	if (watch_id != 0) {
		g_bus_unwatch_name (watch_id);
		watch_id = 0;
	}
}

void iMedia::next( void )
{
	g_idle_add(&next_file_cb, NULL);
}

void iMedia::previous( void )
{
	g_idle_add(&previous_file_cb, NULL);
}

gboolean iMedia::next_file_cb( gpointer )
{
	Gloobus::instance()->next_file();
	return false;
}

gboolean iMedia::previous_file_cb( gpointer )
{
	Gloobus::instance()->prev_file();
	return false;
}
