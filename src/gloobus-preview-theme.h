#ifndef _GLOOBUS_THEME_
#define _GLOOBUS_THEME_

#include <cairo.h>
#include <gtk/gtk.h>

#include "gloobus-preview-config.h"
#include "gloobus-preview-defines.h"

class GloobusThemeEngine;
class GloobusUI;
class GloobusContainerSize;

class GloobusTheme {
	friend class GloobusUI;
	private:
		GloobusConfig			*	config;
		GloobusThemeEngine		*	engine;
		bool						use_gtk;
		bool						loaded;
		char					*	path;
		GHashTable				*	icons;
		GdkColor				*	error_color;
		GdkColor				*	header_color;
		GdkColor				*	text_color;
		PangoFontDescription	*	error_font;
		PangoFontDescription	*	header_font;
		PangoFontDescription	*	text_font;
		GloobusContainerSize	*	container;
		GloobusContainerSize	*	header;
		GloobusContainerSize	*	toolbar;
		int							toolbar_icon_size;
		
	public:
						GloobusTheme			( void );
						~GloobusTheme			( void );
			void		load					( void );
			//GtkWidget*	get_button				( const gchar* name , int size=0 );
			GdkPixbuf*	get_icon				( const gchar *name, int size );
			GtkWidget*	get_label				( const gchar *text, int size=0, int max_width=0 );
			GtkWidget*	get_header_label		( const gchar *text );
			GtkWidget*	get_error_label			( const gchar *text, int size=0, int max_width=0 );
			GtkWidget*	get_container_align		( void );
			GtkWidget*	get_header_align		( void );
			GtkWidget*	get_toolbar_align		( void );
			int			get_header_icon_size	( void );
			int			get_toolbar_icon_size	( void );
			int			get_header_size			( void );
			int			get_toolbar_size		( void );
			void		draw_background			( cairo_t *cr, int width, int height);
			void		calc_add_sizes			( int *w, int *h);
			GdkColor*	get_text_color			( void );
			GdkColor*	get_error_color			( void );

	private:
			void		clean					( void );
			void		load_default			( void	);
			GdkPixbuf*	get_gtk_icon			( const gchar *name , int size );
	static	void		theme_icon_destroy		( gpointer );
	static	void		xml_start_element		( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer user_data, GError **error );
	static	void		xml_end_element			( GMarkupParseContext *context, const gchar *element_name, gpointer user_data, GError **error );
	static	void		xml_error				( GMarkupParseContext *context, GError *error, gpointer user_data );
			void		parse_header_attributes	( const gchar **attribute_names, const gchar **attribute_values );
			void		parse_icon_attributes	( const gchar **attribute_names, const gchar **attribute_values );
			void		parse_text_attributes	( const gchar **attribute_names, const gchar **attribute_values , PangoFontDescription *font, GdkColor *color);
			void		parse_toolbar_attributes( const gchar **attribute_names, const gchar **attribute_values );
			void		parse_settings_attributes( const gchar **attribute_names, const gchar **attribute_values );

};

#endif
