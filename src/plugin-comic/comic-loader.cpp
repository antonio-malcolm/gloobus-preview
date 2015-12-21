#include "comic-loader.h"

#include <algorithm>
#include <glib.h>
#include <sys/wait.h>

#include "../gloobus-preview-defines.h"
#include "../utils.h"

static const char* _ATTRIBUTES =
    G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE
;

static bool compareFilenames(std::string a, std::string b)
{
    return g_utf8_collate (a.c_str(), b.c_str()) < 0;
}

static void areaPreparedCb (GdkPixbufLoader *loader, gpointer data)
{
    gboolean *got_size = static_cast<gboolean*>(data);
    *got_size = true;
}

static void
sizePreparedCb (GdkPixbufLoader *loader,
                gint             width,
                gint             height,
                gpointer         data)
{
    double *scale = static_cast<double*>(data);
    int w = (width  * (*scale) + 0.5);
    int h = (height * (*scale) + 0.5);

    gdk_pixbuf_loader_set_size (loader, w, h);
}

ComicLoader::ComicLoader ():comic_path_(NULL) {}

ComicLoader::~ComicLoader ()
{
    if (comic_path_) {
        g_free(comic_path_);
    }
}

DocumentState ComicLoader::loadDocument( GFile *file )
{
    gchar *path = g_file_get_path (file);
    if (!path) {
        g_debug("ComicLoader: failed retrieving comic path");
        return DOCUMENT_INVALID;
    }

    comic_path_ = g_shell_quote (path);
    if (!comic_path_) {
        g_debug("ComicLoader: failed quoting comic path");
        g_free(path);
        return DOCUMENT_INVALID;
    }

    GFileInfo *info = g_file_query_info(file,
                                        _ATTRIBUTES,
                                        G_FILE_QUERY_INFO_NONE,
                                        NULL,
                                        NULL);

    const gchar *type = g_file_info_get_content_type (info);

    //1.Depending on mime define list_command and extract_command
    if (interface::is_program_available("bsdtar")) {
        list_cmd_ = "bsdtar -tf";
        extract_cmd_ = "bsdtar -xOf";
    }
    else if(!g_strcmp0 ("application/x-cbr", type)) {
        list_cmd_ = "unrar vb -c- -- ";
        extract_cmd_ = "unrar p -c- -ierr";
    }
    else if(!g_strcmp0 ("application/x-cbt", type)) {
        list_cmd_ = "tar -tf";
        extract_cmd_ = "tar -xOf";
    }
    else if(!g_strcmp0 ("application/x-cbz", type)) {
        list_cmd_ = "zipinfo -1 -- ";
        extract_cmd_ = "unzip -p -C";
    }
    else {
        g_debug("ComicLoader: wrong mime type??");
        g_assert_not_reached();
    }

    list_cmd_.append(comic_path_);

    g_free(path);

    //2.Get names of the files in the cbz/cbr sorted! (When they are image files)
    gint exit_status;
    gchar *std_out = 0;
    gboolean success = g_spawn_command_line_sync (list_cmd_.c_str(),
                                                  &std_out,
                                                  NULL,
                                                  &exit_status,
                                                  NULL);

    if (!success) {
        return DOCUMENT_INVALID;
    }
    else if (!WIFEXITED(exit_status) || WEXITSTATUS(exit_status) != EXIT_SUCCESS) {
        //g_free (std_out);
        //set_error( g_error_new(g_quark_from_string ("iComic"), 201,
        //    _("Error listing comic book files")) );
        g_debug("ComicLoader: error listing comic book files");
        return DOCUMENT_INVALID;
    }

    gchar **files = g_strsplit (std_out, "\n", 0);
    g_free (std_out);

    if (!files) {
        g_debug("ComicLoader: no files in archive");
        return DOCUMENT_INVALID;
    }

    for (int i = 0; files[i] != NULL; i++) {
        if(checkFilename(files[i])) {
            comic_pages_.push_back(files[i]);
        }
    }

    g_strfreev(files);

    if (comic_pages_.empty()) {
        g_debug("ComicLoader: no image files in archive");
        return DOCUMENT_INVALID;
    }

    std::sort( comic_pages_.begin(), comic_pages_.end(), compareFilenames );

    return DOCUMENT_LOADED;
}

int ComicLoader::getPagesNumber (void)
{
    return comic_pages_.size();
}

void ComicLoader::getPageSize (int index, int *width, int *height)
{
    GdkPixbufLoader *loader;
    char **argv;
    guchar buf[1024];
    gboolean success, got_size = FALSE;
    gint outpipe = -1;
    GPid child_pid;
    gssize bytes;
    GdkPixbuf *pixbuf;
    gchar *filename;

    argv = extractArgv (index);
    success = g_spawn_async_with_pipes (NULL, argv, NULL,
                                        (GSpawnFlags)(G_SPAWN_SEARCH_PATH |
                                                      G_SPAWN_STDERR_TO_DEV_NULL),
                                        NULL, NULL,
                                        &child_pid,
                                        NULL, &outpipe, NULL, NULL);
    g_strfreev (argv);
    g_return_if_fail (success == TRUE);

    loader = gdk_pixbuf_loader_new ();
    g_signal_connect (loader,
                      "area-prepared",
                      G_CALLBACK (areaPreparedCb),
                      &got_size);

    while (outpipe >= 0) {
        bytes = read (outpipe, buf, 1024);

        if (bytes > 0) {
            gdk_pixbuf_loader_write (loader, buf, bytes, NULL);
        }
        if (bytes <= 0 || got_size) {
            close (outpipe);
            outpipe = -1;
            gdk_pixbuf_loader_close (loader, NULL);
        }
    }

    pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
    if (pixbuf) {
        if (width) {
            *width = gdk_pixbuf_get_width (pixbuf);
        }
        if (height) {
            *height = gdk_pixbuf_get_height (pixbuf);
        }
    }

    g_spawn_close_pid (child_pid);
    g_object_unref (loader);
}

cairo_surface_t * ComicLoader::getPage(int index, double scale)
{
    GdkPixbuf *pixbuf = getPagePixbuf (index, scale);
    cairo_surface_t *surface = Utils::surfaceFromPixbuf (pixbuf);
    g_object_unref (pixbuf);

    return surface;
}

bool ComicLoader::checkFilename( const gchar *filename )
{
    std::string temp(filename);

    if(temp.find(".jpg")!= std::string::npos) return true;
    if(temp.find(".JPG")!= std::string::npos) return true;
    if(temp.find(".jpeg")!= std::string::npos) return true;
    if(temp.find(".JPEG")!= std::string::npos) return true;
    if(temp.find(".png")!= std::string::npos) return true;
    if(temp.find(".PNG")!= std::string::npos) return true;

    return false;
}

GdkPixbuf* ComicLoader::getPagePixbuf ( int index , double scale)
{
    GdkPixbufLoader *loader;
    GdkPixbuf *pixbuf;
    char **argv;
    guchar buf[4096];
    gboolean success;
    gint outpipe = -1;
    GPid child_pid;
    gssize bytes;
    gint width, height;
    gchar *filename;

    argv = extractArgv (index);
    success = g_spawn_async_with_pipes (NULL, argv, NULL,
                                        (GSpawnFlags)(G_SPAWN_SEARCH_PATH |
                                                      G_SPAWN_STDERR_TO_DEV_NULL),
                                        NULL, NULL,
                                        &child_pid,
                                        NULL, &outpipe, NULL, NULL);
    g_strfreev (argv);
    g_return_val_if_fail (success == TRUE, NULL);

    loader = gdk_pixbuf_loader_new ();
    g_signal_connect (loader, "size-prepared",
                      G_CALLBACK (sizePreparedCb),
                      &scale);

    while (outpipe >= 0) {
        bytes = read (outpipe, buf, 4096);

        if (bytes > 0) {
            gdk_pixbuf_loader_write (loader, buf, bytes, NULL);
        } else if (bytes <= 0) {
            close (outpipe);
            gdk_pixbuf_loader_close (loader, NULL);
            outpipe = -1;
        }
    }

    //rotated_pixbuf = gdk_pixbuf_rotate_simple (
    //            gdk_pixbuf_loader_get_pixbuf (loader),
    //                360 - rc->rotation);

    pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
    g_object_ref(pixbuf);

    g_spawn_close_pid (child_pid);
    g_object_unref (loader);

    return pixbuf;
}

char** ComicLoader::extractArgv (gint page)
{
    char **argv;
    char *command_line, *quoted_filename;
    GError *err = NULL;

    quoted_filename = g_shell_quote (comic_pages_[page].c_str());

    command_line = g_strdup_printf ("%s %s %s",
                    extract_cmd_.c_str(),
                    comic_path_,
                    quoted_filename);

    g_shell_parse_argv (command_line, NULL, &argv, &err);

    g_free (command_line);
    g_free (quoted_filename);

    if (err) {
        g_warning (_("Error %s"), err->message);
        g_error_free (err);
        return NULL;
    }

    return argv;
}
