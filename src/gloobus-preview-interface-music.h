#ifndef _IMUSIC_
#define _IMUSIC_

#include <string>

#include "gloobus-preview-interface-main.h"
/*
using namespace std;

class iMusic : public interface
{
	private:
		GdkPixbuf 		*	pix_cover;
		GdkPixbuf		*	pix_reflection;
		GdkPixbuf 		*	pix_play;
		GdkPixbuf 		*	pix_pause;
		GtkWidget		*	cover;
		GtkWidget		*	reflection;
		int					timeout_id;
		bool 				playing;
		GtkWidget		*	button;
		GtkWidget		*	button_box;
		GtkWidget		*	hbox;

		GtkWidget 		*	duration;
		GtkWidget 		*	current_time;
		GtkWidget		*	seekbar;

	public:
							iMusic				( void );
							~iMusic				( void );

		//================ INTERFACE FUNCTIONS ================= //
		virtual bool		load				( void );
		virtual void		end					( void );
		virtual int			get_height			( void ); 
		virtual int			get_width			( void );
		virtual void		draw_fullscreen 	( GtkContainer * container );
		virtual void		draw_unfullscreen 	( GtkContainer * container );

		// =============== ===================== //
		virtual GdkPixbuf*	get_pixbuf  		( void ) = 0;
		virtual string		get_song_artist		( void ) = 0;
		virtual string		get_song_name		( void ) = 0;
		virtual string		get_song_album  	( void ) = 0;
		virtual string		get_duration		( void ) = 0;	 //Returns the string correctly formed
		virtual string		get_current_time	( void ) = 0;
		
		//================= PLAYER FUNCTIONS =================== //
		virtual void		play 				( void ) = 0;
		virtual void		pause 				( void ) = 0;
		virtual void		stop 				( void ) = 0;
		virtual double		get_position		( void ) = 0;
		virtual void		seek				( gdouble value )   = 0;


				void		draw 				(GtkContainer * container);
				void		draw_controls		(GtkContainer * container);
				void		play_pause			( void );

		static void button_play_pause(GtkWidget *widget, GdkEventButton *event, gpointer);
		static gboolean seekbar_press_cb(GtkWidget *widget, GdkEventButton *event, gpointer);
		static gboolean seekbar_release_cb(GtkWidget *widget, GdkEventButton *event, gpointer);
		static gboolean seekbar_change_cb(GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
		static gboolean draw_position( gpointer data );

};
*/
#endif
