#include "ps-loader.h"

#include "../gloobus-preview-defines.h"

PsLoader::PsLoader ():ps_document_(NULL) {}

PsLoader::~PsLoader ()
{
    if (ps_document_) {
        spectre_document_free(ps_document_);
    }
}

DocumentState PsLoader::loadDocument( GFile *file )
{
    gchar *uri = g_file_get_uri (file);
    if (!uri) {
        return DOCUMENT_INVALID;
    }

    gchar *filename = g_filename_from_uri (uri, NULL, NULL);

    if (!filename) {
        //set_error( g_error_new(g_quark_from_string ("iPs"), 401,
        //    _("Error getting filename")) );
        g_free(uri);
        return DOCUMENT_INVALID;
    }

    ps_document_ = spectre_document_new ();
    spectre_document_load (ps_document_, filename);
    if (spectre_document_status (ps_document_)) {
        //set_error( g_error_new(g_quark_from_string ("iPs"), 402,
        //    _("Failed to open document")) );

        g_free(uri);
        g_free (filename);
        return DOCUMENT_INVALID;
    }

    g_free (uri);
    g_free (filename);

    return DOCUMENT_LOADED;
}

int PsLoader::getPagesNumber (void)
{
    return spectre_document_get_n_pages (ps_document_);
}

void PsLoader::getPageSize (int index, int *width, int *height)
{
    SpectrePage *page = spectre_document_get_page (ps_document_, index);
    spectre_page_get_size (page, width, height);
    spectre_page_free(page);
}

cairo_surface_t * PsLoader::getPage(int index, double scale)
{
    SpectrePage          *page;
    SpectreRenderContext *src;
    gint                  width_points;
    gint                  height_points;
    gint                  width, height;
    gint                  swidth, sheight;
    guchar               *data = NULL;
    gint                  stride;
    //gint                  rotation;
    cairo_surface_t      *surface;
    static const cairo_user_data_key_t key = {0};

    page = spectre_document_get_page (ps_document_, index);

    spectre_page_get_size (page, &width_points, &height_points);

    width = (gint) ((width_points * scale) + 0.5);
    height = (gint) ((height_points * scale) + 0.5);
    //rotation = (rc->rotation + get_page_rotation (ps_page)) % 360;

    src = spectre_render_context_new ();
    spectre_render_context_set_scale (src,
                                      (gdouble)width / width_points,
                                      (gdouble)height / height_points);
    //spectre_render_context_set_rotation (src, rotation);
    spectre_page_render (page, src, &data, &stride);
    spectre_render_context_free (src);

    if (!data) {
        return NULL;
    }

    if (spectre_page_status (page)) {
        g_warning ("%s", spectre_status_to_string (spectre_page_status (page)));
        g_free (data);

        return NULL;
    }

    //if (rotation == 90 || rotation == 270) {
    //    swidth = height;
    //    sheight = width;
    //} else {
        swidth = width;
        sheight = height;
    //}

    surface = cairo_image_surface_create_for_data (data,
                                                   CAIRO_FORMAT_RGB24,
                                                   swidth, sheight,
                                                   stride);
    cairo_surface_set_user_data (surface,
                                 &key,
                                 data,
                                 (cairo_destroy_func_t)g_free);
    return surface;
}