/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** gloobus-preview
**
** gloobus-preview-interface-media.h - interface for audio/video plugins
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

#ifndef _GLOOBUS_PREVIEW_INTERFACE_MEDIA_H_
#define _GLOOBUS_PREVIEW_INTERFACE_MEDIA_H_

#include "gloobus-preview-interface-main.h"

class iMedia : public interface
{
	protected:
		bool				playing;
		GdkPixbuf 		*	pix_play;
		GdkPixbuf 		*	pix_pause;
		GtkWidget		*	control_image;
		GtkWidget 		*	duration_label;
		GtkWidget		*	position_label;
		GtkWidget 		*	seekbar;
		guint				timeout_id;
		GtkWidget		*	f_control_image;
		GtkWidget 		*	f_duration_label;
		GtkWidget		*	f_position_label;
		GtkWidget 		*	f_seekbar;
		bool				f_visible;
		gint				duration;

	public:
		GDBusProxy 		*	proxy;
		guint				handler_id;
		guint				watch_id;
		GCancellable		*	cancellable_init;
		GCancellable		*	cancellable;

	public:
							iMedia					( void );
		virtual				~iMedia					( void );
		virtual	void		draw_toolbar			( void );
		virtual void		draw_floating_toolbar	( void );
    	virtual	bool		key_press_event		    (int ){ return false;}
	static	void			button_play_pause		( GtkWidget*, GdkEventButton*, gpointer );
	static	gboolean		seekbar_press_cb		( GtkWidget*, GdkEventButton*, gpointer );
	static	gboolean		seekbar_release_cb		( GtkWidget*, GdkEventButton*, gpointer );
	static	gboolean		seekbar_change_cb		( GtkRange*, GtkScrollType, gdouble, gpointer );
				void		set_playing				( bool p );
				void		start_timeout			( void );
				void		stop_timeout			( void );
	static	gboolean		timeout_cb				( gpointer data );
	static	void			f_unrealize_cb			( GtkWidget*, gpointer );
			gchar*			sec_to_str				( gint sec );

		virtual	bool		play_pause				( void )		= 0;
		virtual	void		stop					( void )		= 0;
		virtual	void		previous				( void );
		virtual	void		next					( void );
		virtual	void		set_position			( gint sec )	= 0;
		virtual	gint		get_position			( void )		= 0;
		virtual	gint		get_duration			( void )		= 0;
		virtual	void		update_position			( gint sec );
		virtual	void		update_duration			( gint sec );
				void		activate_media_keys		( void );
				void		deactivate_media_keys	( void );
		static gboolean	    next_file_cb			( gpointer );
		static gboolean	    previous_file_cb		( gpointer );
};

#endif
