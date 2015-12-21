#include "document-plugin.h"

#include <boost/bind.hpp>
#include <gdk/gdkkeysyms-compat.h>

#include "task-manager.h"

using boost::bind;

enum {
    PAGE_PRELOADING = 3,
    PAGE_SPACING = 5,
    PASSWORD_FORM_HEIGHT = 300,
    PASSWORD_FORM_WIDTH = 400
};

struct PageTask
{
    enum {
        NONE,
        LOADED,
        OUT_OF_RANGE,
        ERROR
    };

    int page;
    double scale;
    cairo_surface_t *surface;
    int state;

    PageTask(int p, double s):page(p), scale(s), surface(NULL), state(NONE) {}

    ~PageTask() {
        if (NULL != surface) {
            cairo_surface_destroy(surface);
        }
    }

    static void destroy(gpointer data) {
        delete static_cast<PageTask*>(data);
    }
};

struct PasswordData
{
    DocumentPlugin *document;
    GtkContainer *container;
    GtkWidget *entry;
    GtkWidget *label;
};

DocumentPlugin::DocumentPlugin(DocumentLoader *loader)
    :loader_(loader),
    state_(DOCUMENT_NONE),
    max_width_(0),
    max_height_(0),
    total_height_(0),
    scale_(1.0),
    active_tasks_(0),
    load_queue_(NULL),
    done_queue_(NULL),
    loading_counter_(0),
    loading_id_(0)
{
    g_assert(NULL != loader);
}

DocumentPlugin::~DocumentPlugin()
{
    if (load_queue_) {
        loader_->setActive(false);
        g_async_queue_push(load_queue_, new PageTask(-1, 0.0));
        g_async_queue_unref(load_queue_);
        g_async_queue_unref(done_queue_);
    }
    if (loading_id_) {
        g_source_remove(loading_id_);
    }
}

int DocumentPlugin::get_width()
{
    switch(state_) {
        case DOCUMENT_LOADED:
            return max_width_ * scale_ + 25; /* static 25 for scrollbars is baaad */
            break;
        case DOCUMENT_ENCRYPTED:
            return PASSWORD_FORM_WIDTH;
            break;
        default:
            g_assert_not_reached();
    }

    return 0;
}

int DocumentPlugin::get_height()
{
    switch(state_) {
        case DOCUMENT_LOADED:
            return total_height_ * scale_;
            break;
        case DOCUMENT_ENCRYPTED:
            return PASSWORD_FORM_HEIGHT;
            break;
        default:
            g_assert_not_reached();
    }

    return 0;
}

bool DocumentPlugin::load()
{
    state_ = loader_->loadDocument(m_gfile);
    switch(state_) {
        case DOCUMENT_LOADED: {
            int width, height;
            int num = loader_->getPagesNumber();
            pages_.resize(num);
            for (int i=0; i < num; i++) {
                loader_->getPageSize(i, &width, &height);
                if (width > max_width_) {
                    max_width_ = width;
                }
                if (height > max_height_) {
                    max_height_ = height;
                }
                total_height_ += height;

                DocumentPage *page = DOCUMENT_PAGE(document_page_new(width, height));

                pages_[i] = page;
            }

            double available_width = ui->get_max_width();

            if (available_width < max_width_) {
                scale_ = available_width / max_width_;
            }

            load_queue_ = g_async_queue_new_full (&PageTask::destroy);
            done_queue_ = g_async_queue_new_full (&PageTask::destroy);

            g_async_queue_ref(load_queue_); // for loadPages thread
            g_async_queue_ref(done_queue_); // for loadPages thread

            TaskManager::add(new Task(bind(&loadPages, loader_, load_queue_, done_queue_)));

            // lets preload a couple of first pages
            int stop = PAGE_PRELOADING;
            if (stop > (int)pages_.size()) {
                stop = pages_.size();
            }
            for (int i=0; i<stop; i++) {
                loadPage(i);
            }

            return true;
        }
        case DOCUMENT_ENCRYPTED:
            return true;
        default:
            break;
    }
    return false;
}

void DocumentPlugin::end()
{
}

static gboolean
cleanBackground(GtkWidget * widget, cairo_t *cr, gpointer data)
{
    //cairo_set_operator        ( cr, CAIRO_OPERATOR_CLEAR );
    //cairo_paint( cr );
    //cairo_set_operator        ( cr, CAIRO_OPERATOR_OVER );
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    return false;
}

void DocumentPlugin::draw( GtkContainer * container )
{
    if (DOCUMENT_LOADED == state_) {
        drawDocumentLayout(container);
    }
    else if (DOCUMENT_ENCRYPTED == state_) {
        drawPasswordForm(container);
    }
    else {
        g_assert_not_reached();
    }

    draw_toolbar();
}

bool DocumentPlugin::key_press_event( int keyval ) {
    if( keyval == GDK_Page_Up ) {
        gdouble value = gtk_adjustment_get_value(vAdjustment_);
        value -= gtk_adjustment_get_page_size(vAdjustment_);
        gtk_adjustment_set_value(vAdjustment_, value);
        return true;
    }
    else if( keyval == GDK_Page_Down ) {
        gdouble value = gtk_adjustment_get_value(vAdjustment_);
        value += gtk_adjustment_get_page_size(vAdjustment_);
        gtk_adjustment_set_value(vAdjustment_, value);
        return true;
    }
    return false;
}

void DocumentPlugin::loadPage(int page)
{
    if (!document_page_is_loading(pages_[page])) {
        g_async_queue_push(load_queue_, new PageTask(page, scale_));
        loading_counter_++;
        if (loading_id_ == 0) {
            loading_id_ = g_timeout_add(250, &updatePagesCb, this);
        }
        document_page_set_loading(pages_[page], true);
    }
}

void DocumentPlugin::preloadPages(int current)
{
    int start = current - PAGE_PRELOADING;
    if (start < 0) {
        start = 0;
    }
    int end = current + PAGE_PRELOADING;
    if (end > (int)pages_.size()) {
        end = pages_.size();
    }
    for (; start < end; start++) {
        if (!document_page_is_loading(pages_[start])) {
            loadPage(start);
        }
    }
}

void DocumentPlugin::unloadPages(int current)
{
    int upper = current - PAGE_PRELOADING;
    for (int page=0; page < upper; page++) {
        document_page_set_surface(pages_[page], NULL);
    }

    guint page = current + PAGE_PRELOADING;

    for (; page < pages_.size(); page++) {
        document_page_set_surface(pages_[page], NULL);
    }
}

void DocumentPlugin::setPageInfo(int p) {
    char buffer[20];
    sprintf(buffer, "%04d / %04d", p, (int)pages_.size());
    string text = file_info_string_ + buffer;
    set_toolbar_info(text.c_str());
}

void DocumentPlugin::loadPages(DocumentLoaderPtr loader,
                               GAsyncQueue *load_queue,
                               GAsyncQueue *done_queue)
{
    while (loader->isActive()) {
        gpointer data = g_async_queue_pop ( load_queue );
        PageTask *task = static_cast<PageTask*>(data);
        int page = task->page;

        if (page < 0) {
            delete task;
        }
        else {
            gint current = loader->getCurrentPage();
            if (page > current+PAGE_PRELOADING || page < current-PAGE_PRELOADING) {
                task->state = PageTask::OUT_OF_RANGE;
            }
            else {
                task->surface = loader->getPage(task->page, task->scale);
                if (NULL == task->surface) {
                    task->state = PageTask::ERROR;
                }
                else {
                    task->state = PageTask::LOADED;
                }
            }
            g_async_queue_push(done_queue, data);
        }
    }

    g_async_queue_unref(load_queue);
    g_async_queue_unref(done_queue);
}

gboolean DocumentPlugin::updatePages()
{
    int tries = PAGE_PRELOADING;
    if (tries > loading_counter_) {
        tries = loading_counter_;
    }

    while (tries > 0) {
        gpointer data = g_async_queue_try_pop(done_queue_);
        if (NULL == data) {
            return true;
        }

        PageTask *task = static_cast<PageTask*>(data);

        switch (task->state) {
            case PageTask::LOADED:
                document_page_set_surface(pages_[task->page], task->surface);
                break;
            case PageTask::ERROR:
                // TODO: report error
                break;
            default:
                break;
        }

        loading_counter_--;

        delete task;
    }

    //TODO: unload pages

    if (loading_counter_ == 0) {
        loading_id_ = 0;
        return false;
    }
    else {
        return true;
    }
}

gboolean DocumentPlugin::updatePagesCb(gpointer ptr)
{
    return static_cast<DocumentPlugin*>(ptr)->updatePages();
}

void DocumentPlugin::checkScrollbar(GtkAdjustment * adjustment)
{
    int page = 0;
    int total_pages = pages_.size();
    gdouble temp = 0;
    gdouble position = gtk_adjustment_get_value(adjustment);
    position += gtk_adjustment_get_page_size(adjustment) / 2;

    for( ; page < total_pages ; page++) {
        temp += document_page_get_height(pages_[page]);
        if(position < temp) {
            break;
        } else {
            temp += PAGE_SPACING;
        }
    }

    if(page != loader_->getCurrentPage()) {
        loader_->setCurrentPage(page);
        loadPage(page);
        preloadPages(page);
        unloadPages(page);
    }
    setPageInfo(page+1);
}

void DocumentPlugin::checkScrollbarCb(GtkAdjustment * adjustment, gpointer data)
{
    static_cast<DocumentPlugin*>(data)->checkScrollbar(adjustment);
}

void DocumentPlugin::drawDocumentLayout( GtkContainer * container )
{
    GtkWidget *swin = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy      ( GTK_SCROLLED_WINDOW(swin ),
                                          GTK_POLICY_AUTOMATIC,
                                          GTK_POLICY_AUTOMATIC );

    gtk_scrolled_window_set_shadow_type ( GTK_SCROLLED_WINDOW(swin ),
                                          GTK_SHADOW_NONE);

    vAdjustment_ = gtk_scrolled_window_get_vadjustment ( GTK_SCROLLED_WINDOW(swin) );
    g_signal_connect (vAdjustment_,
                      "value-changed",
                      G_CALLBACK(checkScrollbarCb),
                      this);

    GtkWidget *layout = gtk_layout_new(NULL, NULL);
    g_signal_connect(layout, "draw", G_CALLBACK(cleanBackground), NULL);

    gint x = 0;
    gint y = 0;
    PagesList::const_iterator it;
    gint max_width = scale_ * max_width_;

    for ( it=pages_.begin(); it != pages_.end(); it++) {
        DocumentPage *page = *it;
        x = 0.5 * (max_width - document_page_get_width(page)) + 0.5;
        gtk_layout_put(GTK_LAYOUT(layout), GTK_WIDGET(page), x, y);
        y += document_page_get_height(page) + PAGE_SPACING;
    }

    gtk_layout_set_size(GTK_LAYOUT(layout), max_width, y - PAGE_SPACING);

    gtk_container_add( GTK_CONTAINER( swin ), layout);
    //GtkWidget *alignment = gtk_alignment_new(0.5,0.0,0.0,0.0);
    //gtk_container_add( GTK_CONTAINER(alignment), layout);
    //gtk_container_add( GTK_CONTAINER( swin ), alignment);

    gtk_container_add( container, swin );
    gtk_widget_show_all( swin );

    file_info_string_ = get_file_info_string()  + "     " + _("Page: ");
    //g_debug("c");
    //setPageInfo(1);
    //g_debug("d");
}

/*static void removeContainerChildWidget(GtkWidget *widget, gpointer ptr)
{
    PasswordData *data = static_cast<PasswordData*>(ptr);
    gtk_container_remove(data->container, widget);
}*/

void DocumentPlugin::checkPassword(GtkButton *button, gpointer ptr)
{
    PasswordData *data = static_cast<PasswordData*>(ptr);
    if (gtk_entry_get_text_length(GTK_ENTRY(data->entry)) == 0) {
        g_debug("empty password");
        gtk_widget_grab_focus(data->entry);
    }
    else {
        const char *password = gtk_entry_get_text(GTK_ENTRY(data->entry));
        data->document->loader_->setPassword(password);
        if (data->document->load() && DOCUMENT_LOADED == data->document->state_) {
            /*gtk_container_foreach(data->container, removeContainerChildWidget, ptr);
            data->document->ui->resize( data->document->get_width(), data->document->get_height() );
            g_debug("1");
            data->document->draw(data->container);
            g_debug("100");*/
            data->document->ui->prepare_window();
            data->document->show();

        }
        else {
            g_debug("incorrect password");
            GdkRGBA color = {1.,0.,0.,1.};
            gtk_widget_override_color (data->label, GTK_STATE_FLAG_NORMAL, &color);
            gtk_label_set_text(GTK_LABEL(data->label), _("Incorrect password. Please try again"));
            gtk_entry_set_text(GTK_ENTRY(data->entry), "");
            gtk_widget_grab_focus(data->entry);
        }
    }
}

static gboolean blockKeyEvents(GtkWidget *, GdkEvent*, gpointer)
{
    return true;
}

void DocumentPlugin::drawPasswordForm( GtkContainer * container )
{

    PasswordData *data = new PasswordData;
    data->document = this;
    data->container = container;

    data->label = gtk_label_new(_("This document requires a password to be opened."));
    GdkRGBA color = {1.,1.,1.,1.};
    gtk_widget_override_color (data->label, GTK_STATE_FLAG_NORMAL, &color);
    gtk_misc_set_alignment(GTK_MISC(data->label), 0.5, 0.5);

    data->entry = gtk_entry_new();
    g_signal_connect(data->entry, "key-release-event", G_CALLBACK(blockKeyEvents), data);
    gtk_entry_set_visibility(GTK_ENTRY(data->entry), false);
    gtk_widget_set_size_request(data->entry, PASSWORD_FORM_WIDTH / 2, -1);
    gtk_widget_grab_focus(data->entry);
    GtkWidget *entry_align = gtk_alignment_new(0.5, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(entry_align), data->entry);

    GtkWidget *button = gtk_button_new_with_label(_("Unlock Document"));
    GtkWidget *button_align = gtk_alignment_new(0.5, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(button_align), button);
    g_signal_connect(button, "clicked",G_CALLBACK(checkPassword), data);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_box_pack_start(GTK_BOX(vbox), data->label, false, false, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry_align, false, false, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button_align, false, false, 0);

    GtkWidget *alignment = gtk_alignment_new(0.5, 0.5, 0.0, 0.0);

    gtk_container_add( GTK_CONTAINER(alignment), vbox);
    gtk_container_add( container, alignment);
    gtk_widget_show_all( alignment );
}