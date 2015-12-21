#ifndef PS_LOADER_H
#define PS_LOADER_H

#include <libspectre/spectre.h>

#include "../document-loader.h"

class PsLoader : public DocumentLoader
{
    private:
        SpectreDocument *ps_document_;

    public:
        PsLoader ();
        virtual ~PsLoader ();

        virtual DocumentState loadDocument( GFile *file );
        virtual int getPagesNumber();
        virtual void getPageSize( int index, int *width, int *height );
        virtual cairo_surface_t *getPage( int index, double scale );
};

#endif /* PS_LOADER_H */