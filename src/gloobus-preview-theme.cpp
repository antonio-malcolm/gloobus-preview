#include "gloobus-preview-theme.h"
#include "gloobus-preview-singleton.h"

class GloobusIconInfo {
	public:
		char		*path;
		GdkPixbuf	*icon;
		GloobusIconInfo(gchar *p):path(p), icon(NULL){}
		GloobusIconInfo(GdkPixbuf *i):path(NULL), icon(i){}
		~GloobusIconInfo(void){
			if(path) g_free(path);
			if(icon) g_object_unref(icon);
		}
};

// ========================================================================== //

class GloobusColor {
	public:
		double red;
		double green;
		double blue;
		double alpha;
		GloobusColor(void):red(0.0), green(0.0), blue(0.0), alpha(1.0) {}
		void set( double r, double g, double b, double a);
		bool parse_color( const gchar * str);
};

void GloobusColor::set( double r, double g, double b, double a) {
	red = r;
	green = g;
	blue = b;
	alpha = a;
}

bool GloobusColor::parse_color( const gchar * str) {
	if(!str || !str[0] || str[0] != '#') {
		g_warning("1");
		return false;
	}
	int len = 1;
	for( ; str[len] ; len++ ){
		if( !g_ascii_isxdigit(str[len]) ) {
			g_warning("2");
			return false;
		}
	}
	int r,g,b,a;
	switch(len) {
		case 4:
			r = g_ascii_xdigit_value(str[1]) * 16 + g_ascii_xdigit_value(str[1]);
			g = g_ascii_xdigit_value(str[2]) * 16 + g_ascii_xdigit_value(str[2]);
			b = g_ascii_xdigit_value(str[3]) * 16 + g_ascii_xdigit_value(str[3]);
			a = 255;
			break;
		case 7:
			r = g_ascii_xdigit_value(str[1]) * 16 + g_ascii_xdigit_value(str[2]);
			g = g_ascii_xdigit_value(str[3]) * 16 + g_ascii_xdigit_value(str[4]);
			b = g_ascii_xdigit_value(str[5]) * 16 + g_ascii_xdigit_value(str[6]);
			a = 255;
			break;
		case 9:
			r = g_ascii_xdigit_value(str[1]) * 16 + g_ascii_xdigit_value(str[2]);
			g = g_ascii_xdigit_value(str[3]) * 16 + g_ascii_xdigit_value(str[4]);
			b = g_ascii_xdigit_value(str[5]) * 16 + g_ascii_xdigit_value(str[6]);
			a = g_ascii_xdigit_value(str[7]) * 16 + g_ascii_xdigit_value(str[8]);
			break;
		default:
			g_warning("3");
			return false;
			break;
	}
	red = (double)r / 255;
	green = (double)g / 255;
	blue = (double)b / 255;
	alpha = (double)a / 255;
	return true;
}

class GloobusContainerSize {
	public:
		int size;
		int top;
		int bottom;
		int left;
		int right;
		GloobusContainerSize(void): size(0), top(0), bottom(0), left(0), right(0){}
		bool parse(const gchar *str);
};

bool GloobusContainerSize::parse(const gchar *str)
{
	int s[4] = {0,0,0,0};
	int j = 0;
	int l = 0;
	char val;
	for( ; str[l] ; l++ ) {
		val = str[l];
		if(val == ',') {
			j++;
		} else {
			if( !g_ascii_isdigit(val)) {
				break;
			} else {
				s[j] = s[j]*10 + g_ascii_digit_value(val);
			}
		}
	}
	if(j < 3) {
		//g_warning(_("Bad padding setting"));
		return false;
	} else {
		top = s[0];
		right = s[1];
		bottom = s[2];
		left = s[3];
	}
	return true;
}

// ========================================================================== //

class GloobusThemeEngine {
	protected:
		GloobusConfig	*	config;
		int					shadow_radius;
		double				shadow_alpha;
	
	public:
				GloobusThemeEngine			( GloobusConfig *cfg );
	virtual	~GloobusThemeEngine			( void ) {}
				void	set_shadow			( int radius , double alpha );
	virtual	void	draw_background		( cairo_t *cr , int w , int h )=0;
	virtual	bool	parse_attributes	( const gchar **attribute_names, const gchar **attribute_values )=0;
};

GloobusThemeEngine::GloobusThemeEngine( GloobusConfig *cfg )
	:config(cfg)
	,shadow_radius(cfg->get_shadow_radius())
	,shadow_alpha(cfg->get_shadow_alpha())
{}

void GloobusThemeEngine::set_shadow( int radius , double alpha )
{
	shadow_radius = radius;
	shadow_alpha = alpha;
}

// ========================================================================== //

class GloobusThemeEngineCairo : public GloobusThemeEngine {
	private:
		double				border_width;
		int					window_radius;
		GloobusColor	*	border_color;
		GloobusColor	*	window_color;
		GdkPixbuf		*	pattern;
		double				alpha;
	
	public:
				GloobusThemeEngineCairo		( GloobusConfig *cfg );
	virtual	~GloobusThemeEngineCairo	( void );
	virtual	void	draw_background		( cairo_t *cr , int w , int h );
	virtual	bool	parse_attributes	( const gchar **attribute_names, const gchar **attribute_values );
	
	private:
				void	draw_shadow	( cairo_t *cr , int w , int h );
};

GloobusThemeEngineCairo::GloobusThemeEngineCairo( GloobusConfig *cfg )
	:GloobusThemeEngine(cfg)
	,border_width(1)
	,window_radius(7)
	,border_color(new GloobusColor)
	,window_color(new GloobusColor)
	,pattern(0)
	,alpha(1)
{
	window_color->set(0.08,0.08,0.08,0.91);
	border_color->set(0.71,0.71,0.71,0.85);
}

GloobusThemeEngineCairo::~GloobusThemeEngineCairo( void )
{
	delete border_color;
	delete window_color;
	if(pattern)
		g_object_unref(pattern);
}

void GloobusThemeEngineCairo::draw_background( cairo_t *cr, int w, int h)
{
	cairo_save(cr);
	if(shadow_radius)
		draw_shadow(cr, w, h);

	double x = shadow_radius;
	double y = shadow_radius;
	double width = w - shadow_radius*2;
	double height = h - shadow_radius*2;

	if(window_radius) {
		double r = window_radius;
		cairo_move_to  ( cr, x + r, y );
		cairo_line_to  ( cr, x + width - r, y );
		cairo_curve_to ( cr, x + width - r, y, x + width, y, x + width, y+r );
		cairo_line_to  ( cr, x + width, y + height - r );
		cairo_curve_to ( cr, x + width, y + height - r, x + width, y + height, x + width - r, y + height );
		cairo_line_to  ( cr, x + r, y + height );
		cairo_curve_to ( cr, x + r, y + height, x, y + height, x, y + height - r );
		cairo_line_to  ( cr, x, y + r );
		cairo_curve_to ( cr, x, y + r, x, y, x + r, y);
	} else {
		cairo_rectangle(cr, x, y, width, height);
	}

	cairo_clip_preserve( cr );

	cairo_set_operator 		( cr, CAIRO_OPERATOR_CLEAR );
	cairo_paint( cr );
	cairo_set_operator 		( cr, CAIRO_OPERATOR_OVER );
	if(pattern) {
		gdk_cairo_set_source_pixbuf(cr, pattern, x,y);
		cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
	} else {
		cairo_set_source_rgba	( cr, window_color->red, window_color->green, window_color->blue, window_color->alpha);
	}
	cairo_paint_with_alpha( cr , alpha );
	if(border_width > 0) {
		cairo_set_source_rgba( cr, border_color->red, border_color->green, border_color->blue, border_color->alpha);
		cairo_set_line_width( cr , border_width*2); // *2 for drawing line completely inside clip region
		cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
		cairo_stroke			( cr );
	}
	
	cairo_restore(cr);
}

void GloobusThemeEngineCairo::draw_shadow( cairo_t *cr, int w, int h )
{
	cairo_save( cr );
	
	double o = shadow_radius + window_radius;
	double r = window_radius;
	double s = shadow_radius;
	double a = shadow_alpha;
	cairo_pattern_t* pattern;
	
	// top left
	pattern = cairo_pattern_create_radial	( o, o, r, o, o, o );
	cairo_pattern_add_color_stop_rgba 		( pattern, 0, 0, 0, 0, a );
	cairo_pattern_add_color_stop_rgba 		( pattern, 1, 0, 0, 0, 0 );
	cairo_set_source 	(cr, pattern);
	cairo_rectangle 	(cr, 0, 0, o, o);
	cairo_fill 			(cr);
	cairo_pattern_destroy( pattern );

	// top
	pattern = cairo_pattern_create_linear	( 0, 0, 0, s);
	cairo_pattern_add_color_stop_rgba 		( pattern, 0, 0, 0, 0, 0 );
	cairo_pattern_add_color_stop_rgba 		( pattern, 1, 0, 0, 0, a);
	cairo_set_source 	(cr, pattern);
	cairo_rectangle 	(cr, o, 0, w-o*2, s);
	cairo_fill 			(cr);
	cairo_pattern_destroy( pattern );

	// top rigth
	pattern = cairo_pattern_create_radial	( w-o, o, r, w-o, o, o );
	cairo_pattern_add_color_stop_rgba 		( pattern, 0, 0, 0, 0, a );
	cairo_pattern_add_color_stop_rgba 		( pattern, 1, 0, 0, 0, 0 );
	cairo_set_source 	(cr, pattern);
	cairo_rectangle 	(cr, w-o, 0, o, o);
	cairo_fill 			(cr);
	cairo_pattern_destroy( pattern );
	
	//right
	pattern = cairo_pattern_create_linear	( w-s, 0, w, 0);
	cairo_pattern_add_color_stop_rgba 		( pattern, 0, 0, 0, 0, a );
	cairo_pattern_add_color_stop_rgba 		( pattern, 1, 0, 0, 0, 0);
	cairo_set_source 	(cr, pattern);
	cairo_rectangle 	(cr, w-s, o, s, h-o*2);
	cairo_fill 			(cr);
	cairo_pattern_destroy( pattern );
	
		
	// bottom rigth
	pattern = cairo_pattern_create_radial	( w-o, h-o, r, w-o, h-o, o);
	cairo_pattern_add_color_stop_rgba 		( pattern, 0, 0, 0, 0, a );
	cairo_pattern_add_color_stop_rgba 		( pattern, 1, 0, 0, 0, 0 );
	cairo_set_source 	(cr, pattern);
	cairo_rectangle 	(cr, w-o, h-o, o, o);
	cairo_fill 			(cr);
	cairo_pattern_destroy( pattern );
	
	// bottom
	pattern = cairo_pattern_create_linear	( 0, h-s, 0, h);
	cairo_pattern_add_color_stop_rgba 		( pattern, 0, 0, 0, 0, a );
	cairo_pattern_add_color_stop_rgba 		( pattern, 1, 0, 0, 0, 0);
	cairo_set_source 	(cr, pattern);
	cairo_rectangle 	(cr, o, h-s, w-o*2, s);
	cairo_fill 			(cr);
	cairo_pattern_destroy( pattern );

	// bottom left
	pattern = cairo_pattern_create_radial	( o, h-o, r, o, h-o, o);
	cairo_pattern_add_color_stop_rgba 		( pattern, 0, 0, 0, 0, a );
	cairo_pattern_add_color_stop_rgba 		( pattern, 1, 0, 0, 0, 0 );
	cairo_set_source 	(cr, pattern);
	cairo_rectangle 	(cr, 0, h-o, o, o);
	cairo_fill 			(cr);
	cairo_pattern_destroy( pattern );

	// left
	pattern = cairo_pattern_create_linear	( 0, 0, s, 0 );
	cairo_pattern_add_color_stop_rgba 		( pattern, 0, 0, 0, 0, 0 );
	cairo_pattern_add_color_stop_rgba 		( pattern, 1, 0, 0, 0, a);
	cairo_set_source 	(cr, pattern);
	cairo_rectangle 	(cr, 0, o, s, h-o*2);
	cairo_fill 			(cr);
	cairo_pattern_destroy( pattern );

	cairo_restore( cr );
}

bool GloobusThemeEngineCairo::parse_attributes( const gchar **attribute_names,
											const gchar **attribute_values )
{
	for( int i=0 ; attribute_names[i] ; i++ ) {
		if( !g_strcmp0(attribute_names[i] , "radius") ) {
			window_radius = g_ascii_strtoull(attribute_values[i], NULL,10);
		} else if( !g_strcmp0(attribute_names[i] , "color") ) {
			if( !window_color->parse_color(attribute_values[i]) )
				g_warning(_("Bad window color value"));
		} else if( !g_strcmp0(attribute_names[i] , "border-color") ) {
			if( !border_color->parse_color(attribute_values[i]) )
				g_warning(_("Bad border color value"));
		} else if( !g_strcmp0(attribute_names[i] , "border-width") ) {
			gdouble w = g_ascii_strtod(attribute_values[i], NULL);
			if( w < 0 )
				g_warning(_("Bad border width value"));
			else
				border_width = w;
		} else if( !g_strcmp0(attribute_names[i] , "pattern") ) {
			char *path = g_strconcat(config->get_theme(),"/",attribute_values[i],NULL);
			pattern = gdk_pixbuf_new_from_file(path, NULL);
			if(!pattern)
				g_warning(_("Error opening pattern file"));
		} else if( !g_strcmp0(attribute_names[i] , "opacity") ) {
			gdouble a = g_ascii_strtod(attribute_values[i], NULL);
			if( a < 0 || a > 1 )
				g_warning(_("Bad opacity value"));
			else
				alpha = a;
		}
	}
	return true;
}

// ========================================================================== //

GloobusTheme::GloobusTheme( void )
	:config( Gloobus::instance()->get_config() )
	,use_gtk(false)
	,loaded(false)
	,path(0)
	,icons( g_hash_table_new_full( g_str_hash , g_str_equal , g_free , theme_icon_destroy ) )
	,error_color( new GdkColor )
	,header_color( new GdkColor )
	,text_color( new GdkColor )
	,error_font( pango_font_description_new() )
	,header_font( pango_font_description_new() )
	,text_font( pango_font_description_new() )
	,container(new GloobusContainerSize)
	,header(new GloobusContainerSize)
	,toolbar(new GloobusContainerSize)
{
	g_debug("Creating GloobusTheme");
	use_gtk = config->get_theme_gtk();
	load();
}

GloobusTheme::~GloobusTheme( void ) {}

void GloobusTheme::clean( void )
{
	if(loaded) {
		// TOOD: clean everything for loading new theme
		loaded = false;
	}
}

void GloobusTheme::calc_add_sizes( int *w, int *h)
{
	*w = config->get_shadow_radius()*2 +
		container->left + container->right;
		
	*h = config->get_shadow_radius()*2 +
		header->top +
		header->size +
		header->bottom +
		container->top +
		container->bottom;
}

void GloobusTheme::load( void )
{
	//TODO: error handling
	clean();
	load_default();
	loaded = true;

	path = config->get_theme();
	if(path == NULL) return;
	gchar *xml_path = g_strconcat(path,"/","theme.xml",NULL);
	GFile *file = g_file_new_for_path(xml_path);
	GFileInfo * info = g_file_query_info(file,"standard::size", G_FILE_QUERY_INFO_NONE, NULL, NULL);
	gsize size = g_file_info_get_attribute_uint64 ( info, "standard::size" ) + 1;
	gchar text[size];
	gsize count;
	GFileInputStream * stream = g_file_read(file, NULL, NULL);
	g_input_stream_read_all(G_INPUT_STREAM(stream), text, size, &count, NULL, NULL);
	gssize text_len = (gssize)count;

	GMarkupParser parser;
	GMarkupParseContext *context;
	GError *error;
		
	parser.start_element = &xml_start_element;
	parser.end_element = &xml_end_element;
	parser.text = NULL;
	parser.passthrough = NULL;
	parser.error = &xml_error;
		
	context = g_markup_parse_context_new(&parser, G_MARKUP_TREAT_CDATA_AS_TEXT, this, NULL );//&xml_destroy_notify
	g_markup_parse_context_parse(context, text, text_len, &error);
	g_markup_parse_context_free(context);
	g_input_stream_close(G_INPUT_STREAM(stream),NULL,NULL);
	g_object_unref(info);
	g_object_unref(file);
	g_free(xml_path);
}

void GloobusTheme::load_default( void )
{
	if( !use_gtk ) {
		g_hash_table_insert(icons, g_strdup("close"), new GloobusIconInfo(
			g_strconcat(PACKAGE_DATA_PATH , "/window-close.svg", NULL)));

		g_hash_table_insert(icons, g_strdup("menu"), new GloobusIconInfo(
			g_strconcat(PACKAGE_DATA_PATH , "/gloobus-menu.svg", NULL)));

		g_hash_table_insert(icons, g_strdup("fullscreen"), new GloobusIconInfo(
			g_strconcat(PACKAGE_DATA_PATH , "/view-fullscreen.svg", NULL)));

		g_hash_table_insert(icons, g_strdup("unfullscreen"), new GloobusIconInfo(
			g_strconcat(PACKAGE_DATA_PATH , "/view-restore.svg", NULL)));

		g_hash_table_insert(icons, g_strdup("play"), new GloobusIconInfo(
			g_strconcat(PACKAGE_DATA_PATH , "/media-play.svg", NULL)));

		g_hash_table_insert(icons, g_strdup("pause"), new GloobusIconInfo(
			g_strconcat(PACKAGE_DATA_PATH , "/media-pause.svg", NULL)));

		g_hash_table_insert(icons, g_strdup("defaultapp"), new GloobusIconInfo(
			g_strconcat(PACKAGE_DATA_PATH , "/document-open.svg", NULL)));

		g_hash_table_insert(icons, g_strdup("nocover"), new GloobusIconInfo(
			g_strconcat(PACKAGE_DATA_PATH , "/cover-missing.svg", NULL)));

		g_hash_table_insert(icons, g_strdup("install"), new GloobusIconInfo(
			g_strconcat(PACKAGE_DATA_PATH , "/font-install.svg", NULL)));
	}
	pango_font_description_set_family	(error_font, "Lucida Grande");
	pango_font_description_set_size		(error_font, 10*PANGO_SCALE);
	pango_font_description_set_style	(error_font, PANGO_STYLE_NORMAL);
	pango_font_description_set_variant	(error_font, PANGO_VARIANT_NORMAL);
	pango_font_description_set_weight	(error_font, PANGO_WEIGHT_NORMAL);

	pango_font_description_set_family	(header_font, "Lucida Grande");
	pango_font_description_set_size		(header_font, 10*PANGO_SCALE);
	pango_font_description_set_style	(header_font, PANGO_STYLE_NORMAL);
	pango_font_description_set_variant	(header_font, PANGO_VARIANT_NORMAL);
	pango_font_description_set_weight	(header_font, PANGO_WEIGHT_BOLD);

	pango_font_description_set_family	(text_font, "Lucida Grande");
	pango_font_description_set_size		(text_font, 10*PANGO_SCALE);
	pango_font_description_set_style	(text_font, PANGO_STYLE_NORMAL);
	pango_font_description_set_variant	(text_font, PANGO_VARIANT_NORMAL);
	pango_font_description_set_weight	(text_font, PANGO_WEIGHT_NORMAL);
	
	gdk_color_parse("#d23a00", error_color);
	gdk_color_parse("#cccccc", header_color);
	gdk_color_parse("#ffffff", text_color);

	container->top = 0;
	container->bottom = 0;
	container->left = 1;
	container->right = 1;

	header->size = 16;
	header->top = 3;
	header->bottom = 3;
	header->left = 5;
	header->right = 5;

	toolbar->size = 64;
	toolbar->top = 3;
	toolbar->bottom = 3;
	toolbar->left = 5;
	toolbar->right = 5;
	
	toolbar_icon_size = 32;

	engine = new GloobusThemeEngineCairo(config);
}

void GloobusTheme::draw_background( cairo_t *cr, int width, int height)
{
	engine->draw_background( cr, width, height);
}

GtkWidget* GloobusTheme::get_error_label( const gchar* text , int size , int max_width )
{
	GtkWidget *label = gtk_label_new(text);
	if(max_width)
		gtk_label_set_max_width_chars (GTK_LABEL(label), max_width/6 );

	if(use_gtk)
		return label;
	
	if(size) {
		PangoFontDescription *font = pango_font_description_copy(error_font);
		pango_font_description_set_size(font, size*PANGO_SCALE);
		gtk_widget_modify_font(label, font);
		pango_font_description_free(font);
	} else {
		gtk_widget_modify_font(label, error_font);
	}
	gtk_widget_modify_fg(label, GTK_STATE_NORMAL,error_color);

	return label;
}

GdkPixbuf * GloobusTheme::get_gtk_icon( const gchar* name , int size )
{
	char *gtk_name = NULL;

	if( !g_strcmp0(name , "close") )
		gtk_name = g_strdup("gtk-close");
	else if( !g_strcmp0(name , "menu") )
		gtk_name = g_strdup("gnome-gmenu"); // is it the right one?
	else if( !g_strcmp0(name , "fullscreen") )
		gtk_name = g_strdup("gtk-fullscreen");
	else if( !g_strcmp0(name , "unfullscreen") )
		gtk_name = g_strdup("gtk-leave-fullscreen");
	else if( !g_strcmp0(name , "play") )
		gtk_name = g_strdup("media-playback-start");
	else if( !g_strcmp0(name , "pause") )
		gtk_name = g_strdup("media-playback-pause");
	else if( !g_strcmp0(name , "defaultapp") )
		gtk_name = g_strdup("gtk-open");
	else if( !g_strcmp0(name , "error") )
		gtk_name = g_strdup("gtk-dialog-error");
	else if( !g_strcmp0(name , "nocover") )
		gtk_name = g_strdup("stock_music-library"); // or music-player

	if(!gtk_name)
		return NULL;
	
	GtkIconTheme *icon_theme;
	icon_theme = gtk_icon_theme_get_default ();
	return gtk_icon_theme_load_icon(icon_theme, gtk_name, size, GTK_ICON_LOOKUP_FORCE_SIZE, NULL);
}

int GloobusTheme::get_header_icon_size( void )
{
	return header->size;
}

int GloobusTheme::get_header_size( void )
{
	return header->top + header->size + header->bottom;
}

int GloobusTheme::get_toolbar_size( void )
{
	return toolbar->top + toolbar->size + toolbar->bottom;
}

GtkWidget* GloobusTheme::get_container_align( void )
{
	GtkWidget *align = gtk_alignment_new(0.5,0.5,1,1);
	if(!config->get_theme_gtk()) {
		gtk_alignment_set_padding( GTK_ALIGNMENT(align),
			container->top, container->bottom, container->left, container->right );
	}
	return align;
}

GtkWidget* GloobusTheme::get_header_align( void )
{
	GtkWidget *align = gtk_alignment_new(0.5,0.5,1,0);
	if(!config->get_theme_gtk()) {
		gtk_alignment_set_padding( GTK_ALIGNMENT(align),
			header->top, header->bottom, header->left, header->right );
	}
	gtk_widget_set_size_request(align, -1, header->top + header->size + header->bottom);
	return align;
}

GtkWidget* GloobusTheme::get_toolbar_align( void )
{
	GtkWidget *align = gtk_alignment_new(0.5,0.5,1,0);
	gtk_alignment_set_padding( GTK_ALIGNMENT(align),
		toolbar->top, toolbar->bottom, toolbar->left, toolbar->right );
	gtk_widget_set_size_request(align, -1, toolbar->top + toolbar->size + toolbar->bottom);
	return align;
}

GtkWidget* GloobusTheme::get_header_label( const gchar* text )
{
	GtkWidget *label = gtk_label_new(text);
	gtk_widget_modify_font(label, header_font);
	gtk_widget_modify_fg(label, GTK_STATE_NORMAL,header_color);
	return label;
}

GdkPixbuf* GloobusTheme::get_icon( const gchar *name, int size )
{
	GloobusIconInfo *icon = (GloobusIconInfo*)g_hash_table_lookup(icons, name);
	if( !icon ) {
		GdkPixbuf * temp = get_gtk_icon(name, size);
		if(temp) {
			icon = new GloobusIconInfo(temp);
			g_hash_table_insert(icons, g_strdup(name), icon);
			return temp;
		} else {
			return NULL;
		}
	}

	if( !icon->icon ) {
		icon->icon = gdk_pixbuf_new_from_file_at_size( icon->path, size, size , NULL );
	}
	g_object_ref(icon->icon);
	return icon->icon;
}

GtkWidget* GloobusTheme::get_label( const gchar* text , int size, int max_width )
{
	GtkWidget *label = gtk_label_new(text);
	if(max_width)
		gtk_label_set_max_width_chars (GTK_LABEL(label), max_width/6 );
	if(use_gtk)
		return label;
	
	if(size) {
		PangoFontDescription *font = pango_font_description_copy(text_font);
		pango_font_description_set_size(font, size*PANGO_SCALE);
		gtk_widget_modify_font(label, font);
		pango_font_description_free(font);
	} else {
		gtk_widget_modify_font(label, text_font);
	}
	gtk_widget_modify_fg(label, GTK_STATE_NORMAL,text_color);

	return label;
}

int GloobusTheme::get_toolbar_icon_size( void )
{
	return toolbar_icon_size;
}

GdkColor * GloobusTheme::get_text_color( void )
{
	return text_color;
}

GdkColor * GloobusTheme::get_error_color( void )
{
	return error_color;
}



// ========================================================================== //

void GloobusTheme::parse_header_attributes( const gchar **attribute_names,
										const gchar **attribute_values)
{
	for( int i=0 ; attribute_names[i] ; i++) {
		if( !g_strcmp0(attribute_names[i] , "height") ) {
			guint64 height = g_ascii_strtoull(attribute_values[i], NULL,10);
			if(height)
				//header_size = height;
				header->size = height;
			else
				g_warning(_("Bad header height value"));
		} else if( !g_strcmp0(attribute_names[i] , "padding") ) {
			if( !header->parse(attribute_values[i]) )
				g_warning(_("Bad header padding settings"));
		}
	}
}

void GloobusTheme::parse_icon_attributes( const gchar **attribute_names,
										const gchar **attribute_values)
{
	char *icon_name = NULL;
	char *icon_path = NULL;
	for( int i=0 ; attribute_names[i] ; i++) {
		if( !g_strcmp0(attribute_names[i] , "name") ) {
			icon_name = g_strdup(attribute_values[i]);
		} else if( !g_strcmp0(attribute_names[i] , "path") ) {
			icon_path = g_strconcat(path,"/",attribute_values[i],NULL);
		}
	}
	if( !icon_name || !icon_path ) {
		g_warning(_("Bad icon entry"));
		return;
	}
	g_hash_table_replace(icons, icon_name, new GloobusIconInfo(icon_path));
}

void GloobusTheme::parse_text_attributes( const gchar **attribute_names,
										const gchar **attribute_values,
										PangoFontDescription *font,
										GdkColor *color)
{
	for( int i=0 ; attribute_names[i] ; i++ ) {
		if( !g_strcmp0(attribute_names[i] , "color") ) {
			if( !gdk_color_parse(attribute_values[i] , color) )
				g_warning(_("Bad text color value"));
		} else if( !g_strcmp0(attribute_names[i] , "family") ) {
			pango_font_description_set_family(font, attribute_values[i]);
		} else if( !g_strcmp0(attribute_names[i] , "size") ) {
			guint64 size = g_ascii_strtoull(attribute_values[i], NULL,10);
			if(size)
				pango_font_description_set_size(font, size*PANGO_SCALE);
			else
				g_warning(_("Bad text size value"));
		} else if( !g_strcmp0(attribute_names[i] , "style") ) {
			if( !g_strcmp0(attribute_values[i] , "normal") )
				pango_font_description_set_style(font, PANGO_STYLE_NORMAL);
			else  if( !g_strcmp0(attribute_values[i] , "oblique") )
				pango_font_description_set_style(font, PANGO_STYLE_OBLIQUE);
			else if( !g_strcmp0(attribute_values[i] , "italic") )
				pango_font_description_set_style(font, PANGO_STYLE_ITALIC);
			else
				g_warning(_("Bad text style value"));
		} else if( !g_strcmp0(attribute_names[i] , "variant") ) {
			if( !g_strcmp0(attribute_values[i] , "normal") )
				pango_font_description_set_variant(font, PANGO_VARIANT_NORMAL);
			else if( !g_strcmp0(attribute_values[i] , "small-camps") )
				pango_font_description_set_variant(font, PANGO_VARIANT_SMALL_CAPS);
			else
				g_warning(_("Bad text variant value"));
		} else if( !g_strcmp0(attribute_names[i] , "weight") ) {
			if( !g_strcmp0(attribute_values[i] , "normal") )
				pango_font_description_set_weight(font, PANGO_WEIGHT_NORMAL);
			else if( !g_strcmp0(attribute_values[i] , "bold") )
				pango_font_description_set_weight(font, PANGO_WEIGHT_BOLD);
			else
				g_warning(_("Bad text weight value"));
		} //else {
		//	g_warning("%s: %s", _("Uknown text attribute"), attribute_names[i]);
		//}
	}
}

void GloobusTheme::parse_toolbar_attributes( const gchar **attribute_names,
										const gchar **attribute_values)
{
	for( int i=0 ; attribute_names[i] ; i++) {
		if( !g_strcmp0(attribute_names[i] , "height") ) {
			guint64 height = g_ascii_strtoull(attribute_values[i], NULL,10);
			if(height)
				toolbar->size = height;
			else
				g_warning(_("Bad toolbar height value"));
		} else if( !g_strcmp0(attribute_names[i] , "icon-size") ) {
			guint64 size = g_ascii_strtoull(attribute_values[i], NULL,10);
			if(size)
				toolbar_icon_size = size;
			else
				g_warning(_("Bad toolbar icon size value"));
		} else if( !g_strcmp0(attribute_names[i] , "padding") ) {
			if( !toolbar->parse(attribute_values[i]) )
				g_warning(_("Bad toolbar padding settings"));
		}
	}
}

void GloobusTheme::parse_settings_attributes( const gchar **attribute_names,
										const gchar **attribute_values)
{
	for( int i=0 ; attribute_names[i] ; i++) {
		if( !g_strcmp0(attribute_names[i] , "padding") ) {
			if( !container->parse(attribute_values[i]) )
				g_warning(_("Bad container padding settings"));
		}
	}
}

void GloobusTheme::theme_icon_destroy( gpointer data)
{
	GloobusIconInfo * ptr = (GloobusIconInfo*)data;
	delete ptr;
}

void GloobusTheme::xml_error(GMarkupParseContext *context,
					GError *error,
					gpointer user_data)
{}

void GloobusTheme::xml_end_element(GMarkupParseContext *context,
					const gchar *element_name,
					gpointer user_data,
					GError **error)
{}

void GloobusTheme::xml_start_element( GMarkupParseContext *context,
									const gchar *element_name,
									const gchar **attribute_names,
									const gchar **attribute_values,
									gpointer user_data,
									GError **error)
{
	GloobusTheme *theme = (GloobusTheme*)user_data;
	//g_debug(element_name);
	if( !g_strcmp0(element_name , "error-text") ) {
		theme->parse_text_attributes(attribute_names, attribute_values, theme->error_font, theme->error_color);
	} else if( !g_strcmp0(element_name , "header") ) {
		theme->parse_text_attributes(attribute_names, attribute_values, theme->header_font, theme->header_color);
		theme->parse_header_attributes(attribute_names, attribute_values);
	} else if( !g_strcmp0(element_name , "text") ) {
		theme->parse_text_attributes(attribute_names, attribute_values, theme->text_font, theme->text_color);
	} else if( !g_strcmp0(element_name , "icon") ) {
		theme->parse_icon_attributes(attribute_names, attribute_values);
	} else if( !g_strcmp0(element_name , "background") ) {
		for( int i=0 ; attribute_names[i] ; i++ ) {
			if( !g_strcmp0(attribute_names[i] , "engine") ) {
				if( !g_strcmp0(attribute_values[i] , "cairo") ) {
					delete theme->engine;
					theme->engine = new GloobusThemeEngineCairo(theme->config);
					theme->engine->parse_attributes(attribute_names, attribute_values);
				}
			}
		}
	} else if( !g_strcmp0(element_name , "settings") ) {
		theme->parse_settings_attributes(attribute_names, attribute_values);
	} else if( !g_strcmp0(element_name , "toolbar") ) {
		theme->parse_toolbar_attributes(attribute_names, attribute_values);
	}
}
