#include "gloobus-preview-toolbar.h"
#include "gloobus-preview-window.h"
#include "gloobus-preview-singleton.h"

GloobusToolbar::GloobusToolbar( void )
    :config( Gloobus::instance()->get_config() )
    ,timeout_id(0)
    ,fade_id(0)
    ,background(0)
    ,toolbar(0)
    ,alpha(0)
{
    /* TODO: make this class load info from theme class */
    GdkScreen *screen = gdk_screen_get_default();
    GloobusUI *ui = Gloobus::instance()->get_ui();
    background = ui->create_pixbuf("toolbar", 480,100);

    toolbar = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_widget_set_visual               (toolbar, gdk_screen_get_rgba_visual (screen));
    gtk_window_set_default_size         (GTK_WINDOW(toolbar), 480, 100      );  //default size
    gtk_widget_set_app_paintable        (toolbar            , true          );  //To draw in cairo
    gtk_window_set_keep_above           (GTK_WINDOW(toolbar), true          );  //Always on top
    gtk_widget_realize                  (toolbar );
    gtk_container_set_border_width(GTK_CONTAINER(toolbar), 10);

    //frame = gtk_fixed_new();
    frame = gtk_alignment_new(0.5,0.5,1,1);
    gtk_container_add(GTK_CONTAINER(toolbar),frame);

    int w = gdk_screen_get_width(screen);
    int h = gdk_screen_get_height(screen);
    gtk_window_move(GTK_WINDOW(toolbar), w/2 - 480/2, h - 120);

    GdkWindow *window = gtk_widget_get_window(toolbar);

    GdkEventMask event_mask = (GdkEventMask) (gdk_window_get_events (window) | GDK_POINTER_MOTION_MASK);
    gdk_window_set_events( gtk_widget_get_window(ui->get_window()), event_mask);
    gdk_window_set_events( window, event_mask);
    handler_id = g_signal_connect( ui->get_window(), "motion_notify_event", G_CALLBACK(motion_cb), this);
    g_signal_connect( toolbar , "motion_notify_event", G_CALLBACK(motion_cb), this);
    g_signal_connect( toolbar, "draw", G_CALLBACK(draw_cb), this);
    g_signal_connect (toolbar, "key-press-event",G_CALLBACK(key_press_cb), NULL);
    gtk_widget_show_all( toolbar );
    gtk_window_set_opacity(GTK_WINDOW(toolbar), 0);
}

GloobusToolbar::~GloobusToolbar( void )
{
    if(fade_id)
        g_source_remove(fade_id);
    if(timeout_id)
        g_source_remove(timeout_id);
    g_signal_handler_disconnect( Gloobus::instance()->get_ui()->get_window() , handler_id );
    g_object_unref( background );
    gtk_widget_destroy( toolbar );
}

GtkWidget * GloobusToolbar::get( void )
{
    return frame;
}

gboolean GloobusToolbar::draw_cb  (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    GloobusToolbar *toolbar = static_cast<GloobusToolbar*>(data);

    if( toolbar->config->get_theme_gtk() )
        return false;

    cairo_set_operator( cr, CAIRO_OPERATOR_CLEAR );
    cairo_paint( cr );
    cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
    gdk_cairo_set_source_pixbuf( cr, toolbar->background, 0, 0);
    cairo_paint( cr );

    return false;
}

gboolean GloobusToolbar::fade_cb( gpointer data )
{
    GloobusToolbar *toolbar = (GloobusToolbar*)data;
    toolbar->alpha -= 0.1;
    gtk_window_set_opacity(GTK_WINDOW(toolbar->toolbar), toolbar->alpha);
    return (toolbar->alpha > 0);
}

gboolean GloobusToolbar::key_press_cb ( GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    return Gloobus::key_press_cb(widget, event, data);
}

gboolean GloobusToolbar::motion_cb(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    GloobusToolbar *toolbar = (GloobusToolbar*)data;
    if(toolbar->fade_id)
        g_source_remove(toolbar->fade_id);
    if(toolbar->timeout_id)
        g_source_remove(toolbar->timeout_id);
    toolbar->alpha = 1;
    gtk_window_set_opacity(GTK_WINDOW(toolbar->toolbar), 1);
    toolbar->timeout_id = g_timeout_add(2000, &timeout_cb, data);
    return true;
}

gboolean GloobusToolbar::timeout_cb( gpointer data )
{
    GloobusToolbar *toolbar = (GloobusToolbar*)data;
    toolbar->fade_id = g_timeout_add(50, &fade_cb, data);
    return false;
}
