#ifndef _GLOOBUS_TOOLBAR_
#define _GLOOBUS_TOOLBAR_

#include <gtk/gtk.h>

#include "gloobus-preview-config.h"

class GloobusToolbar {
    private:
        GloobusConfig   *   config;
        int             timeout_id;
        int             fade_id;
        gulong          handler_id;
        GdkPixbuf   *   background;
        GtkWidget   *   toolbar;
        double          alpha;
        GtkWidget   *   frame;
    public:
        GloobusToolbar( void );
        ~GloobusToolbar( void );
        GtkWidget* get( void );
    private:
    static  gboolean    draw_cb          ( GtkWidget*, cairo_t*, gpointer );
    static  gboolean    fade_cb          ( gpointer );
    static  gboolean    key_press_cb     ( GtkWidget * , GdkEventKey*, gpointer );
    static  gboolean    motion_cb        ( GtkWidget*, GdkEventMotion*, gpointer );
    static  gboolean    timeout_cb       ( gpointer );
};

#endif
