#include "gloobus-preview-window.h"
#include "gloobus-preview-singleton.h"

/*Translations*/
#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)

#define GP_SCREEN_BORDER_GAP 150

#define GP_WINDOW_WIDTH 200
#define GP_WINDOW_HEIGHT 200

#define GP_CONTAINER_MIN_WIDTH 300
#define GP_CONTAINER_MIN_HEIGHT 300

static GdkRGBA GP_BLACK = {0, 0, 0, 1};
static GdkRGBA GP_WHITE = {1, 1, 1, 1};

GloobusUI::GloobusUI( void )
    :config( Gloobus::instance()->get_config() )
    ,floating_toolbar(0)
    ,theme(new GloobusTheme)
    ,fullscreen(false)
    ,window(0)
    ,width(GP_WINDOW_WIDTH)
    ,height(GP_WINDOW_HEIGHT)
    ,screen_width(0)
    ,screen_height(0)
    ,background(0)
    ,container(0)
    ,frame(0)
    ,header(0)
    ,toolbar(0)
    ,title(0)
    ,btn_close(0)
    ,btn_menu(0)
    ,btn_title(0)
    ,btn_fullscreen(0)
    ,btn_defaultapp(0)
    //,controls_visible(false)
    ,menu(0)
{
    g_debug("Creating GloobusUI");
    draw_background( width, height );
    create_window();
    if( !config->get_theme_gtk() ) {
        create_header();
    }
    gtk_widget_show_all(window);
    theme->calc_add_sizes(&add_width, &add_height);
}

GloobusUI::~GloobusUI( void ) {}

GtkWidget * GloobusUI::create_button( const gchar* name , int size )
{
    GdkPixbuf * icon = theme->get_icon( name, size );
    GtkWidget * image = gtk_image_new_from_pixbuf(icon);
    GtkWidget * btn;
    if(config->get_theme_gtk()) {
        btn = gtk_button_new();
    } else {
        btn = gtk_event_box_new();
        gtk_event_box_set_visible_window( GTK_EVENT_BOX(btn) , false );
    }
    gtk_container_add( GTK_CONTAINER(btn) , image );
    return btn;
}

GdkPixbuf * GloobusUI::create_pixbuf( const string &type , int w , int h ) {
    /* TODO: change plugins and remove this method */
    string path(PACKAGE_DATA_PATH);

    if( type == "close" )
        path = path + "/window-close.svg";
    else if( type == "menu" )
        path = path + "/gloobus-menu.svg";
    else if( type == "fullscreen" )
        path = path + "/view-fullscreen.svg";
    else if( type == "unfullscreen" )
        path = path + "/view-restore.svg";
    else if( type == "play" )
        path = path + "/media-play.svg";
    else if( type == "pause" )
        path = path + "/media-pause.svg";
    else if( type == "open" )
        path = path + "/document-open.svg";
    else if( type == "toolbar" )
        path = path + "/floating-bar.svg";
    else
        return 0;

    return gdk_pixbuf_new_from_file_at_size( path.c_str(), w, h , NULL );
}

GtkWidget * GloobusUI::create_title( void )
{
    title = theme->get_header_label("Gloobus-Preview");
    GtkWidget *btn = gtk_event_box_new();
    gtk_event_box_set_visible_window( GTK_EVENT_BOX(btn) , false );
    gtk_container_add( GTK_CONTAINER(btn) , title );

    return btn;
}

void GloobusUI::create_header( void )
{
    g_debug("Creating GloobusUI header");
    btn_close = create_button( "close" , theme->get_header_icon_size());
    btn_menu = create_button( "menu" , theme->get_header_icon_size());
    btn_title = create_title();
    header = theme->get_header_align();

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    //GtkWidget *hsize = gtk_alignment_new(0,0,0,0);
    //gtk_widget_set_size_request(hsize, 1, theme->get_header_size());

    if (config->get_winbar_layout()) {
        gtk_box_pack_start(GTK_BOX(hbox), btn_close, false, false, 0);
        //gtk_box_pack_start(GTK_BOX(hbox), hsize, false, false, 0); //force header height
        gtk_box_pack_end(GTK_BOX(hbox), btn_menu, false, false, 0);
    } else {
        gtk_box_pack_start(GTK_BOX(hbox), btn_menu, false, false, 0);
        //gtk_box_pack_start(GTK_BOX(hbox), hsize, false, false, 0); //force header height
        gtk_box_pack_end(GTK_BOX(hbox), btn_close, false, false, 0);
    }
    gtk_box_pack_start(GTK_BOX(hbox), btn_title, true, true, 0);

    gtk_container_add( GTK_CONTAINER(header) , hbox );
    gtk_box_pack_start(GTK_BOX(frame), header, false, false, 0);

    g_signal_connect(btn_close, "button-press-event", G_CALLBACK(btn_close_cb), this);
    g_signal_connect(btn_menu, "button-press-event", G_CALLBACK(btn_menu_cb), this);
    g_signal_connect(btn_title, "button-press-event", G_CALLBACK(btn_title_cb), this);
}

void GloobusUI::create_window( void )
{
    g_debug("Creating GloobusUI window");
    GdkScreen * screen = gdk_screen_get_default();
    screen_width = gdk_screen_get_width(screen);
    screen_height = gdk_screen_get_height(screen);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size     ( GTK_WINDOW(window), width, height);
    gtk_window_set_position         ( GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    //gtk_window_set_position           ( GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_keep_above       ( GTK_WINDOW(window), config->get_ontop());
    gtk_window_set_skip_taskbar_hint( GTK_WINDOW(window), !config->get_taskbar());
    gtk_window_set_icon_name        ( GTK_WINDOW(window), "gloobus-preview");

    if (gdk_screen_is_composited(screen)) {
        GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
        if (visual) {
            gtk_widget_set_visual(window, visual);
        }
    }

    if( !config->get_theme_gtk() ) {
        gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_STATIC);

        gtk_widget_set_app_paintable    ( window, true );
        gtk_window_set_decorated        ( GTK_WINDOW(window), false);
        //gtk_widget_modify_bg            ( window, GTK_STATE_NORMAL, &black);
        gtk_container_set_border_width  ( GTK_CONTAINER(window), config->get_shadow_radius());

        gtk_widget_realize              ( window );
        //GdkRGBA color = {0.,0.,0.,0.8};
        //gtk_widget_override_background_color (window, GTK_STATE_FLAG_NORMAL, &color);

        g_signal_connect ( window, "draw", G_CALLBACK(draw_cb), this);
        g_signal_connect ( window, "configure-event", G_CALLBACK(configure_cb), this);
    }// else {
    //  gtk_window_set_position         ( GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
    //}
    frame = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), frame);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    if(config->get_focus()) {
        g_signal_connect ( window, "focus-out-event", G_CALLBACK(focus_out_cb), this);
    }

    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

    g_signal_connect (window, "button-press-event", G_CALLBACK(button_press_cb), this);
}

GtkWidget* GloobusUI::create_properties( const string &text , int max_width )
{
    // TODO: change plugins and remove this method ?
    return theme->get_label(text.c_str(), 0, max_width);
}

void GloobusUI::draw_background( int w , int h ) {
    if( config->get_theme_gtk() )
        return;
    if(background)
        cairo_surface_destroy( background );
    background = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
    cairo_t * cr = cairo_create( background );
    theme->draw_background(cr,w,h);
    cairo_destroy( cr );
}

void GloobusUI::set_fullscreen( bool full )
{
    if( full == fullscreen)
        return;

    fullscreen = full;

    if(fullscreen) {
        gtk_window_fullscreen(GTK_WINDOW(window));
        if(header)
            gtk_widget_hide(header);
        if(toolbar)
            gtk_widget_hide(toolbar);
        if( !config->get_theme_gtk() ) {
            gtk_container_set_border_width(GTK_CONTAINER(window),0);
        } else {
            gtk_widget_override_background_color (window, GTK_STATE_FLAG_NORMAL, &GP_BLACK);
            gtk_widget_override_color (window, GTK_STATE_FLAG_NORMAL, &GP_WHITE);
        }

        floating_toolbar = new GloobusToolbar();
    } else {
        gtk_window_unfullscreen(GTK_WINDOW(window));
        gtk_window_resize(GTK_WINDOW(window), width, height);
        if(header)
            gtk_widget_show_all(header);
        if(toolbar)
            gtk_widget_show_all(toolbar);
        if( !config->get_theme_gtk() )
            gtk_container_set_border_width(GTK_CONTAINER(window),config->get_shadow_radius());
        else {
            gtk_widget_override_background_color (window, GTK_STATE_FLAG_NORMAL, NULL);
            gtk_widget_override_color (window, GTK_STATE_FLAG_NORMAL, NULL);
        }
        delete floating_toolbar;
    }
}

void GloobusUI::set_title( const gchar *str )
{
    if( !config->get_theme_gtk() ) {
        gtk_label_set_text(GTK_LABEL(title), str);
    }
    gtk_window_set_title(GTK_WINDOW(window), str);
}

void GloobusUI::prepare_window( void )
{
    g_debug("Preparing window");
    if(toolbar) {
        gtk_widget_destroy(toolbar);
        toolbar = 0;
    }
    //gtk_widget_hide(toolbar);
    //controls_visible = false;

    if(container)
        gtk_widget_destroy(container);
    //container = gtk_fixed_new();
    container = theme->get_container_align();
    gtk_box_pack_start(GTK_BOX(frame), container, true, true, 0);
    gtk_widget_show( container );

    if(fullscreen) {
        delete floating_toolbar;
        floating_toolbar = new GloobusToolbar();
    } else {
        gtk_window_resize(GTK_WINDOW(window), GP_WINDOW_WIDTH, GP_WINDOW_HEIGHT);
    }
}

void GloobusUI::resize( int w, int h )
{
    g_debug("Resizing window");
    // TODO: make this method start animation ?
    int max_w = get_max_width();
    int max_h = get_max_height( (toolbar != 0) );

    if(w > max_w)
        w = max_w;
    if(h > max_h)
        h = max_h;
/*
    if(w < GP_CONTAINER_MIN_WIDTH)
        w = GP_CONTAINER_MIN_WIDTH;
    if(h < GP_CONTAINER_MIN_WIDTH)
        h = GP_CONTAINER_MIN_WIDTH;
*/
    if( !config->get_theme_gtk()) {
        w = w + add_width;
        h = h + add_height;
    }
    if(toolbar)
        h += theme->get_toolbar_size();
    if(fullscreen) {
        width = w;
        height = h;
    } else {
        g_debug("SIZE : %dx%d", w,h);
        //gtk_window_resize(GTK_WINDOW(window), w, h);
        gtk_widget_set_size_request(window, w, h);
    }
}

GdkPixbuf* GloobusUI::get_file_icon( GFileInfo *info , int size )
{
    GIcon *icon;
    GtkIconInfo *icon_info;
    GtkIconTheme *icon_theme;
    GdkPixbuf* temp;

    icon_theme = gtk_icon_theme_get_default();
    icon = g_file_info_get_icon(info);
    icon_info = gtk_icon_theme_lookup_by_gicon(icon_theme, icon, size,
        GtkIconLookupFlags(GTK_ICON_LOOKUP_USE_BUILTIN | GTK_ICON_LOOKUP_GENERIC_FALLBACK | GTK_ICON_LOOKUP_FORCE_SIZE));
    temp = gtk_icon_info_load_icon(icon_info, NULL);
    return temp;
}

GtkWidget * GloobusUI::get_container( void )
{
    return container;
}

GtkWidget * GloobusUI::get_toolbar( void )
{
    if(!toolbar) {
        toolbar = theme->get_toolbar_align();
        gtk_box_pack_end(GTK_BOX(frame), toolbar, false, false, 0);
        //if(fullscreen)
        gtk_widget_hide(toolbar);
    }
    return toolbar;
}

GtkWidget * GloobusUI::get_floating_toolbar( void )
{
    if(floating_toolbar)
        return floating_toolbar->get();
    return 0;
}

GtkWidget * GloobusUI::get_window( void )
{
    return window;
}

int GloobusUI::get_max_width( void )
{
    int max_width = screen_width - GP_SCREEN_BORDER_GAP;
    if( !config->get_theme_gtk() )
        max_width -= add_width;
    return max_width;
}

int GloobusUI::get_max_height( bool with_toolbar )
{
    int max_height = screen_height - GP_SCREEN_BORDER_GAP;
    if( ! config->get_theme_gtk() )
        max_height -= add_height;
    if(with_toolbar)
        max_height -= theme->get_toolbar_size();
    return max_height;
}

int GloobusUI::get_screen_width( void )
{
    return screen_width;
}

int GloobusUI::get_screen_height( void )
{
    return screen_height;
}

GloobusTheme* GloobusUI::get_theme( void )
{
    return theme;
}

bool GloobusUI::is_fullscreen( void ) {
    return fullscreen;
}

/* ******************************* CALLBACKS ******************************** */

gboolean GloobusUI::draw_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    GloobusUI *ui = static_cast<GloobusUI*>(data);

    //cairo_clip(cr);

    cairo_set_operator      ( cr, CAIRO_OPERATOR_CLEAR );
    cairo_paint( cr );
    cairo_set_operator      ( cr, CAIRO_OPERATOR_OVER );

    if(ui->fullscreen) {
        cairo_set_source_rgb( cr, 0, 0, 0 );
    } else {
        cairo_set_source_surface( cr, ui->background, 0, 0 );
    }
    cairo_paint( cr );

    return false;
}

gboolean GloobusUI::configure_cb( GtkWidget *widget,
                                GdkEventConfigure *event,
                                gpointer data )
{
    GloobusUI *ui = (GloobusUI*)data;
    if( ui->fullscreen )
        return false;
    if( ui->width != event->width || ui->height != event->height ) {
        ui->width = event->width;
        ui->height = event->height;
        ui->draw_background(ui->width,ui->height);
        int x = (ui->screen_width - ui->width)/2;
        int y = (ui->screen_height - ui->height)/2;
        gtk_window_move(GTK_WINDOW(widget), x, y);
        gtk_widget_queue_draw(widget);
    }
    return false;
}

/*
void GloobusUI::clean_bg( cairo_t * cr , int x , int y, int w , int h )
{
    cairo_save(cr);

    int pos_x = add_width + x;
    int pos_x = add_height + y;

    cairo_rectangle(cr, x ,y , w, h);
    cairo_clip(cr);

    cairo_set_operator      ( cr, CAIRO_OPERATOR_CLEAR );
    cairo_paint( cr );
    cairo_set_operator      ( cr, CAIRO_OPERATOR_OVER );

    if(fullscreen) {
        cairo_set_source_rgb( cr, 0, 0, 0 );
    } else {
        cairo_set_source_surface( cr, background, -1*pos_x, -1*pos_y );
    }
    cairo_paint( cr );

    cairo_restore(cr);
}
*/
gboolean GloobusUI::btn_close_cb(GtkWidget *widget,
                        GdkEventButton *event,
                        gpointer data)
{
    Gloobus::instance()->quit();
    return true;
}

GtkWidget* GloobusUI::add_menu_item ( string text, GCallback cb, gpointer data )
{
    if(!menu)
    {
        create_menu();
    }

    GtkWidget* entry =      gtk_menu_item_new_with_label(text.c_str());
    gtk_menu_shell_append  (GTK_MENU_SHELL(menu),entry);
    g_signal_connect       ( G_OBJECT(entry),"activate", G_CALLBACK(cb), data);
    g_debug("Add menu entry: %s", text.c_str());

    gtk_widget_show_all(GTK_WIDGET(entry));
    return entry;
}

void GloobusUI::create_menu ()
{
        menu = gtk_menu_new();
        GtkWidget *settings = gtk_menu_item_new_with_label(_("Settings"));
        GtkWidget *blog     = gtk_menu_item_new_with_label(_("Blog"));
        GtkWidget *donate   = gtk_menu_item_new_with_label(_("Donate"));
        GtkWidget *bug      = gtk_menu_item_new_with_label(_("Report a Problem"));
        GtkWidget *about    = gtk_menu_item_new_with_label(_("About"));
        GtkWidget *quit     = gtk_menu_item_new_with_label(_("Quit"));
        GtkWidget *separator        =  gtk_separator_menu_item_new();

        gtk_menu_shell_append  (GTK_MENU_SHELL(menu),settings);
        gtk_menu_shell_append  (GTK_MENU_SHELL(menu),blog);
        gtk_menu_shell_append  (GTK_MENU_SHELL(menu),donate);
        gtk_menu_shell_append  (GTK_MENU_SHELL(menu),bug);
        gtk_menu_shell_append  (GTK_MENU_SHELL(menu),about);
        gtk_menu_shell_append  (GTK_MENU_SHELL(menu),quit);
        gtk_menu_shell_append  (GTK_MENU_SHELL(menu),separator);

        g_signal_connect( G_OBJECT(settings),"activate", G_CALLBACK(menu_item_cb), this);
        g_signal_connect( G_OBJECT(blog)    ,"activate", G_CALLBACK(menu_item_cb), this);
        g_signal_connect( G_OBJECT(donate)  ,"activate", G_CALLBACK(menu_item_cb), this);
        g_signal_connect( G_OBJECT(bug)     ,"activate", G_CALLBACK(menu_item_cb), this);
        g_signal_connect( G_OBJECT(about)   ,"activate", G_CALLBACK(menu_item_cb), this);
        g_signal_connect( G_OBJECT(quit)    ,"activate", G_CALLBACK(menu_item_cb), this);

        gtk_widget_show_all(GTK_WIDGET(menu));

}
gboolean GloobusUI::btn_menu_cb(GtkWidget *widget,
                        GdkEventButton *event,
                        gpointer data)
{
    GloobusUI *ui = (GloobusUI*)data;
    if( !ui->menu ) {
        ui->create_menu();
    }

    gtk_menu_popup(GTK_MENU(ui->menu),NULL,NULL,NULL,NULL,1,gtk_get_current_event_time());
    return true;
}

gboolean GloobusUI::btn_title_cb(GtkWidget *widget,
                        GdkEventButton *event,
                        gpointer data)
{
    GloobusUI *ui = (GloobusUI*)data;
    if( event->button == 1) {
        gtk_window_begin_move_drag(GTK_WINDOW(ui->window), event->button,
            (int)event->x_root, (int)event->y_root, event->time);
        return true;
    }
    return false;
}

gboolean GloobusUI::button_press_cb( GtkWidget *widget, GdkEventButton *event, gpointer data )
{
    if (event->type == GDK_2BUTTON_PRESS) {
        Gloobus::instance()->fullscreen();
    } else if (event->button == 1) {
        GloobusUI *ui = (GloobusUI*)data;
        int r = ui->config->get_shadow_radius();
        if( event->y > (ui->height - r - 10) ) {
            if( event->x < r+10 ) {
                gtk_window_begin_resize_drag(GTK_WINDOW(ui->window),
                    GDK_WINDOW_EDGE_SOUTH_WEST, event->button,
                    event->x_root, event->y_root, event->time);
            } else if( event->x > (ui->width - r - 10) ) {
                gtk_window_begin_resize_drag(GTK_WINDOW(ui->window),
                    GDK_WINDOW_EDGE_SOUTH_EAST, event->button,
                    event->x_root, event->y_root, event->time);
            }
        }
    } else if(event->button == 3) {
        btn_menu_cb(widget, event, data);
        return true;
    }

    return false;
}

void GloobusUI::menu_item_cb( GtkMenuItem * item, gpointer data)
{
    GloobusUI *ui = (GloobusUI*)data;
    string uri;
    string label(gtk_menu_item_get_label(item));
    if(label ==  _("Settings")) {
        g_spawn_command_line_async("gloobus-preview-configuration", NULL);
        return;
    } else if(label ==  _("Report a Problem"))
        uri = "https://bugs.launchpad.net/gloobus-preview/";
    else if(label == _("Blog"))
        uri = "http://gloobus.wordpress.com" ;
    else if(label == _("Donate"))
        uri = "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=guitarboy000%40gmail%2ecom&item_name=Gloobus%2c%20A%20Quicklook%20for%20linux&no_shipping=0&no_note=1&tax=0&currency_code=EUR&lc=US&bn=PP%2dDonationsBF&chars" ;
    else if(label == _("About"))
        ui->create_about();
    else if(label == _("Quit"))
        Gloobus::instance()->quit();
    else {
        g_debug("Uknown menu item label");
        return;
    }
    g_app_info_launch_default_for_uri(uri.c_str(), NULL, NULL);
}

gboolean GloobusUI::focus_out_cb( GtkWidget *widget,
                                GdkEventFocus *event,
                                gpointer data)
{
    GloobusUI *ui = (GloobusUI*)data;
    if(ui->menu && gtk_widget_get_visible(ui->menu))
        return false;
    else
        Gloobus::instance()->quit();
    return true;
}
/*
gboolean GloobusUI::fullscreen_cb(GtkWidget *widget,
    GdkEventButton *event,
    gpointer data)
{
    Gloobus::instance()->fullscreen();
    return true;
};

gboolean GloobusUI::defaultapp_cb(GtkWidget *widget,
    GdkEventButton *event,
    gpointer data)
{
    Gloobus::instance()->defaultapp();
    return true;
};
*/
/* ************************************************************************** */

void GloobusUI::create_about( void )
{
    static const gchar *authors[] = {
        "Jordi Puigdellívol Hernández <guitarboy000@gmail.com>",
        "ammonkey (aka KitKat) <am.monkeyd@gmail.com>",
        "Alexandr Grigorcea <cahr.gr@gmail.com>",
        "Balló György <ballogyor@gmail.com>",
        NULL
    };

    static const gchar *artists[] = {
        "Jordi Puigdellívol Hernández <guitarboy000@gmail.com>",
        "Daniel Foré <Daniel.p.Fore@gmail.com>",
        NULL
    };

    string version = (string)VERSION;
    if (REVNO[0] != '\0')
        version +="\nbzr r" + (string)REVNO;

    gtk_show_about_dialog(GTK_WINDOW(window),
                        "program-name",             "Gloobus Preview",
                        "version",      version.c_str(),
                        "copyright",        "Copyright \xc2\xa9 2009-2014 Gloobus Developers",
                        "comments",         _("See your files in a blink."),
                        "authors",      authors,
                        "artists",      artists,
                        "translator-credits", _("translator-credits"),
                        "website",      "http://gloobus.net",
                        "website-label",    "Gloobus.net",
                        "logo-icon-name", "gloobus-preview",
                        "license-type", GTK_LICENSE_GPL_3_0,
                        NULL);

}

GdkPixbuf * GloobusUI::pixbuf_reflect( const GdkPixbuf *pixbuf )
{
    GdkPixbuf * temp;
    int w, h, stride;
    guchar *src, *dst, *ps, *pd;

    w = gdk_pixbuf_get_width( pixbuf );
    h = gdk_pixbuf_get_height( pixbuf ) / 2 + 1;
    stride = gdk_pixbuf_get_rowstride( pixbuf );
    temp = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, w, h);
    src = gdk_pixbuf_get_pixels( pixbuf );
    dst = gdk_pixbuf_get_pixels( temp );

    double alpha = 0.5; // initial opacity
    double step = alpha / ( h - 1 );

    int z = gdk_pixbuf_get_height( pixbuf )-1;
    for( int y=0 ; y < h ; y++, z-- ) {
        for( int x=0 ; x < w ; x++ ) {
            ps = src + z*stride + x*4;
            pd = dst + y*stride + x*4;
            pd[0] = ps[0];
            pd[1] = ps[1];
            pd[2] = ps[2];
            pd[3] = ps[3]*alpha;
        }
        alpha -= step;
        if(alpha < 0)
            alpha = 0;
    }

    return temp;
}

GdkPixbuf * GloobusUI::pixbuf_scale(GdkPixbuf *pixbuf, int max_w, int max_h , bool upscale)
{
    GdkPixbuf *result;
    int w = gdk_pixbuf_get_width(pixbuf);
    int h = gdk_pixbuf_get_height(pixbuf);

    bool modify = false;
    double ratio;

    if(w > max_w) {
        ratio = double(max_w)/double(w);
        w = max_w;
        h *= ratio;
        modify=true;
    }
    if(h > max_h) {
        ratio = double(max_h)/double(h);
        w *= ratio;
        h = max_h;
        modify=true;
    }

    if( !modify && upscale ) {
        if(w < max_w) {
            ratio = double(max_w)/double(w);
            w = max_w;
            h *= ratio;
            modify=true;
        }
        if(h < max_h) {
            ratio = double(max_h)/double(h);
            w *= ratio;
            h = max_h;
            modify=true;
        }
    }

    if(modify) {
        g_debug("Scaling image");
        result = gdk_pixbuf_scale_simple(pixbuf, w, h, GDK_INTERP_BILINEAR);
    } else {
        result = (GdkPixbuf*) g_object_ref(pixbuf);
    }
    if( !gdk_pixbuf_get_has_alpha( result ) ) {
        GdkPixbuf *temp = gdk_pixbuf_add_alpha(result, false, 0, 0, 0 );
        g_object_unref(result);
        result = temp;
    }
    return result;
}
