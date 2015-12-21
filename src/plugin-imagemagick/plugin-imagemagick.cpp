#include "plugin-imagemagick.h"

#include <cstdlib>
#include <sstream>
#include <glib/gstdio.h>
#include <gio/gio.h>
#include <string>
#include <cstring>

#include "../utils.h"

#define IMAGE_MAX_SIZE 10485760 // 10Mb

static gchar * getTempPath( GFile *gfile );

GdkPixbuf * iImagemagick::get_pixbuf ()
{
    GdkPixbuf * temp;
    gchar *path = getPngUri(m_gfile);
    if (NULL != path) {
        g_debug("PLUGIN:Filename to load: %s", path);
        temp = gdk_pixbuf_new_from_file (path, NULL);
        g_unlink(path);
        g_free (path);
        return temp;
    }
}

gchar * iImagemagick::getPngUri( GFile *file ) {
    g_debug("Trying to convert to PNG");

    gchar *tmp = getTempPath(file);
    if(g_file_test(tmp, G_FILE_TEST_EXISTS)) {
        return tmp;
    }

    GFileInfo *info = g_file_query_info(file,
                                        "*",
                                        G_FILE_QUERY_INFO_NONE,
                                        NULL,
                                        NULL);


    if(g_file_info_get_size(info) > IMAGE_MAX_SIZE) {
        g_free(tmp);
        return NULL;
    }

    if (Utils::isProgramAvailable("convert")) {
        const gchar *type = g_file_info_get_content_type (info);
        gchar *path = g_file_get_path(file);
        gchar *cmd = g_strdup_printf("convert \"%s\"[0] \"%s\"", path, tmp);

        if( system(cmd) != 0) {
            g_warning("Unable to convert file");
            tmp = NULL;
        }

        g_free(path);
        g_free(cmd);
    }

    return tmp;
}

static gchar * getTempPath( GFile *gfile )
{
    gchar * path = g_file_get_path(gfile);
    char * hash = g_compute_checksum_for_data(G_CHECKSUM_SHA256,
                                              (const guchar*)path,
                                              strlen(path));

    gchar *result = g_strdup_printf("/tmp/gloobus.%s.png", hash);
    g_free(hash);
    g_debug("Temp path: %s", result);
    return result;
}
