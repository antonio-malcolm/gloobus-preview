#ifndef _GLOOBUSWIN_
#define _GLOOBUSWIN_


#include <gtk/gtk.h>
#include <string>
#include <cairo.h>
#include <config.h>

#include "gloobus-preview-config.h"
#include "gloobus-preview-defines.h"
#include "gloobus-preview-theme.h"
#include "gloobus-preview-toolbar.h"

using namespace std;

class GloobusUI {
    private:
        GloobusConfig   *   config;
        GloobusToolbar  *   floating_toolbar;
        GloobusTheme    *   theme;
        bool                fullscreen;
        GtkWidget       *   window;
        int                 width;
        int                 height;
        int                 add_width;
        int                 add_height;
        int                 screen_width;
        int                 screen_height;
        cairo_surface_t *   background;
        GtkWidget       *   container;
        GtkWidget       *   frame;
        GtkWidget       *   header;
        GtkWidget       *   toolbar;
        GtkWidget       *   toolbar_info;
        GtkWidget       *   title;
        GtkWidget       *   btn_close;
        GtkWidget       *   btn_menu;
        GtkWidget       *   btn_title;
        GtkWidget       *   btn_fullscreen;
        GtkWidget       *   btn_defaultapp;
        //bool              controls_visible;
        GtkWidget       *   menu;

    public:
                            GloobusUI           ( void );
                            ~GloobusUI          ( void );

            void            prepare_window      ( void );
            void            resize              ( int w , int h );
            void            set_fullscreen      ( bool full );
            void            set_title           ( const gchar *str );
            GtkWidget*      get_container       ( void );
            GtkWidget*      get_toolbar         ( void );
            GtkWidget*      get_floating_toolbar( void );
            GtkWidget*      get_window          ( void );
            int             get_max_width       ( void );
            int             get_max_height      ( bool with_toolbar=false );
            int             get_screen_width    ( void );
            int             get_screen_height   ( void );
            GloobusTheme*   get_theme           ( void );
            GdkPixbuf*      get_file_icon       ( GFileInfo *info , int size );
            bool            is_fullscreen       ( void );
            void            create_about        ( void );
            GtkWidget*      create_button       ( const gchar* name , int size );
            GdkPixbuf*      create_pixbuf       ( const string &type , int w , int h ); //remove this
            GtkWidget*      create_title        ( void );
            GtkWidget*      create_properties   ( const string &text , int max_width ); //remove this
            //void          clean_bg            ( cairo_t * cr , int x , int y, int w, int h, int pos_x, int pos_y );
            GdkPixbuf*      pixbuf_reflect      ( const GdkPixbuf *pixbuf );
            GdkPixbuf*      pixbuf_scale        ( GdkPixbuf *pixbuf, int max_w, int max_h , bool upscale=false );
    //static    gboolean        fullscreen_cb       ( GtkWidget*, GdkEventButton*, gpointer);
    //static    gboolean        defaultapp_cb       ( GtkWidget*, GdkEventButton*, gpointer);

            GtkWidget*      add_menu_item       ( string, GCallback, gpointer);
            void            create_menu         ();

    private:
            void            create_header       ( void );
            void            create_window       ( void );
            void            draw_background     ( int , int );

    static  gboolean        draw_cb         ( GtkWidget*, cairo_t*, gpointer );
    static  gboolean        btn_close_cb        ( GtkWidget*, GdkEventButton*, gpointer );
    static  gboolean        btn_menu_cb         ( GtkWidget*, GdkEventButton*, gpointer );
    static  gboolean        btn_title_cb        ( GtkWidget*, GdkEventButton*, gpointer );
    static  void            menu_item_cb        ( GtkMenuItem*, gpointer );
    static  gboolean        focus_out_cb        ( GtkWidget*, GdkEventFocus*, gpointer );
    static  gboolean        configure_cb        ( GtkWidget*, GdkEventConfigure*, gpointer );
    static  gboolean        button_press_cb     ( GtkWidget*, GdkEventButton*, gpointer );
};

#endif
