#ifndef PDF_LOADER_H
#define PDF_LOADER_H

#include <poppler.h>

#include "../document-loader.h"

class PdfLoader : public DocumentLoader
{
    private:
        PopplerDocument *pdf_document_;

    public:
        PdfLoader ();
        virtual ~PdfLoader ();

        virtual DocumentState loadDocument( GFile *file );
        virtual int getPagesNumber();
        virtual void getPageSize( int index, int *width, int *height );
        virtual cairo_surface_t *getPage( int index, double scale );
};

#endif /* PDF_LOADER_H */