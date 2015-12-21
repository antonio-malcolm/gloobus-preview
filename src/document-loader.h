#ifndef DOCUMENT_LOADER_H
#define DOCUMENT_LOADER_H

#include <cairo.h>
#include <gio/gio.h>
#include <glib.h>

enum DocumentState
{
    DOCUMENT_NONE,
    DOCUMENT_LOADED,
    DOCUMENT_INVALID,
    DOCUMENT_ENCRYPTED
};

class DocumentLoader
{
    private:
        gint active_;
        gint current_page_;
        gchar *password_;

    public:
        DocumentLoader():active_(1), current_page_(0), password_(NULL) {}
        virtual ~DocumentLoader() { if (password_) g_free(password_); }

        void setActive(bool active) { g_atomic_int_set(&active_, (gint)active); }
        bool isActive() { return g_atomic_int_get(&active_) != 0; }

        void setCurrentPage(gint page) { g_atomic_int_set(&current_page_, page); }
        gint getCurrentPage() { return g_atomic_int_get(&current_page_); }

        void setPassword( const gchar *password) { password_ = g_strdup(password); };
        const gchar * getPassword() const { return password_; };

        virtual DocumentState loadDocument( GFile *file )=0;
        virtual int getPagesNumber()=0;
        virtual void getPageSize( int index, int *width, int *height )=0;
        virtual cairo_surface_t *getPage( int index, double scale )=0;
};

#endif /* DOCUMENT_LOADER_H */