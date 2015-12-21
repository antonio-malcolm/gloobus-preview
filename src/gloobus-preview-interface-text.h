#ifndef _ITEXT_
#define _ITEXT_

#include <gtksourceview/gtksource.h>
//sudo ln -s /usr/include/gtksourceview-2.0/gtksourceview/ /usr/include/gtksourceview

#include "gloobus-preview-interface-main.h"

using namespace std;

class iText : public interface
{
	protected:
		GtkTextBuffer			*	m_textbuf;
		GtkSourceBuffer			*	m_sBuf;
		GtkWidget				*	m_scwin;
		GtkAdjustment       	*	m_vadj;
		PangoFontDescription	*	font;
		GtkWidget				*	view;
	
	public:
								iText				( void );
								~iText				( void );
	virtual	bool				load				( void );
	virtual	int 				get_width			( void );
	virtual	int 				get_height			( void );
	virtual	void				end					( void );
	virtual	void				draw				( GtkContainer * container );
    virtual	bool		        key_press_event		( int );

			void				draw_file_text		( GtkContainer * container); 
	virtual GtkSourceBuffer*	get_textbuf 		( void ) = 0;
};

#endif
