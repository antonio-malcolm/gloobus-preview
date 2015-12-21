#include "plugin-ttf.h"

// TODO: font info, add an entry in menu and show a window with info?

iTtf::iTtf()
{
    g_debug("Creating iTtf");
    menuItemInstallFont = ui->add_menu_item ( _("Install Font"), G_CALLBACK(install_font_cb), NULL );
}

iTtf::~iTtf()
{
    gtk_widget_destroy(menuItemInstallFont);
    g_debug("Destroying iTtf");
}

int iTtf::get_width ( void )
{
    return 900;
}

int iTtf::get_height( void )
{
    return 550;
}

void iTtf::draw( GtkContainer * container)
{
    //gtk_alignment_set(GTK_ALIGNMENT(container), 0.5,0.5,0,0);
    load_font();
    area = gtk_event_box_new();
    gtk_event_box_set_visible_window( GTK_EVENT_BOX(area) , false );
    //gtk_widget_set_size_request(GTK_WIDGET(area),get_width(),get_height());

    g_signal_connect (G_OBJECT (area), "draw", G_CALLBACK (draw_cb), this);

    gtk_container_add(container, area);
    gtk_widget_show(area);
    draw_toolbar();




}

void iTtf::draw_toolbar( void )
{
    GtkWidget *toolbar = ui->get_toolbar();
    int isize = ui->get_theme()->get_toolbar_icon_size();

    toolbar_info = ui->get_theme()->get_label("");
    gtk_misc_set_alignment(GTK_MISC(toolbar_info),0,0.5);
    string info = get_file_info_string ();
    gtk_label_set_text(GTK_LABEL(toolbar_info), info.c_str());

    GtkWidget *btn_fullscreen   = ui->create_button( "fullscreen" , isize);
    GtkWidget *btn_defaultapp   = ui->create_button( "defaultapp" , isize);
    GtkWidget *btn_installFont  = ui->create_button( "install" , isize); //TODO: create a nice icon

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_set_homogeneous (GTK_BOX(hbox), true);
    GtkWidget *lbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    GtkWidget *rbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

    gtk_box_pack_start(GTK_BOX(hbox), lbox,true, true,0);
    gtk_box_pack_start(GTK_BOX(hbox), rbox,true, true,0);

    gtk_box_pack_start(GTK_BOX(lbox),   toolbar_info,       false, false,0);
    gtk_box_pack_end(GTK_BOX(lbox),     btn_fullscreen,     false, false,0);
    gtk_box_pack_start(GTK_BOX(rbox),   btn_defaultapp,     false, false,0);
    gtk_box_pack_start(GTK_BOX(rbox),   btn_installFont,    false, false,0);

    gtk_container_add( GTK_CONTAINER(toolbar) , hbox );

    g_signal_connect(btn_fullscreen,  "button-press-event", G_CALLBACK(fullscreen_cb), this);
    g_signal_connect(btn_defaultapp,  "button-press-event", G_CALLBACK(defaultapp_cb), this);
    g_signal_connect(btn_installFont, "button-press-event", G_CALLBACK(install_font_cb), this);

    gtk_widget_show_all(toolbar);
}

void iTtf::install_font_cb(GtkWidget *widget, gpointer data)
{
    string home = g_get_home_dir();
    string fontsDir = home + "/.fonts/";

    if (!g_file_test (fontsDir.c_str(), G_FILE_TEST_EXISTS))
        g_mkdir_with_parents (fontsDir.c_str(), 0755);

    string cmd = "cp " + Gloobus::instance()->get_filename() + " " + fontsDir;

    g_debug("Installing font: %s", cmd.c_str());
    g_spawn_command_line_async(cmd.c_str(), NULL);

    string notify_text = _("Font installed sucesfully");
    cmd = "notify-send -i gloobus-preview -u critical \"" + notify_text + "\"";
    g_spawn_command_line_async(cmd.c_str(), NULL);
}

void iTtf::load_font(void)
{
    FT_Library library;
    FT_Init_FreeType( &library );
    FT_Face face;
    FT_New_Face( library, m_filename.c_str(), 0, &face );
    font = cairo_ft_font_face_create_for_ft_face(face,0);
}

void iTtf::draw_cb(GtkWidget *widget, cairo_t *cr, iTtf *ttf)
{
    cairo_text_extents_t extents;

    GtkAllocation allocation;
    gtk_widget_get_allocation (widget, &allocation);

    int y = 60;
    int w = allocation.width;
    int h = allocation.height;
    int s = 10;

    cairo_translate( cr , allocation.x, allocation.y);
    cairo_rectangle( cr, 0 , 0 , w, h);
    cairo_clip( cr );

    cairo_set_font_face     (cr, ttf->font);
    //cairo_set_source_rgba     (cr, 255, 255, 255, 1);
    GdkColor *color = ttf->ui->get_theme()->get_text_color();
    cairo_set_source_rgb    (cr, color->red, color->green, color->blue);

    cairo_set_font_size     (cr, 28);
    cairo_text_extents      (cr, "ABCDEFGHIJKLMNOPQRSTVWXYZ", &extents);
    cairo_move_to           (cr, (w - extents.width) / 2, y);
    cairo_show_text         (cr, "ABCDEFGHIJKLMNOPQRSTVWXYZ");
    y += extents.height + 10;

    cairo_text_extents      (cr, "abcdefghijklmnopqrstvwxyz", &extents);
    cairo_move_to           (cr, (w - extents.width) / 2, y);
    cairo_show_text         (cr, "abcdefghijklmnopqrstvwxyz");
    y += extents.height + 10;

    cairo_text_extents      (cr, "ÀÁàáÈÉèéÌÍìíÒÓòóÙÚùú", &extents);
    cairo_move_to           (cr, (w - extents.width) / 2, y);
    cairo_show_text         (cr, "ÀÁàáÈÉèéÌÍìíÒÓòóÙÚùú");
    y += extents.height + 10;

    cairo_text_extents      (cr, "(1234567890).:;,*!?'", &extents);
    cairo_move_to           (cr, (w - extents.width) / 2, y);
    cairo_show_text         (cr, "(1234567890).:;,*!?'");
    y += extents.height + 10;

    while( y < h) {
        cairo_set_font_size (cr, s);
        cairo_text_extents  (cr, _("The quick brown fox jumps over the lazy dog"), &extents);
        cairo_move_to       (cr, (w - extents.width) / 2, y);
        cairo_show_text     (cr, _("The quick brown fox jumps over the lazy dog"));
        y += extents.height + 10;
        s += 4;
    }

/*
    cairo_set_font_size     (cr, 72);
    cairo_text_extents      (cr, "Font Preview", &extents);
    cairo_move_to           (cr, (ttf->get_width()-extents.width)/2, y);
    cairo_show_text         (cr, "Font Preview");
    y += extents.height + 30;

    cairo_set_font_size     (cr, 48);
    cairo_text_extents      (cr, "ABCDEFGHIJKLMNOPQRSTVWXYZ", &extents);
    cairo_move_to           (cr, (ttf->get_width()-extents.width)/2, y);
    cairo_show_text         (cr, "ABCDEFGHIJKLMNOPQRSTVWXYZ");
    y += extents.height + 20;

    cairo_text_extents      (cr, "abcdefghijklmnopqrstvwxyz", &extents);
    cairo_move_to           (cr, (ttf->get_width()-extents.width)/2, y);
    cairo_show_text         (cr, "abcdefghijklmnopqrstvwxyz");
    y += extents.height + 20;

    cairo_text_extents      (cr, "ÀÁàáÈÉèéÌÍìíÒÓòóÙÚùú", &extents);
    cairo_move_to           (cr, (ttf->get_width()-extents.width)/2, y);
    cairo_show_text         (cr, "ÀÁàáÈÉèéÌÍìíÒÓòóÙÚùú");
    y += extents.height + 20;

    cairo_text_extents      (cr, "1234567890", &extents);
    cairo_move_to           (cr, (ttf->get_width()-extents.width)/2, y);
    cairo_show_text         (cr, "1234567890");
    y += extents.height + 30;


    cairo_set_font_size     (cr, 24);
    cairo_matrix_t matrix;
    cairo_matrix_t temp_matrix;
    cairo_matrix_init(&matrix, 1,0,-0.4,1,0,0);
    cairo_get_font_matrix(cr, &temp_matrix);
    cairo_matrix_multiply(&matrix, &matrix, &temp_matrix);
    cairo_set_font_matrix(cr, &matrix);

    cairo_text_extents      (cr, "Computer science is as much about computers", &extents);
    cairo_move_to           (cr, ttf->get_width()-extents.width-20, y);
    cairo_show_text         (cr, "Computer science is as much about computers");
    y += extents.height + 10;

    cairo_text_extents      (cr, "as astronomy is about telescopes.", &extents);
    cairo_move_to           (cr, ttf->get_width()-extents.width-20, y);
    cairo_show_text         (cr, "as astronomy is about telescopes.");
    y += extents.height + 10;

    cairo_set_font_matrix(cr, &temp_matrix);
    cairo_text_extents      (cr, "- Edsgar Dijkstra", &extents);
    cairo_move_to           (cr, ttf->get_width()-extents.width-20, y);
    cairo_show_text         (cr, "- Edsgar Dijkstra");
    * */
}
