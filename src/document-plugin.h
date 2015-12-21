#ifndef DOCUMENT_PLUGIN_H
#define DOCUMENT_PLUGIN_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include "document-loader.h"
#include "document-page.h"
#include "gloobus-preview-interface-main.h"

typedef boost::shared_ptr<DocumentLoader> DocumentLoaderPtr;

class DocumentPlugin : public interface
{
    typedef std::vector<DocumentPage*> PagesList;

    private:
        DocumentLoaderPtr loader_;
        DocumentState state_;

        int max_width_;
        int max_height_;

        int total_height_;

        double scale_;

        int active_tasks_;

        GAsyncQueue *load_queue_;
        GAsyncQueue *done_queue_;
        int loading_counter_;
        guint loading_id_;

        PagesList pages_;

        std::string file_info_string_;

        GtkAdjustment *vAdjustment_;

    public:
        DocumentPlugin(DocumentLoader *loader);
        virtual ~DocumentPlugin();

        virtual int get_width();
        virtual int get_height();
        virtual bool load();
        virtual void end();
        virtual void draw( GtkContainer * container );
        virtual bool key_press_event( int );

    private:
        void loadPage(int page);
        void preloadPages(int current);
        void unloadPages(int current);
        void setPageInfo(int p);

        static void loadPages(DocumentLoaderPtr, GAsyncQueue*, GAsyncQueue*);

        gboolean updatePages();
        static gboolean updatePagesCb(gpointer);

        void checkScrollbar (GtkAdjustment *);
        static void checkScrollbarCb (GtkAdjustment*, gpointer);

        void drawDocumentLayout( GtkContainer * container );
        void drawPasswordForm( GtkContainer * container );
        static void checkPassword(GtkButton*, gpointer);
};

#endif /* DOCUMENT_PLUGIN_H */