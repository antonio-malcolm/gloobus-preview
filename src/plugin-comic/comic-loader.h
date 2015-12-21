#ifndef COMIC_LOADER_H
#define COMIC_LOADER_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <string>
#include <vector>

#include "../gloobus-preview-interface-main.h"
#include "../document-loader.h"

class ComicLoader : public DocumentLoader
{
    typedef std::vector<std::string> ComicList;

    private:
        ComicList comic_pages_;
        gchar *comic_path_;
        std::string list_cmd_;
        std::string extract_cmd_;

    public:
        ComicLoader ();
        virtual ~ComicLoader ();

        virtual DocumentState loadDocument( GFile *file );
        virtual int getPagesNumber();
        virtual void getPageSize( int index, int *width, int *height );
        virtual cairo_surface_t *getPage( int index, double scale );

    private:
        bool checkFilename( const char *filename );
        GdkPixbuf * getPagePixbuf( int index, double scale );
        char** extractArgv (gint page);
};

#endif /* DJVU_LOADER_H */
