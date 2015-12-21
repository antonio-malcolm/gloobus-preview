#ifndef _INTERFACE_
#define _INTERFACE_

#include <map>
#include <string>
#include <gtk/gtk.h>

//class interface;

#include "gloobus-preview-defines.h"
#include "gloobus-preview-singleton.h"
#include "config.h"

using namespace std;

class interface
{
	protected:
		GloobusUI	*	ui;
		string			m_filename;
		string			m_filetype;
		string			m_uri;
		GFile		*	m_gfile;
		GFileInfo	*	m_file_info;
		GError		*	m_error;
		GtkWidget	*	toolbar_info;
        bool            m_block_slideshow;

	public:
						interface				( void );
		virtual			~interface				( void );
		
		virtual int		get_width				( void )						= 0;
		virtual int		get_height				( void )						= 0;
		virtual bool	load					( void )						= 0;
		virtual void	end						( void )						= 0;
		virtual void	draw					( GtkContainer * container )	= 0;
		virtual void	draw_fullscreen			( GtkContainer * container ){};
		virtual void	draw_unfullscreen		( GtkContainer * container ){};
		virtual	void	draw_toolbar			( void );
		virtual void	draw_floating_toolbar	( void );
		virtual bool	key_press_event			( int ) = 0;
		virtual void	resize					( int , int ) {};

		
				bool	set_filename	 		( string filename );
				void	set_file_info	 		( void );
				void	set_toolbar_info		( const gchar *str );
				string	get_file_size_string	( string filename );
				double	get_file_size	 		( string filename );
				string	get_file_info_string 	( void );
				string	get_safe_type	 		( bool replace_slash );//Mime Type
                bool    is_slideshow_blocked    ( void );
                void    block_slideshow         ( bool );

				GError*	get_error				( void );
				void	set_error				( GError* );
	static	gboolean	fullscreen_cb			( GtkWidget*, GdkEventButton*, gpointer);
	static	gboolean	defaultapp_cb			( GtkWidget*, GdkEventButton*, gpointer);
    


	static bool is_program_available( const char * );
	static void resize_cb( GtkWidget* , GtkRequisition* , gpointer );
	/* **************************************************** */

		void show( void );
};

#endif
