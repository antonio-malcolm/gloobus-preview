#ifndef _ITTF_
#define _ITTF_

#ifndef PLUGIN_LOADER

#include <gdk/gdk.h>
#include <cairo.h>
#include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../gloobus-preview-interface-free.h"

class iTtf: public iFree
{
    private:
        GtkWidget           *   area;
        GtkWidget           *   menuItemInstallFont;
        cairo_font_face_t   *   font;


    public:
                        iTtf                ( void );
                        ~iTtf               ( void );
    virtual void        draw                ( GtkContainer * container);
    virtual int         get_width           ( void );
    virtual int         get_height          ( void );

    private:
            void        draw_toolbar        ( void );
            void        load_font           ( void );
    static  void        draw_cb             ( GtkWidget*, cairo_t*, iTtf* );
    static  void        install_font_cb     ( GtkWidget*, gpointer );
};

extern "C" interface * create() {
    return new iTtf;
}

extern "C" void destroy(interface * p) {
    delete (iTtf*)p;
}

extern "C" int getVersion() {
    return MANAGER_VERSION;
}

#endif

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("ttf");

PluginManager::register_filetype("application/x-font-ttf", 1);
PluginManager::register_filetype("application/x-font-otf", 1);
PluginManager::register_filetype("application/x-font-pcf", 1);

#endif

#endif
