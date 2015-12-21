#include "xps-loader.h"

#include "../gloobus-preview-defines.h"

XpsLoader::XpsLoader ():xps_document_(NULL) {}

XpsLoader::~XpsLoader ()
{
    if (xps_document_) {
        g_object_unref(xps_document_);
    }
}

DocumentState XpsLoader::loadDocument( GFile *file )
{
    GError *error = NULL;
    DocumentState state = DOCUMENT_NONE;

    GXPSFile * xps_file = gxps_file_new (file, &error);
    if (!xps_file) {
        g_warning ("Error opening XPS file: %s\n", error->message);
        g_error_free (error);
        return DOCUMENT_INVALID;
    }

    xps_document_ = gxps_file_get_document (xps_file, 0, &error);
    if (!xps_document_) {
        g_warning ("Error loading XPS document: %s\n", error->message);
        g_error_free (error);
        state = DOCUMENT_INVALID;
    } else {
        state = DOCUMENT_LOADED;
    }

    return state;
}

int XpsLoader::getPagesNumber (void)
{
    return gxps_document_get_n_pages(xps_document_);
}

void XpsLoader::getPageSize (int index, int *width, int *height)
{
    GError          *error = NULL;
    GXPSPage *page = gxps_document_get_page (xps_document_, index, &error);
    if (!page) {
        g_warning ("Error loading page %d: %s\n", index, error->message);
        g_error_free (error);
        return;
    }

    double page_width, page_height;
    gxps_page_get_size (page, &page_width, &page_height);

    if (width) {
        *width = page_width;
    }
    if (height) {
        *height = page_height;
    }

    g_object_unref(page);
}

cairo_surface_t * XpsLoader::getPage(int index, double scale)
{
    GXPSPage        *page;
    gdouble          page_width, page_height;
    gdouble          width, height;
    cairo_t         *cr;
    cairo_surface_t *surface;
    GError          *error = NULL;

    page = gxps_document_get_page (xps_document_, index, &error);
    if (!page) {
        g_warning ("Error loading page %d: %s\n", index, error->message);
        g_error_free (error);
        return NULL;
    }

    gxps_page_get_size (page, &page_width, &page_height);
    width = page_width * scale;
    height = page_height * scale;

    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create (surface);

    if (scale != 1.0) {
        cairo_scale (cr, scale, scale);
    }
    cairo_set_source_rgb (cr, 1., 1., 1.);
    cairo_paint (cr);

    gxps_page_render (page, cr, &error);

    cairo_destroy (cr);
    g_object_unref(page);

    if (error) {
        g_warning ("Error rendering page %d: %s\n", index, error->message);
        g_error_free (error);
    }

    return surface;
}
