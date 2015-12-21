#ifndef XPS_LOADER_H
#define XPS_LOADER_H

#include <libgxps/gxps.h>

#include "../document-loader.h"

class XpsLoader : public DocumentLoader
{
    private:
        GXPSDocument *xps_document_;

    public:
        XpsLoader ();
        virtual ~XpsLoader ();

        virtual DocumentState loadDocument( GFile *file );
        virtual int getPagesNumber();
        virtual void getPageSize( int index, int *width, int *height );
        virtual cairo_surface_t *getPage( int index, double scale );
};

#endif /* XPS_LOADER_H */
