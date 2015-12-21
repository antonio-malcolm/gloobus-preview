#ifndef OFFICE_LOADER_H
#define OFFICE_LOADER_H

#include <poppler.h>
#include <string>

#include "pdf-loader.h"

class OfficeLoader : public PdfLoader
{
    private:
        GFile *pdf_file_;

    public:
        OfficeLoader ();
        virtual ~OfficeLoader ();

        virtual DocumentState loadDocument( GFile *file );

    private:
        gchar * getPdfUri (GFile *file);
};

#endif /* OFFICE_LOADER_H */