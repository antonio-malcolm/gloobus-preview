#include "gloobus-preview-interface-default.h"

#include <ctime>

#define IDEFAULT_WIDTH 350
#define IDEFAULT_HEIGHT 450
#define IDEFAULT_PADDING 10
#define IDEFAULT_THUMBNAIL_SIZE 300
#define IDEFAULT_THUMBNAIL_X 10
#define IDEFAULT_THUMBNAIL_Y 10
#define IDEFAULT_INFO_BOX_X 180
#define IDEFAULT_INFO_BOX_Y 30

iDefault::iDefault( void )
    :thumbnail_image(0)
    ,name_label(0)
    ,filetype_label(0)
    ,filesize_label(0)
    ,modified_label(0)
    ,error_label(0)
    ,resize_default_id(0)
{
    g_debug("Creating iDefault");
}

iDefault::~iDefault( void )
{
    g_debug("Destroying iDefault");
}

bool iDefault::load( void )
{
    g_debug("Loading iDefault");
    load_default_info();
    return true;
}

void iDefault::end( void )
{
    g_debug("Ending iDefault");
    if(info_box)
        gtk_widget_destroy(info_box);
}

int iDefault::get_width( void )
{
    return IDEFAULT_WIDTH;
}

int iDefault::get_height( void )
{
    return IDEFAULT_HEIGHT;
}

/*void iDefault::draw( GtkContainer * container )
{//FIXME: make this goood :D
    gtk_alignment_set(GTK_ALIGNMENT(container), 0.5,0.5,0,0);
    GtkWidget *align = gtk_alignment_new(0.5,0.5,1,1);
    gtk_widget_set_size_request(align, IDEFAULT_WIDTH, IDEFAULT_HEIGHT);
    //gtk_alignment_set_padding(GTK_ALIGNMENT(align),
    //    IDEFAULT_PADDING, IDEFAULT_PADDING, IDEFAULT_PADDING, IDEFAULT_PADDING);

    //GtkWidget *hbox = gtk_hbox_new(false, IDEFAULT_PADDING);
    //gtk_widget_set_size_request(hbox, IDEFAULT_WIDTH, IDEFAULT_HEIGHT);
    GtkWidget *vbox = gtk_vbox_new(false, IDEFAULT_PADDING);
        
    if(thumbnail_image)
        gtk_box_pack_start(GTK_BOX(vbox), thumbnail_image, false, false, 0);
        //gtk_fixed_put(GTK_FIXED(container), thumbnail_image, IDEFAULT_THUMBNAIL_X, IDEFAULT_THUMBNAIL_Y);
    
    //gtk_fixed_put(GTK_FIXED(container), info_box, IDEFAULT_INFO_BOX_X, IDEFAULT_INFO_BOX_Y);
    //GtkWidget *valign = gtk_alignment_new(0,0.5,0,0);
   // gtk_container_add(GTK_CONTAINER(valign), info_box);
    //gtk_box_pack_start(GTK_BOX(hbox), valign, true, true, 0);
    gtk_box_pack_start(GTK_BOX(vbox), info_box, true, true, 0);
    //resize_default_id = g_signal_connect(info_box, "size-allocate", G_CALLBACK(resize_default_cb), this );
    //resize_default_id = g_signal_connect(info_box, "realize", G_CALLBACK(resize_default_cb), this );
    
    //gtk_widget_show(thumbnail_image);
    //gtk_widget_show_all(info_box);
    
    gtk_container_add(GTK_CONTAINER(align), vbox);
    gtk_container_add(container, align);
        
    //gtk_container_add(container, vbox);
    gtk_widget_show_all(GTK_WIDGET(container));
    //ui->draw_controls();
    draw_toolbar();
}*/

void iDefault::draw( GtkContainer * container )
{

    gtk_alignment_set(GTK_ALIGNMENT(container), 0.5,0.5,0,0);
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, IDEFAULT_PADDING);
        
    if(thumbnail_image) {
        gtk_box_pack_start(GTK_BOX(vbox), thumbnail_image, false, false, 0);
    }
        
    gtk_box_pack_start(GTK_BOX(vbox), info_box, true, true, 0);
    
    gtk_container_add(container, vbox);
    gtk_widget_show_all(GTK_WIDGET(container));
    draw_toolbar();
}


void iDefault::draw_fullscreen( GtkContainer * container )
{
    //int x = ( ui->get_screen_width()-get_width() ) / 2;
    //int y = ( ui->get_screen_height()-get_height() ) / 2;
    //gtk_fixed_move(GTK_FIXED(container), thumbnail_image, x+IDEFAULT_THUMBNAIL_X, y+IDEFAULT_THUMBNAIL_Y);
    //gtk_fixed_move(GTK_FIXED(container), info_box, x+IDEFAULT_INFO_BOX_X, y+IDEFAULT_INFO_BOX_Y);
}

void iDefault::draw_unfullscreen( GtkContainer * container )
{
    //gtk_fixed_move(GTK_FIXED(container), thumbnail_image, IDEFAULT_THUMBNAIL_X, IDEFAULT_THUMBNAIL_Y);
    //gtk_fixed_move(GTK_FIXED(container), info_box, IDEFAULT_INFO_BOX_X, IDEFAULT_INFO_BOX_Y);
}

void iDefault::load_default_info( void )
{
    GloobusTheme * theme = ui->get_theme();
    GdkPixbuf * temp = ui->get_file_icon(m_file_info , IDEFAULT_THUMBNAIL_SIZE);
    if(temp)
        thumbnail_image = gtk_image_new_from_pixbuf(temp);
    g_object_unref(temp);

    name_label = theme->get_label(g_file_info_get_name(m_file_info));
    
    GFileType type = g_file_info_get_file_type(m_file_info);
    if(type == G_FILE_TYPE_REGULAR) {
        char * desc;
        
        const char * ctype = g_file_info_get_content_type(m_file_info);
        char * ctype_desc = g_content_type_get_description(ctype);
        //desc = g_strconcat(ctype_desc," (", ctype, ")", NULL);
        desc = g_strdup_printf( "%s (%s)", ctype_desc, ctype);
        filetype_label = theme->get_label(desc);
        g_free(ctype_desc);
        g_free(desc);

        goffset fsize = g_file_info_get_size(m_file_info);
        char * fsize_desc = g_format_size( fsize );
        desc = g_strdup_printf( "%s (%li bytes)", fsize_desc, (long int)fsize);
        filesize_label = theme->get_label(desc);
        g_free(fsize_desc);
        g_free(desc);

        time_t time;
        struct tm * timeinfo;
        
        time = g_file_info_get_attribute_uint64(m_file_info, G_FILE_ATTRIBUTE_TIME_MODIFIED);
        desc = new char[128];
        timeinfo = localtime( &time );
        strftime (desc,128,"%x %X",timeinfo);
        modified_label = theme->get_label(desc);
        delete[] desc;

        time = g_file_info_get_attribute_uint64(m_file_info, G_FILE_ATTRIBUTE_TIME_ACCESS);
        desc = new char[128];
        timeinfo = localtime( &time );
        strftime (desc,128,"%x %X",timeinfo);
        accessed_label = theme->get_label(desc);
        delete[] desc;

    } else if(type == G_FILE_TYPE_DIRECTORY) {
        filetype_label = theme->get_label("Folder (inode/directory)");
        //TODO: load number of files in the folder with g_idle_add ???
    } else {
        //TODO: do what ???
    }

    info_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    //gtk_widget_set_size_request(info_box, IDEFAULT_WIDTH, IDEFAULT_HEIGHT);
    GtkWidget * lbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget * rbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(info_box), lbox, false, false, 0);
    gtk_box_pack_start(GTK_BOX(info_box), rbox, true, true, 0);
    GtkWidget * label;

    if(name_label) {
        label = theme->get_label(_("Name:"));
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
        gtk_misc_set_alignment(GTK_MISC(name_label), 0, 0);
        gtk_box_pack_start(GTK_BOX(lbox), label, false, false, 0);
        gtk_box_pack_start(GTK_BOX(rbox), name_label, false, false, 0);
    }

    /*if(filetype_label) {
        label = theme->get_label(_("Type:"));
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
        gtk_misc_set_alignment(GTK_MISC(filetype_label), 0, 0);
        gtk_box_pack_start(GTK_BOX(lbox), label, false, false, 0);
        gtk_box_pack_start(GTK_BOX(rbox), filetype_label, false, false, 0);
    }*/
    /*if(filesize_label) {
        label = theme->get_label(_("Size:"));
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
        gtk_misc_set_alignment(GTK_MISC(filesize_label), 0, 0);
        gtk_box_pack_start(GTK_BOX(lbox), label, false, false, 0);
        gtk_box_pack_start(GTK_BOX(rbox), filesize_label, false, false, 0);
    }*/
    if(modified_label) {
        label = theme->get_label(_("Accessed:"));
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
        gtk_misc_set_alignment(GTK_MISC(modified_label), 0, 0);
        gtk_box_pack_start(GTK_BOX(lbox), label, false, false, 0);
        gtk_box_pack_start(GTK_BOX(rbox), modified_label, false, false, 0);
    }
    if(accessed_label) {
        label = theme->get_label(_("Modified:"));
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
        gtk_misc_set_alignment(GTK_MISC(accessed_label), 0, 0);
        gtk_box_pack_start(GTK_BOX(lbox), label, false, false, 0);
        gtk_box_pack_start(GTK_BOX(rbox), accessed_label, false, false, 0);
    }

    if(m_error) {
        label = theme->get_error_label(_("Error:"));
        error_label = theme->get_error_label(m_error->message);
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
        gtk_misc_set_alignment(GTK_MISC(error_label), 0, 0);
        gtk_box_pack_start(GTK_BOX(lbox), label, false, false, 0);
        gtk_box_pack_start(GTK_BOX(rbox), error_label, false, false, 0);
    }
}

void iDefault::resize_default_cb( GtkWidget *widget, GtkAllocation *allocation, gpointer data )
{
    g_debug("realize");
    iDefault * self = (iDefault*)data;
    GtkRequisition req;
    gtk_widget_get_preferred_size( widget, &req, NULL );
    int width = IDEFAULT_PADDING*3 + IDEFAULT_THUMBNAIL_SIZE + req.width;
    int height = req.height;
    if(height > IDEFAULT_THUMBNAIL_SIZE)
        height += IDEFAULT_PADDING*2;
    else
        height = IDEFAULT_PADDING*2 + IDEFAULT_THUMBNAIL_SIZE;
    /*
    GtkAllocation alloc;
    gtk_widget_get_allocation(self->info_box, &alloc);
    int width = IDEFAULT_PADDING*3 + IDEFAULT_THUMBNAIL_SIZE + alloc.width;
    int height = alloc.height;
    if(height > IDEFAULT_THUMBNAIL_SIZE)
        height += IDEFAULT_PADDING*2;
    else
        height = IDEFAULT_PADDING*2 + IDEFAULT_THUMBNAIL_SIZE;
        */
    /*
    int width = IDEFAULT_PADDING*3 + IDEFAULT_THUMBNAIL_SIZE + allocation->width;
    int height = allocation->height;
    if(height > IDEFAULT_THUMBNAIL_SIZE)
        height += IDEFAULT_PADDING*2;
    else
        height = IDEFAULT_PADDING*2 + IDEFAULT_THUMBNAIL_SIZE;
        */
    self->ui->resize(width, height);
    //gtk_widget_set_size_request(widget, width, height);
    g_signal_handler_disconnect(widget, self->resize_default_id);
}

/* ************************************************************************** */

#define IERROR_WIDTH 384
#define IERROR_HEIGHT 84
#define IERROR_ICON_SIZE 64
#define IERROR_PADDING 10
#define IERROR_LABEL_WIDTH 280
#define IERROR_LABEL_HEIGHT 64

iError::iError( void )
{
    g_debug("Creating iError");
}

iError::~iError( void )
{
    g_debug("Destroying iError");
}

bool iError::load( void )
{
    g_debug("Loading iError");
    return true;
}

void iError::end( void )
{
    g_debug("Ending iError");
}

int iError::get_width( void )
{
    return IERROR_WIDTH;
}

int iError::get_height( void )
{
    return IERROR_HEIGHT;
}

void iError::draw( GtkContainer * container )
{
    GdkPixbuf *icon = ui->get_theme()->get_icon("error", IERROR_ICON_SIZE);
    error_image = gtk_image_new_from_pixbuf(icon);
    error_label = ui->get_theme()->get_error_label(m_error->message, 14);
    gtk_widget_set_size_request(error_label, IERROR_LABEL_WIDTH, IERROR_LABEL_HEIGHT);
    gtk_label_set_line_wrap(GTK_LABEL(error_label), true);
    gtk_misc_set_alignment(GTK_MISC(error_label), 0.5 , 0.5);
    gtk_label_set_justify(GTK_LABEL(error_label), GTK_JUSTIFY_CENTER);
    gtk_fixed_put(GTK_FIXED(container), error_image, IERROR_PADDING, IERROR_PADDING);
    gtk_fixed_put(GTK_FIXED(container), error_label, IERROR_PADDING*2+IERROR_ICON_SIZE, IERROR_PADDING);
    gtk_widget_show(error_image);
    gtk_widget_show(error_label);
}

void iError::draw_fullscreen( GtkContainer * container )
{
    int x = ( ui->get_screen_width()-get_width() ) / 2;
    int y = ( ui->get_screen_height()-get_height() ) / 2;
    gtk_fixed_move(GTK_FIXED(container), error_image, x+IERROR_PADDING, y+IERROR_PADDING);
    gtk_fixed_move(GTK_FIXED(container), error_label, x+IERROR_PADDING*2+IERROR_ICON_SIZE, y+IERROR_PADDING);
}

void iError::draw_unfullscreen( GtkContainer * container )
{
    gtk_fixed_move(GTK_FIXED(container), error_image, IERROR_PADDING, IERROR_PADDING);
    gtk_fixed_move(GTK_FIXED(container), error_label, IERROR_PADDING*2+IERROR_ICON_SIZE, IERROR_PADDING);
}
