#include "djvu-loader.h"

#include "../gloobus-preview-defines.h"

#define SCALE_FACTOR 0.2

DjvuLoader::DjvuLoader ():djvu_document_(NULL) {}

DjvuLoader::~DjvuLoader ()
{
    if(djvu_context_) {
        ddjvu_context_release(djvu_context_);
    }
    if (djvu_document_) {
        ddjvu_document_release(djvu_document_);
    }
    if(djvu_format_) {
        ddjvu_format_release(djvu_format_);
    }
}

DocumentState DjvuLoader::loadDocument( GFile *file )
{
    guint masks[4] = { 0xff0000, 0xff00, 0xff, 0xff000000 };

    djvu_context_ = ddjvu_context_create ("Gloobus-Preview");
    djvu_format_ = ddjvu_format_create (DDJVU_FORMAT_RGBMASK32, 4, masks);
    ddjvu_format_set_row_order (djvu_format_, 1);

    GError **error = NULL;
    GError *djvu_error = NULL;

    gchar *uri = g_file_get_uri(file);
    if (!uri) {
        return DOCUMENT_INVALID;
    }
    gchar *filename = g_filename_from_uri (uri, NULL, NULL);

    if (!filename) {
        //set_error( g_error_new(g_quark_from_string ("iDjvu"), 301,
        //    _("Error getting filename")) );
        g_free(uri);
        return DOCUMENT_INVALID;
    }

    djvu_document_ = ddjvu_document_create_by_filename (djvu_context_, filename, TRUE);
    if (!djvu_document_) {
        //set_error( g_error_new(g_quark_from_string ("iDjvu"), 302,
        //    _("Document has incorrect format")) );
        g_free (uri);
        g_free (filename);
        return DOCUMENT_INVALID;
    }

    waitForMessage (DDJVU_DOCINFO, &djvu_error);
    if (djvu_error) {
        //set_error( g_error_new(g_quark_from_string ("iDjvu"), 303,
        //    "%s:\n%s", _("An error occurred"), djvu_error->message) );

        g_free (uri);
        g_free (filename);
        g_error_free (djvu_error);
        return DOCUMENT_INVALID;
    }

    bool missing_files = false;
    if (ddjvu_document_get_type (djvu_document_) == DDJVU_DOCTYPE_INDIRECT) {
        gchar *base = g_path_get_dirname (filename);
        gint n_files = ddjvu_document_get_filenum (djvu_document_);

        for ( gint i = 0; i < n_files; i++) {
            struct ddjvu_fileinfo_s fileinfo;
            gchar *fname;

            ddjvu_document_get_fileinfo (djvu_document_, i, &fileinfo);

            if (fileinfo.type != 'P') {
                continue;
            }

            fname = g_build_filename (base, fileinfo.id, NULL);
            if (!g_file_test (fname, G_FILE_TEST_EXISTS)) {
                missing_files = TRUE;
                g_free (fname);
                break;
            }
            g_free (fname);
        }
        g_free (base);
    }
    g_free (uri);
    g_free (filename);

    if (missing_files) {
        //set_error( g_error_new(g_quark_from_string ("iDjvu"), 304,
        //    _("The document is composed of several files. One or more of such files cannot be accessed")) );
        return DOCUMENT_INVALID;
    }

    return DOCUMENT_LOADED;
}

int DjvuLoader::getPagesNumber (void)
{
    return ddjvu_document_get_pagenum (djvu_document_);
}

void DjvuLoader::getPageSize (int index, int *width, int *height)
{
    ddjvu_pageinfo_t info;
    ddjvu_status_t r;

    while ((r = ddjvu_document_get_pageinfo(djvu_document_, index, &info)) < DDJVU_JOB_OK) {
        handleEvents(TRUE, NULL);
    }

    if (r >= DDJVU_JOB_FAILED) {
        handleEvents(TRUE, NULL);
    }

    if (width) {
        *width = info.width * SCALE_FACTOR;
    }
    if (height) {
        *height = info.height * SCALE_FACTOR;
    }
}

cairo_surface_t * DjvuLoader::getPage(int index, double scale)
{
    cairo_surface_t *surface;
    gchar           *pixels;
    gint             rowstride;
    ddjvu_rect_t     rrect;
    ddjvu_rect_t     prect;
    ddjvu_page_t    *d_page;
    //ddjvu_page_rotation_t rotation;
    double           page_width, page_height, tmp;

    d_page = ddjvu_page_create_by_pageno (djvu_document_, index);

    while (!ddjvu_page_decoding_done (d_page)) {
        handleEvents(TRUE, NULL);
    }

    page_width = ddjvu_page_get_width (d_page) * scale * SCALE_FACTOR + 0.5;
    page_height = ddjvu_page_get_height (d_page) * scale * SCALE_FACTOR + 0.5;

    /*switch (rc->rotation) {
            case 90:
            rotation = DDJVU_ROTATE_90;
            tmp = page_height;
            page_height = page_width;
            page_width = tmp;

            break;
            case 180:
            rotation = DDJVU_ROTATE_180;

            break;
            case 270:
            rotation = DDJVU_ROTATE_270;
            tmp = page_height;
            page_height = page_width;
            page_width = tmp;

            break;
            default:
            rotation = DDJVU_ROTATE_0;
    }*/

    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,
                                          page_width,
                                          page_height);

    rowstride = cairo_image_surface_get_stride (surface);
    pixels = (gchar *)cairo_image_surface_get_data (surface);

    prect.x = 0;
    prect.y = 0;
    prect.w = page_width;
    prect.h = page_height;
    rrect = prect;

    //ddjvu_page_set_rotation (d_page, rotation);
    ddjvu_page_set_rotation (d_page, DDJVU_ROTATE_0);

    ddjvu_page_render (d_page, DDJVU_RENDER_COLOR,
               &prect,
               &rrect,
               djvu_format_,
               rowstride,
               pixels);

    cairo_surface_mark_dirty (surface);

    ddjvu_page_release(d_page);

    return surface;
}

void DjvuLoader::waitForMessage (ddjvu_message_tag_t message, GError **error)
{
    const ddjvu_message_t *msg;

    ddjvu_message_wait (djvu_context_);
    while ((msg = ddjvu_message_peek (djvu_context_)) && (msg->m_any.tag != message)) {
        handleMessage (msg, error);
        ddjvu_message_pop (djvu_context_);
        if (error && *error) {
            return;
        }
    }
    if (msg && msg->m_any.tag == message) {
        ddjvu_message_pop (djvu_context_);
    }
}

void DjvuLoader::handleMessage (const ddjvu_message_t *msg, GError **error)
{
    switch (msg->m_any.tag) {
        case DDJVU_ERROR: {
            gchar *error_str;

            if (msg->m_error.filename) {
                error_str = g_strdup_printf ("DjvuLibre error: %s:%d",
                                 msg->m_error.filename,
                                 msg->m_error.lineno);
            } else {
                error_str = g_strdup_printf ("DjvuLibre error: %s",
                                 msg->m_error.message);
            }

            //if (error) {
            //  g_set_error_literal (error, EV_DJVU_ERROR, 0, error_str);
            //} else {
                g_warning ("%s", error_str);
            //}

            g_free (error_str);
            return;
        }
            break;
        default:
            break;
    }
}

void DjvuLoader::handleEvents (int wait, GError **error)
{
    const ddjvu_message_t *msg;

    if (!djvu_context_) {
        return;
    }

    if (wait) {
        ddjvu_message_wait (djvu_context_);
    }

    while ((msg = ddjvu_message_peek (djvu_context_))) {
        handleMessage (msg, error);
        ddjvu_message_pop (djvu_context_);
        if (error && *error) {
            return;
        }
    }
}