#ifndef DJVU_LOADER_H
#define DJVU_LOADER_H

#include <libdjvu/ddjvuapi.h>

#include "../document-loader.h"

class DjvuLoader : public DocumentLoader
{
    private:
        ddjvu_context_t *djvu_context_;
        ddjvu_document_t *djvu_document_;
        ddjvu_format_t *djvu_format_;

    public:
        DjvuLoader ();
        virtual ~DjvuLoader ();

        virtual DocumentState loadDocument( GFile *file );
        virtual int getPagesNumber();
        virtual void getPageSize( int index, int *width, int *height );
        virtual cairo_surface_t *getPage( int index, double scale );

    private:
        void waitForMessage( ddjvu_message_tag_t, GError** );
        void handleMessage( const ddjvu_message_t*, GError** );
        void handleEvents( int, GError** );
};

#endif /* DJVU_LOADER_H */