#include "gloobus-preview-interface-document.h"

// TODO: replace scrolled window with event box, draw pages with cairo, make pages draggable

#include <gdk/gdkkeysyms-compat.h>

iDocument::iDocument()
    :m_scwin(0)
    ,m_hadj(0)
    ,m_vadj(0)
    ,m_vBox(0)
    ,n_pages(0)
    ,current_page(0)
    ,current_position(0)
    ,pages(0)
    ,page_containers(0)
    ,page_heights(0)
    ,info_label(0)
    ,info_attrs(0)
    ,page_width(0)
    ,page_height(0)
    ,visible_width(0)
    ,visible_height(0)
    ,loader_id(0)
{
    g_debug("Creating iDocument");
    self_ptr = new gpointer;
    *self_ptr = (gpointer)this;
}

// ========================== DESTRUCTOR ========================= //
iDocument::~iDocument() {
    g_debug("Destroying iDocument");
    if(*self_ptr)
        *self_ptr = 0;
    else
        delete self_ptr;
}

//=========================  END ========================================= //
void iDocument::end() {
    g_debug("Ending iDocument");

    if(loader_id)
        g_source_remove(loader_id);
    if(pages and page_containers) {
        for(int i=0 ; i < n_pages ; i++) {
            if(pages[i] != NULL)
                gtk_widget_destroy(pages[i]);
            gtk_widget_destroy(page_containers[i]);
        }
        delete pages;
        delete page_containers;
    }
    if(m_vBox)
        gtk_widget_destroy(m_vBox);
    if(m_scwin)
        gtk_widget_destroy(m_scwin);
    if(info_label)
        gtk_widget_destroy(info_label);
    if(info_attrs)
        pango_attr_list_unref(info_attrs);
    if(page_heights)
        delete page_heights;
}

// ========================= LOAD ================================= //
bool iDocument::load(void)
{
    g_debug("Loading iDocument");

    if(!load_doc())
        return false;

    //g_debug("Loading Document (%i Pages)", get_n_pages ());
    //g_debug("Page size: %ix%i",get_width(),get_height());

    n_pages         = get_n_pages();
    page_containers = new GtkWidget*[n_pages];
    page_heights    = new gint[n_pages];
    pages           = new GtkWidget*[n_pages];
    page_width      = get_page_width();
    page_height     = get_page_height();

    for(int i = 0 ; i < n_pages ; i++)
    {
        pages[i]        = NULL;
        page_heights[i] = page_height;
    }

    visible_height  = get_height()-TOOLBAR_HEIGHT;
    visible_width   = get_width();

    m_scwin = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy      ( GTK_SCROLLED_WINDOW(m_scwin ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    gtk_scrolled_window_set_shadow_type ( GTK_SCROLLED_WINDOW(m_scwin ), GTK_SHADOW_NONE);
    //gtk_widget_set_size_request(m_scwin, visible_width, visible_height);
    GtkWidget* viewport = gtk_viewport_new(NULL,NULL);
    gtk_viewport_set_shadow_type        ( GTK_VIEWPORT(viewport), GTK_SHADOW_NONE);

    m_vadj  = gtk_scrolled_window_get_vadjustment   ( GTK_SCROLLED_WINDOW(m_scwin));
    g_object_set((gpointer)m_vadj, "page-size",     (double)page_height,NULL);

    m_hadj  = gtk_scrolled_window_get_hadjustment   ( GTK_SCROLLED_WINDOW(m_scwin));
    m_upper = gtk_adjustment_get_upper              ( m_hadj );
    m_vBox  = gtk_box_new                           ( GTK_ORIENTATION_VERTICAL, PAGE_SPACING);

    info_attrs = pango_attr_list_new();
    pango_attr_list_insert  ( info_attrs, pango_attr_foreground_new (65000,65000,65000));
    pango_attr_list_insert  ( info_attrs, pango_attr_weight_new     (PANGO_WEIGHT_BOLD));
    pango_attr_list_insert  ( info_attrs, pango_attr_size_new       (20*PANGO_SCALE));

    for(int i=0 ; i < n_pages ; i++)
    {
        GtkWidget* label    = gtk_label_new(_("Loading..."));
        page_containers[i]  = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
        gtk_widget_set_size_request(page_containers[i], page_width, page_height);
        gtk_box_pack_start          ( GTK_BOX(m_vBox),  page_containers[i], false, false, 0);
        gtk_label_set_attributes    ( GTK_LABEL(label), info_attrs);
        gtk_box_pack_start          ( GTK_BOX(page_containers[i]), label ,true, true,0);
    }

    gtk_container_add(GTK_CONTAINER(viewport), m_vBox);
    gtk_container_add(GTK_CONTAINER(m_scwin), viewport);


    loader_id = g_idle_add(&page_loader, self_ptr);
    GtkAdjustment* v_adj = gtk_scrolled_window_get_vadjustment( GTK_SCROLLED_WINDOW(m_scwin));

    //g_signal_connect(m_vBox, "draw", G_CALLBACK(draw_cb), this);
    g_signal_connect(v_adj,  "value-changed",G_CALLBACK(check_scrollbar), (void*)this);

    return true;
};
// =========================== GET SIZE (W)================================= //
int  iDocument::get_width()
{
    int w = get_page_width() + 15; //15 = Scrollbar
    int max_w = ui->get_max_width();
    if(w > max_w)
        w = max_w;
    return w;
}

// =========================== GET SIZE (H)================================= //
int  iDocument::get_height()
{
    int h = get_page_height() * n_pages + 15 + TOOLBAR_HEIGHT;
    int max_h = ui->get_max_height();
    if(h > max_h)
        h = max_h;
    return h;
}

// ========================== SET PAGE INFO ================== //
void iDocument::set_page_info(int p) {
    char buffer[20];
    sprintf(buffer, "%04d / %04d", p, n_pages);
    string text = file_info + "     " + _("Page: ") + buffer;
    //gtk_label_set_text(GTK_LABEL(info_label), text.c_str());
    set_toolbar_info(text.c_str());
}
// ========================= GET SCALE ===================== //
float iDocument::get_scale   ( int x, int y )
{

    float scale = 1;
    if ( x > y ) {
        scale = ui->get_screen_width()-190 / x;
    } else {
        scale = ui->get_screen_height()-220 / y;
    }
    return scale;
}

// ========================= DRAW ===================== //
// Virtual function that draws what plugin wants
void iDocument::draw( GtkContainer * container )
{
    gtk_container_add(container, m_scwin);
    gtk_widget_show_all( m_scwin  );

    file_info = get_file_info_string();
    set_page_info(1);

    draw_toolbar();
}

gboolean iDocument::draw_cb(GtkWidget * widget, cairo_t *cr, gpointer data)
{
    //iDocument *doc = (iDocument*)data;

    cairo_set_source_rgb(cr, 0,0,0);
    cairo_paint(cr);

    //cairo_set_operator        ( cr, CAIRO_OPERATOR_CLEAR );
    //cairo_paint( cr );
    //cairo_set_operator        ( cr, CAIRO_OPERATOR_OVER );
    /*
    if(data) {
        cairo_rectangle(cr, event->area.x, event->area.y,
            event->area.width, event->area.height);
        cairo_set_source_rgba( cr, 1, 1, 1, 1);
        cairo_clip           (cr);
        cairo_paint          ( cr );
    } else {
        cairo_set_operator   ( cr, CAIRO_OPERATOR_CLEAR );
        cairo_paint          ( cr );
        cairo_set_operator   ( cr, CAIRO_OPERATOR_OVER);
        cairo_set_source_rgba(cr, BG_R, BG_G, BG_B, BG_A);
        cairo_paint          ( cr );
    }*/
    return false;
}

// controls vertrical scrolbars "value-changed" signal
void iDocument::check_scrollbar(GtkAdjustment * adjustment, gpointer ptr) {
    iDocument* doc = (iDocument*)ptr;
    gdouble position = gtk_adjustment_get_value(adjustment);
    position += (doc->visible_height/2);
    gdouble temp = 0;
    int page = 0;
    for( ; page < doc->n_pages ; page++) {
        temp += doc->page_heights[page];
        if(position < temp) {
            break;
        } else {
            temp += PAGE_SPACING;
        }
    }

    if(page == doc->current_page) return;

    doc->current_page = page;

    if(*(doc->self_ptr))
        *(doc->self_ptr) = 0;
    else
        delete doc->self_ptr;
    if(doc->loader_id)
        g_source_remove(doc->loader_id);
    doc->self_ptr = new gpointer;
    *(doc->self_ptr) = ptr;
    doc->loader_id = g_idle_add(&page_loader, doc->self_ptr);
    doc->set_page_info(page+1);
}


void iDocument::load_page( int num ) {
    int width, height;

    pages[num] = get_page(num);
        //TODO: deal with a NULL !!!

    gtk_widget_get_size_request(pages[num], &width, &height);

    gtk_widget_set_size_request(page_containers[num], width, height);
    page_heights[num] = height;
    set_page_widget(page_containers[num], pages[num]);
}

// async page loader and cleaner, at every page gives space to gtk main loop
gboolean iDocument::page_loader(gpointer ptr) {
    //g_debug("Page loader start");
    if( *((iDocument**)ptr) == 0 ) {
        delete (iDocument**)ptr;
        return false;
    }
    iDocument* doc = *((iDocument**)(ptr));
    int start = doc->current_page;
    int to_load = 0;
    // loading pages is done here:
    for (int i = 0; i < PAGE_PRELOAD; i++) {
        while (gtk_events_pending ())
            gtk_main_iteration ();
        if(*((iDocument**)ptr) == 0) {
            delete (iDocument**)ptr;
            return false;
        }
        // one page backward
        to_load = start - i; // i=0 loads current page
        if(to_load >= 0 and !doc->pages[to_load]) {
            doc->load_page(to_load);
        }
        while (gtk_events_pending ())
            gtk_main_iteration ();
        if(*((iDocument**)ptr) == 0) {
            delete (iDocument**)ptr;
            return false;
        }
        // one page forward
        to_load = start + i + 1;
        if(to_load < doc->n_pages and !doc->pages[to_load]) {
            doc->load_page(to_load);
        }
    }
    // cleaning pages out of preload range is done here:
    for(int i=0 ; i < doc->n_pages ; i++) {
        if(doc->pages[i]) {
            if(i < start - PAGE_PRELOAD or i > start + PAGE_PRELOAD) {
                while (gtk_events_pending ())
                    gtk_main_iteration ();
                if(*((iDocument**)ptr) == 0) {
                    delete (iDocument**)ptr;
                    return false;
                }
                doc->pages[i] = 0;
                GtkWidget* label = gtk_label_new(_("Loading..."));
                gtk_label_set_attributes(GTK_LABEL(label), doc->info_attrs);
                doc->set_page_widget(doc->page_containers[i], label, false);
            }
        }
    }

    *((iDocument**)ptr) = 0;
    //g_debug("Page loader end\n");
    return false;
}

// destroys everything inside given page and places widget iside it
void iDocument::set_page_widget(GtkWidget* page,
    GtkWidget* widget,
    bool image)
{
    GList * list = gtk_container_get_children(GTK_CONTAINER(page));
    do {
        gtk_widget_destroy(GTK_WIDGET(list->data));
        list = g_list_delete_link (list,list);
    } while(list);
    if(image) {
        //adjust upper when changed
        if(m_upper != gtk_adjustment_get_upper (m_hadj) ) {
            m_upper = gtk_adjustment_get_upper (m_hadj);
            // scroll allways at the center
            gtk_adjustment_set_value( m_hadj, m_upper / 4 );
        }
    }
    gtk_box_pack_start(GTK_BOX(page), widget, true, true,0);
    gtk_widget_show(widget);
}

// ============= KEY PRESS EVENT ================ //
bool iDocument::key_press_event(int keyval)
{
    if( keyval == GDK_Page_Up )
    {
        gtk_adjustment_set_value(m_vadj, gtk_adjustment_get_value(m_vadj) - visible_height);
        return true;
    }
    else if( keyval == GDK_Page_Down )
    {

        int temp = gtk_adjustment_get_upper(m_vadj) -  visible_height;  //To not put it in a overflow, i think it can be done changint the upper to upper - visible_height too
        if ( gtk_adjustment_get_value(m_vadj) + visible_height > temp)
        {
            gtk_adjustment_set_value(m_vadj, temp);
        }
        else
        {
            gtk_adjustment_set_value(m_vadj, gtk_adjustment_get_value(m_vadj) + visible_height);
        }


        return true;
    }
    return false;

}
