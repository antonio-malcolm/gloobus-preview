#include "gloobus-preview-interface-main.h"

interface::interface()
    :m_gfile(0)
    ,m_file_info(0)
    ,m_error(0)
    ,toolbar_info(0)
    ,m_block_slideshow(0)
{
    g_debug("Creating interface");
    ui = Gloobus::instance()->get_ui();
}

interface::~interface() {
    g_debug("Destroying interface");
    if(m_error)
        g_error_free(m_error);
}

// ================================= SET FILE NAME ============================ //
// @param filename is the path to the file (not URI)
// @returns void
//
// It sets the member variable filename, gfile and sets the basic information
bool interface::set_filename( string filename )
{
    m_filename = filename;
    //m_gfile    = g_file_new_for_path (filename.c_str () );
    m_gfile = Gloobus::instance()->get_file();

    set_file_info();
    if( m_error )
        return false;
    return true;
}

// ================================= SET FILE INFO ========================== //
void interface::set_file_info ()
{
    m_file_info = Gloobus::instance()->get_fileinfo();
    if(m_file_info) {
        m_filetype = g_file_info_get_attribute_string ( m_file_info,    "standard::content-type" );

        g_debug(CLI_COLOR_GREEN "FILE INFO" CLI_COLOR_RESET);
        g_debug("Screen size: %ix%i", ui->get_screen_width(), ui->get_screen_height());

        char * uri = g_file_get_uri ( m_gfile); // FIXME: why  doing this if
        g_debug("URI:   %s",uri);               //        debug is not rquired
        g_free(uri);

        g_debug("THUMB: %s",g_file_info_get_attribute_byte_string( m_file_info, "thumbnail::path" ));
        g_debug("Type:  %s" ,m_filetype.c_str());
        g_debug("Size:  %s",get_file_size_string(m_filename).c_str());
    }
}


// ================================= GET SAFE TYPE ========================== //
string interface::get_safe_type (bool replace_slash )
{
    string mime = m_filetype;
    mime = mime.substr(0,mime.find(";"));
    mime = mime.substr(0,mime.find(","));

    if(mime == "x-directory/normal")        { mime = "folder";}

    if(replace_slash)
    {
        if(mime.find("/") != string::npos)
            mime.replace(mime.find("/"),1,"-");
    }

    return mime;
}

void interface::set_toolbar_info( const gchar *str )
{
    if(toolbar_info) {
        gtk_label_set_text(GTK_LABEL(toolbar_info), str);
    }
}

string interface::get_file_size_string(string filename)
{

    double size = get_file_size( filename );
    if(size == -1) return _("Directory");

    char * size2 = g_format_size( size );
    string result(size2);
    g_free(size2);
    return result;

}

double interface::get_file_size( string filename )
{
    GFileInfo * info = Gloobus::instance()->get_fileinfo();
    string mime = g_file_info_get_attribute_string( info,
        "standard::content-type" );
    if(mime == "inode/directory")
        return -1;

    double size;
    size = g_file_info_get_attribute_uint64 ( info, "standard::size" );
    return size;
}

void interface::draw_toolbar( void )
{
    GtkWidget *toolbar = ui->get_toolbar();
    int isize = ui->get_theme()->get_toolbar_icon_size();

    toolbar_info = ui->get_theme()->get_label("");
    gtk_misc_set_alignment(GTK_MISC(toolbar_info),0,0.5);
    string info = get_file_info_string ();
    gtk_label_set_text(GTK_LABEL(toolbar_info), info.c_str());

    GtkWidget *btn_fullscreen = ui->create_button( "fullscreen" , isize);
    GtkWidget *btn_defaultapp = ui->create_button( "defaultapp" , isize);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_set_homogeneous (GTK_BOX(hbox), true);
    GtkWidget *lbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    GtkWidget *rbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

    gtk_box_pack_start(GTK_BOX(hbox), lbox,true, true,0);
    gtk_box_pack_start(GTK_BOX(hbox), rbox,true, true,0);

    gtk_box_pack_start(GTK_BOX(lbox), toolbar_info, false, false,0);
    gtk_box_pack_end(GTK_BOX(lbox), btn_fullscreen, false, false,0);
    gtk_box_pack_start(GTK_BOX(rbox), btn_defaultapp, false, false,0);

    gtk_container_add( GTK_CONTAINER(toolbar) , hbox );

    g_signal_connect(btn_fullscreen, "button-press-event", G_CALLBACK(fullscreen_cb), this);
    g_signal_connect(btn_defaultapp, "button-press-event", G_CALLBACK(defaultapp_cb), this);

    gtk_widget_show_all(toolbar);
}

void interface::draw_floating_toolbar( void )
{
    GtkWidget *toolbar = ui->get_floating_toolbar();
    int isize = ui->get_theme()->get_toolbar_icon_size();

    GtkWidget *btn_fullscreen = ui->create_button( "fullscreen" , isize);
    GtkWidget *btn_defaultapp = ui->create_button( "defaultapp" , isize);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_set_homogeneous (GTK_BOX(hbox), true);
    GtkWidget *lbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    GtkWidget *rbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

    gtk_box_pack_start(GTK_BOX(hbox), lbox,true, true,0);
    gtk_box_pack_start(GTK_BOX(hbox), rbox,true, true,0);

    gtk_box_pack_end(GTK_BOX(lbox), btn_fullscreen, false, false,0);
    gtk_box_pack_start(GTK_BOX(rbox), btn_defaultapp, false, false,0);

    gtk_container_add( GTK_CONTAINER(toolbar) , hbox );

    //gtk_fixed_put(GTK_FIXED(toolbar), btn_fullscreen, 480/2-isize, (100-isize)/2);
    //gtk_fixed_put(GTK_FIXED(toolbar), btn_defaultapp, 480/2,       (100-isize)/2);

    g_signal_connect( btn_fullscreen, "button-press-event", G_CALLBACK (fullscreen_cb), NULL);
    g_signal_connect( btn_defaultapp, "button-press-event", G_CALLBACK (defaultapp_cb), NULL);

    gtk_widget_show_all(toolbar);
}

string interface::get_file_info_string ( void )
{
    string text;
    char * desc = g_content_type_get_description(m_filetype.c_str());

    text = "";
    text = text + _("Type: ") + desc + "\n\n";
    text = text + _("Size: ") + get_file_size_string(m_filename);

    g_free(desc);

    return text;
}

void interface::set_error(GError* error) {
    m_error = error;
}

GError * interface::get_error(void) {
    if(m_error)
        return g_error_copy(m_error);
    else
        return NULL;
}

bool interface::is_program_available( const char * prog) {
    char * str;
    str = g_find_program_in_path (prog);
    if (str != NULL) {
        g_free (str);
        return true;
    }
    return false;
}

void interface::resize_cb(GtkWidget *widget,
                        GtkRequisition *requisition,
                        gpointer data)
{
    interface * inf = (interface*)data;
    inf->resize( requisition->width , requisition->height );
}

gboolean interface::fullscreen_cb(GtkWidget *widget,
    GdkEventButton *event,
    gpointer data)
{
    Gloobus::instance()->fullscreen();
    return true;
}

gboolean interface::defaultapp_cb(GtkWidget *widget,
    GdkEventButton *event,
    gpointer data)
{
    Gloobus::instance()->defaultapp();
    return true;
}

void interface::show( void ) {
    GtkContainer * container = GTK_CONTAINER(ui->get_container());
    ui->resize( get_width(), get_height() );
    draw( container );

    if(ui->is_fullscreen()) {
        draw_fullscreen( container );
        draw_floating_toolbar();
        gtk_widget_hide(ui->get_toolbar());
    }/* else {
        gtk_widget_show_all(ui->get_toolbar());
    }*/
    //gtk_widget_show_all(GTK_WIDGET(container));
    //g_signal_connect(G_OBJECT( container ), "size-request", G_CALLBACK(resize_cb), this);
}

// =============== IS SLIDESHOW BLOCKED ================= //
bool interface::is_slideshow_blocked    ( void )
{
    return m_block_slideshow;
}

void interface::block_slideshow    ( bool block )
{
    m_block_slideshow = block;
}

