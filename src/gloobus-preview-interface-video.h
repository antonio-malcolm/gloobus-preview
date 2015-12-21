#ifndef _IMOVIE_
#define _IMOVIE_

//#include <string>
#include <vector>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include "gloobus-preview-interface-main.h"
/*
using namespace std;

class iMovie : public interface
{
	private:
		bool				playing;
		GdkPixbuf 		*	pix_play;
		GdkPixbuf 		*	pix_pause;
		GdkPixbuf 		*	pix_fullscreen;
		GtkWidget		*	btn_control;
		GtkWidget		*	btn_fullscreen;
		GtkWidget		*	btn_control_box;
		GtkWidget		*	btn_fullscreen_box;
		GtkWidget		*	hbox;

		GtkWidget 		*	duration_label;
		GtkWidget		*	current_time_label;
		GtkWidget 		*	seekbar;

	protected:
	 	GtkWidget		*	m_drawable;
	 	GtkWidget		*	m_container;
		int 				m_timeout_id;

	public:
						iMovie				( void );
						~iMovie				( void );
								
	virtual	bool		load				( void );
	virtual	int			get_width			( void );
	virtual	int			get_height			( void );
	virtual	void		draw				( GtkContainer * container );
	virtual	void		draw_toolbar		( GtkWidget * container);
	virtual	void		end					( void );
	virtual	bool		can_fullscreen		( void );						//Returns true if the plugin can do fullscreen
	virtual	void		draw_fullscreen 	( GtkContainer * container );
	virtual	void		draw_unfullscreen 	( GtkContainer * container );

	virtual	string		get_duration		( void ) = 0;	 //Returns the string correctly formed
	virtual	string		get_current_time	( void ) = 0;
		
	//================= PLAYER FUNCTIONS =================== //
	virtual	void		play 				( void ) = 0;
	virtual void		pause 				( void ) = 0;
	virtual	void		stop 				( void ) = 0;
	virtual	double		get_position		( void ) = 0;
	virtual	void		seek				( gdouble value )   = 0;

			void		draw_movie			( GtkContainer * container);
			void		draw_controls		( GtkContainer * container);
			void		hide_controls		( void );
			void		show_controls 		( void );
			void		create_timeout		( void );
			void		play_pause			( void );

	static	void		button_play_pause	(GtkWidget *widget, GdkEventButton *event, gpointer);
	static	gboolean	seekbar_press_cb	(GtkWidget *widget, GdkEventButton *event, gpointer);
	static	gboolean	seekbar_release_cb	(GtkWidget *widget, GdkEventButton *event, gpointer);
	static	gboolean	seekbar_change_cb	(GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
	static	gboolean	draw_position		( gpointer data );
};
*/
#endif
