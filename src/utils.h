#include <cairo.h>
#include <glib.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

class Utils
{
    public:
        static bool isProgramAvailable( const char * prog) {
            char * str;
            str = g_find_program_in_path (prog);
            if (str != NULL) {
                g_free (str);
                return true;
            }
            return false;
        }

        static cairo_surface_t * surfaceFromPixbuf (GdkPixbuf *pixbuf) {
            cairo_surface_t *surface;
            cairo_t         *cr;

            g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), NULL);

            surface = cairo_image_surface_create (gdk_pixbuf_get_has_alpha (pixbuf) ?
                                                    CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24,
                                                  gdk_pixbuf_get_width (pixbuf),
                                                  gdk_pixbuf_get_height (pixbuf));
            cr = cairo_create (surface);
            gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);

            return surface;
        }
};