#include "office-loader.h"

#include <cstdlib>
#include <sstream>
#include <glib/gstdio.h>
#include <gio/gio.h>
#include <string>
#include <cstring>

#include "../utils.h"

#define DOCUMENT_MAX_SIZE 10485760 // 10Mb

static gchar * getTempPath( GFile *gfile );
static gchar * pathToUri( std::string path );

OfficeLoader::OfficeLoader ():pdf_file_(NULL) {}

OfficeLoader::~OfficeLoader ()
{
    if (NULL != pdf_file_) {
        char *path = g_file_get_path (pdf_file_);
        g_object_unref(pdf_file_);
        g_unlink (path);
        g_free (path);
    }
}

DocumentState OfficeLoader::loadDocument( GFile *file ) {
    gchar *path = getPdfUri(file);
    if (NULL != path) {
        pdf_file_ = g_file_new_for_uri(path);
        g_free(path);

        if (pdf_file_) {
            return PdfLoader::loadDocument(pdf_file_);
        }
    }

    return DOCUMENT_INVALID;
}

gchar * OfficeLoader::getPdfUri( GFile *file) {
    g_debug("Trying to convert to PDF");

    gchar *uri = NULL;
    gchar *tmp = getTempPath(file);
    if(g_file_test(tmp, G_FILE_TEST_EXISTS)) {
        uri = pathToUri(tmp);
        g_free(tmp);
        return uri;
    }

    GFileInfo *info = g_file_query_info(file,
                                        "*",
                                        G_FILE_QUERY_INFO_NONE,
                                        NULL,
                                        NULL);


    if(g_file_info_get_size(info) > DOCUMENT_MAX_SIZE) {
        //set_error( g_error_new(g_quark_from_string ("iPdf"), 512,
        //    _("File is too big for convertion to PDF")) );
        //TODO: reporta the error in some way
        g_free(tmp);
        return NULL;
    }

    const gchar *type = g_file_info_get_content_type (info);
 
    if (Utils::isProgramAvailable("unoconv")) {
        gboolean go = g_strcmp0 ("application/x-gnumeric", type) != 0;
        if (go) {
            gchar *path = g_file_get_path(file);
            gchar *cmd = g_strdup_printf("unoconv -f pdf -o \"%s\" \"%s\"", tmp, path);

            if( system(cmd) == 0) {
                uri = pathToUri(tmp);
            }

            g_free(path);
            g_free(cmd);
        }
    }

    if (NULL == uri && Utils::isProgramAvailable("abiword")) {
        gboolean go = g_strcmp0 ("application/msword", type) == 0
                   || g_strcmp0 ("application/rtf", type) == 0
                   || g_strcmp0 ("application/x-abiword", type) == 0;
        if (go) {
            gchar *path = g_file_get_path(file);
            gchar *cmd = g_strdup_printf("abiword -t pdf -o \"%s\" \"%s\"", tmp, path);

            if( system(cmd) == 0) {
                uri = pathToUri(tmp);
            }

            g_free(path);
            g_free(cmd);
        }
    }

    if (NULL == uri && Utils::isProgramAvailable("ssconvert")) {
        gboolean go = g_strcmp0 ("application/vnd.ms-excel", type) == 0
                   || g_strcmp0 ("application/vnd.oasis.opendocument.spreadsheet", type) == 0
                   || g_strcmp0 ("application/vnd.oasis.opendocument.spreadsheet-template", type) == 0
                   || g_strcmp0 ("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", type) == 0
                   || g_strcmp0 ("application/x-gnumeric", type) == 0;
        if (go) {
            gchar *path = g_file_get_path(file);
            gchar *cmd = g_strdup_printf("ssconvert \"%s\" \"%s\"", path, tmp);

            if( system(cmd) == 0) {
                uri = pathToUri(tmp);
            }

            g_free(path);
            g_free(cmd);
        }
    }

    //if (NULL == uri) {
        //set_error( g_error_new(g_quark_from_string ("iPdf"), 511,
        //    _("Error converting file to PDF")) );
    //}

    g_free(tmp);
    return uri;
}

static gchar * getTempPath( GFile *gfile )
{
    gchar * path = g_file_get_path(gfile);
    char * hash = g_compute_checksum_for_data(G_CHECKSUM_SHA256,
                                              (const guchar*)path,
                                              strlen(path));

    gchar *result = g_strdup_printf("/tmp/gloobus.%s.pdf", hash);
    g_free(hash);
    g_debug("Temp path: %s", result);
    return result;
}

static gchar * pathToUri( std::string path )
{
    path = "file://" + path;
    return g_uri_escape_string(path.c_str(), G_URI_RESERVED_CHARS_ALLOWED_IN_PATH, false);
}
