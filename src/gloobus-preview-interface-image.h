#ifndef _IIMAGE_
#define _IIMAGE_

#include "gloobus-preview-interface-main.h"

using namespace std;

class iImage : public interface
{
	protected:
		GdkPixbuf 			* m_pixbuf;
		GdkPixbuf 			* m_pixbufFull;
        GdkPixbufAnimation  * m_animation;
		GtkWidget 			* m_image;
		GtkWidget 			* m_image_container;
		GtkWidget			* m_image_info;

	public:
							iImage				( void );
							~iImage				( void );
		virtual GdkPixbuf*	get_pixbuf			( void ) = 0;
		
		virtual bool		load				( void );
		virtual int 		get_width			( void );
		virtual int 		get_height			( void );
		virtual void		draw				( GtkContainer * container);
		virtual void		end					( void );
		virtual void		draw_fullscreen 	( GtkContainer * container);
		virtual void		draw_unfullscreen 	( GtkContainer * container);
    	virtual	bool		key_press_event		(int ){ return false;}
};

#endif
