#include "gloobus-preview-interface-text.h"

#include <gdk/gdkkeysyms-compat.h>

iText::iText( void ) {
    g_debug("Creating iText");
}

iText::~iText( void ) {
    g_debug("Destroying iText");
}

//============================= END =========================================== //
void iText::end()
{
    g_debug("Ending iText");
    g_object_unref(m_sBuf);
}
// ========================= CONSTRUCTOR ================================= //
bool iText::load(void)
{
    g_debug("Loading iText");
    //m_textbuf = get_textbuf();
    m_sBuf = get_textbuf();
    if( !m_sBuf ) {
        m_error = g_error_new(g_quark_from_string ("iText"), 1001,
            _("Error loading text"));
        return false;
    }
    return true;
};


// =========================== GET SIZE (W) ================================= //
int  iText::get_width()
{
    int w = ui->get_max_width();
    if(w > 1024)
        return 1024;
    return w;
}

// =========================== GET SIZE (H) ================================= //
int  iText::get_height()
{
    return ui->get_max_height();
}

void iText::draw( GtkContainer * container )
{
    draw_file_text  ( GTK_CONTAINER(container ));
    draw_toolbar();
}

void iText::draw_file_text(GtkContainer * container)
{
    m_scwin = gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(m_scwin),
        GTK_POLICY_AUTOMATIC ,GTK_POLICY_AUTOMATIC );

    view = gtk_source_view_new_with_buffer(m_sBuf);

    // Disable editing
    gtk_widget_set_can_focus (view, FALSE);

    font = pango_font_description_from_string("monospace 10");
    gtk_widget_modify_font          (view, font);
    //pango_font_description_free       (font_desc);

    gtk_text_view_set_left_margin                   (GTK_TEXT_VIEW(view), 10);
    /*Better view if source language*/
    if(gtk_source_buffer_get_language(m_sBuf) != NULL) {
        gtk_source_view_set_auto_indent             ( GTK_SOURCE_VIEW(view), true );
        gtk_source_view_set_show_line_numbers     ( GTK_SOURCE_VIEW(view), true );
        //gtk_source_view_set_tab_width               ( GTK_SOURCE_VIEW(view),  4    );
        //gtk_source_view_set_highlight_current_line  ( GTK_SOURCE_VIEW(view), true );
    }

    gtk_container_add   ( GTK_CONTAINER(m_scwin), view );
    gtk_container_add   ( container, m_scwin );
    gtk_widget_show_all ( m_scwin );

    m_vadj = gtk_scrolled_window_get_vadjustment( GTK_SCROLLED_WINDOW(m_scwin));
}


// ============= KEY PRESS EVENT ================ //
bool iText::key_press_event(int keyval)
{

    if(keyval == 65451) {
        //gtk_widget_get_pango_context(view);
        int size = pango_font_description_get_size(font) + PANGO_SCALE;
        pango_font_description_set_size(font, size);
        gtk_widget_modify_font(view, font);
        return true;
    } else if (keyval == 65453) {
        int size = pango_font_description_get_size(font);
        if(size > PANGO_SCALE*6) {
            size -= PANGO_SCALE;
            pango_font_description_set_size(font, size);
            gtk_widget_modify_font(view, font);
        }
        return true;
    }

    if( keyval == GDK_Page_Up )
    {
        gtk_adjustment_set_value(m_vadj, gtk_adjustment_get_value(m_vadj) - get_height());
        return true;
    }
    else if( keyval == GDK_Page_Down )
    {
        int temp = gtk_adjustment_get_upper(m_vadj) -  get_height();  //To not put it in a overflow, i think it can be done changint the upper to upper - visible_height too
        if ( gtk_adjustment_get_value(m_vadj) + get_height() > temp)
        {
            gtk_adjustment_set_value(m_vadj, temp);
        }
        else
        {
            gtk_adjustment_set_value(m_vadj, gtk_adjustment_get_value(m_vadj) + get_height());
        }
        return true;
    }
    return false;

}
