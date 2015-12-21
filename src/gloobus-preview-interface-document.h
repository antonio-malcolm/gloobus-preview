#ifndef _IDOCUMENT_
#define _IDOCUMENT_

#include <string>

#include "gloobus-preview-interface-main.h"

#define PAGE_SPACING 5
#define PAGE_PRELOAD 5

using namespace std;

class iDocument : public interface {
    protected:
        GtkWidget       *   m_scwin;  //Scolled window to store the document pages
        GtkAdjustment   *   m_hadj;   //To move the horizontal scroll
        GtkAdjustment   *   m_vadj;
        GtkWidget       *   m_vBox;   //Vertical Box to put the pages
        gdouble             m_upper;
        gint                n_pages;
        gint                current_page;
        gdouble             current_position;
        GtkWidget       **  pages;
        GtkWidget       **  page_containers;
        gint            *   page_heights;
        GtkWidget       *   info_label;
        PangoAttrList   *   info_attrs;
        string              file_info;
        gint                page_width;
        gint                page_height;
        gint                visible_width;
        gint                visible_height;
        guint               loader_id;
        gpointer        *   self_ptr;   // we need this to avoid segmentation
                                        // fault when iDocument is destroyed
                                        // and page_loader is still running

    public:
                        iDocument();
    virtual             ~iDocument();

    virtual int         get_width           ( void );
    virtual int         get_height          ( void );
    virtual void        draw                ( GtkContainer * container );
    virtual bool        load                ( void );
    virtual void        end                 ( void );

            float       get_scale           ( int x, int y );
            void        load_page           ( int num );
    static  gboolean    page_loader         (gpointer);
    static  gboolean    draw_cb        (GtkWidget *, cairo_t*, gpointer);
    static  void        check_scrollbar     (GtkAdjustment *, gpointer);
            void        set_page_info       (int);
            void        set_page_widget     (GtkWidget*, GtkWidget*,bool=true);
    virtual bool        key_press_event     (int );

    /* these virtual functions musth be implemented for iDocument plugins */
    virtual bool        load_doc            ( void )        = 0;
    virtual int         get_n_pages         ( void )        = 0;
    virtual int         get_page_width      ( void )        = 0;
    virtual int         get_page_height     ( void )        = 0;
    virtual GtkWidget*  get_page            ( int page )    = 0;
    virtual void        end_plugin          ( void )        = 0;
};

#endif
