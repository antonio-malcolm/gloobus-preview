#include "pdf-loader.h"

#include "../gloobus-preview-defines.h"

PdfLoader::PdfLoader ():pdf_document_(NULL) {}

PdfLoader::~PdfLoader ()
{
    if (pdf_document_) {
        g_object_unref(pdf_document_);
    }
}

DocumentState PdfLoader::loadDocument( GFile *file )
{
    char *uri = g_file_get_uri( file );
    if(!uri) {
        return DOCUMENT_INVALID;
    }

    GError *error = NULL;
    DocumentState state = DOCUMENT_NONE;

    pdf_document_ = poppler_document_new_from_file (uri, getPassword(), &error);

    if ( !pdf_document_ ) {
        if (error != NULL) {
            if (POPPLER_ERROR == error->domain) {
                if (POPPLER_ERROR_INVALID == error->code) {
                    state = DOCUMENT_INVALID;
                }
                else if (POPPLER_ERROR_ENCRYPTED == error->code) {
                    state = DOCUMENT_ENCRYPTED;
                }
                else {
                    state = DOCUMENT_INVALID;
                }
            }
            g_error_free(error);
        }
        else {
            state = DOCUMENT_INVALID;
        }
        //set_error( g_error_new( g_quark_from_string ("PdfLoader"), 501,
        //                                             _("Error loading PDF file")) );
        //TODO: error communication in some way?
    }
    else {
        state = DOCUMENT_LOADED;
    }

    g_free(uri);

    return state;
}

int PdfLoader::getPagesNumber (void)
{
    return poppler_document_get_n_pages(pdf_document_);
}

void PdfLoader::getPageSize (int index, int *width, int *height)
{
    PopplerPage *page = poppler_document_get_page (pdf_document_, index);

    double page_width, page_height;
    poppler_page_get_size (page, &page_width, &page_height);

    if (width) {
        *width = page_width;
    }
    if (height) {
        *height = page_height;
    }

    g_object_unref(page);
}

cairo_surface_t * PdfLoader::getPage(int index, double scale)
{
    PopplerPage     *page;
    gdouble          page_width, page_height;
    gdouble          width, height;
    cairo_t         *cr;
    cairo_surface_t *surface;

    page = poppler_document_get_page (pdf_document_, index);
    if (!page) {
        return NULL;
    }

    poppler_page_get_size (page, &page_width, &page_height);
    width = page_width * scale;
    height = page_height * scale;

    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create (surface);

    cairo_save (cr);

    if (scale != 1.0) {
        cairo_scale (cr, scale, scale);
    }
    poppler_page_render (page, cr);

    cairo_restore (cr);

    cairo_set_operator (cr, CAIRO_OPERATOR_DEST_OVER);
    cairo_set_source_rgb (cr, 1., 1., 1.);
    cairo_paint (cr);

    cairo_destroy (cr);
    g_object_unref(page);

    return surface;
}
