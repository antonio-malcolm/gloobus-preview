#ifndef _IDEFAULT_
#define _IDEFAULT_

#include <string>

#include "gloobus-preview-interface-main.h"

class iDefault : public interface {
	private:
		GtkWidget		*	thumbnail_image;
		GtkWidget		*	name_label;
		GtkWidget		*	filetype_label;
		GtkWidget		*	filesize_label;
		GtkWidget		*	modified_label;
		GtkWidget		*	accessed_label;
		GtkWidget		*	error_label;
		GtkWidget		*	info_box;
		int					resize_default_id;
	
	public:
					iDefault			( void );
					~iDefault			( void );
	virtual	bool	load				( void );
	virtual	void	end					( void );
	virtual	int		get_width			( void );
	virtual	int		get_height			( void );
	virtual	void	draw				( GtkContainer * container );
	virtual	void	draw_fullscreen		( GtkContainer * container );
	virtual	void	draw_unfullscreen	( GtkContainer * container );
   	virtual	bool		    key_press_event		(int ){ return false;}

	private:
			void	load_default_info	( void );
	static	void	resize_default_cb	( GtkWidget *widget, GtkAllocation *allocation , gpointer data );
};

class iError : public interface {
	private:
		GtkWidget	*	error_image;
		GtkWidget	*	error_label;
	
	public:
					iError			( void );
					~iError			( void );
	virtual	bool	load				( void );
	virtual	void	end					( void );
	virtual	int		get_width			( void );
	virtual	int		get_height			( void );
	virtual	void	draw				( GtkContainer * container );
	virtual	void	draw_fullscreen		( GtkContainer * container );
	virtual	void	draw_unfullscreen	( GtkContainer * container );
   	virtual	bool    key_press_event		( int ){ return false;}
};

#endif

/*
 * CODE FOR LOADING GNOME THUMBNAILS:

#include <libgnomeui/libgnomeui.h>

GnomeThumbnailFactory * fac = gnome_thumbnail_factory_new(GNOME_THUMBNAIL_SIZE_LARGE);
string path = "file:///" + m_filename;
GdkPixbuf *temp = gnome_thumbnail_factory_generate_thumbnail(fac,path.c_str(),m_filetype.c_str());

*/
