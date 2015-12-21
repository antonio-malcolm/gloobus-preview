#ifndef _ILIST_
#define _ILIST_

#include <string>

#include "gloobus-preview-interface-main.h"

using namespace std;

class iList : public interface
{
	protected:
		GdkPixbuf		*	pix_icon;
		GdkPixbuf		*	pix_reflection;
		GtkWidget		*	icon;
		GtkWidget		*	reflection;
		int					n_items;
		GtkListStore	*	m_store;
		GtkWidget		*	m_scwin;
		GtkWidget		*	m_tree_view;
		guint				loader_id;
		gpointer		*	self_ptr;

	public:
								iList();
								~iList();
		virtual GdkPixbuf*		get_pixbuf			( void ) = 0;
		virtual GtkListStore*	get_list_store		( void ) = 0;
		virtual int 			get_n_columns		( void ) = 0;
		virtual string			get_column_title	( int i) = 0;
		virtual int 			get_n_items			( void ) = 0;
		virtual string 			get_item			( int i, int j) = 0;
		virtual GdkPixbuf*		get_item_pixbuf		( int i, int j) = 0;
		virtual bool			is_column_pixbuf	( int i)	= 0;
		virtual bool			has_items			( void ) = 0;
    	virtual	bool		    key_press_event		( int ){ return false;}

		//Virtual Functions implemented from interface
		virtual bool			load				( void );
		virtual int 			get_width			( void );
		virtual int 			get_height			( void );
		virtual	void			draw				( GtkContainer * container );
		virtual void			end					( void );
		virtual void			draw_fullscreen 	( GtkContainer * container );
		virtual void			draw_unfullscreen	( GtkContainer * container );
		static	gboolean		file_loader			( gpointer );
};

#endif
