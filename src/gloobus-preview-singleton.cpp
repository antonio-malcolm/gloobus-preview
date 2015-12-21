/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** gloobus-preview
**
** gloobus-preview-singleton.cpp - singleton class that handles all plugins and
**                                 files, provides file information and
**                                 keyboard interraction
**
** Copyright (c) 2009-2010 Gloobus Developers
**
** Authors:
**    Jordi Puigdellívol Hernández <guitarboy000@gmail.com>
**    Alexandr Grigorcea <cahr.gr@gmail.com>
**
** This program is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License version 3, as published
** by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranties of
** MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
** PURPOSE.  See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

#include <algorithm>

#include <gdk/gdkkeysyms-compat.h>

#include "gloobus-preview-singleton.h"
#include "gloobus-preview-interface-main.h"
#include "gloobus-preview-interface-default.h"
#include "gloobus-preview-plugin-manager.h"

Gloobus * Gloobus::m_instance = 0;
Gloobus * Gloobus::instance()
{
    if (m_instance == NULL)
        m_instance = new Gloobus();
    return m_instance;
}

Gloobus::Gloobus()
    :config(new GloobusConfig)
    ,ui(0)
    ,m_plugin(0)
    ,m_file(0)
    ,m_fileinfo(0)
    ,m_gloobus_interface(0)
    ,m_error(0)
    ,m_slideshow(0)
{
    //g_debug("Creating Gloobus class");
}

int Gloobus::init( int *argc , char ***argv )
{
    config->load_config();
    config->parse_args(argc, argv);
    bool debug = config->get_cli_debug();
    g_log_set_handler (NULL, (GLogLevelFlags)(G_LOG_LEVEL_MASK
        | G_LOG_FLAG_FATAL
        | G_LOG_FLAG_RECURSION)
        ,log_handler, (void*)debug);

    if(debug) g_message(_("Debug mode: On"));

    PluginManager::load_plugins();

    switch(config->get_cli_action()) {
        case GP_ACTION_PREVIEW:
            show_preview( config->get_cli_target() );
            break;
        case GP_ACTION_FILETYPES:
            show_supported_filetypes();
            break;
        case GP_ACTION_HELP:
            show_help();
            break;
        case GP_ACTION_VERSION:
            show_version();
            break;
        case GP_ACTION_CONFIG:
            show_config();
            break;
        case GP_ACTION_UNKNOWN:
            break;
    }
    return 0;
}

void Gloobus::end()
{
    PluginManager::unload_plugins();
}

// =============================== CREATE PLUGIN ======================= //
// This functions searchs the plugin associated with the <m_filename> and then creates it.
// It also defines the variable <m_gloobus_interface> to be a polymorphism so the default functions
// can be called with directly on this variable
bool Gloobus::create_plugin( )
{
    m_plugin = PluginManager::get_plugin(get_filetype());
    if(!m_plugin) {
        m_class = "default"; // TODO: remove this when iDefault will be made better
        m_gloobus_interface = new iDefault();
    } else {
        m_class = ""; // TODO: remove this when iDefault will be made better
        m_gloobus_interface = m_plugin->create();
    }

    if(! m_gloobus_interface->set_filename(m_filename))
        return false;
    if(! m_gloobus_interface->load() )
        return false;

    return true;
}

void Gloobus::destroy_plugin()
{
    if(m_gloobus_interface) {
        m_gloobus_interface->end();
        g_debug("Destroying Plugin");
        if(m_class == "default") { // TODO: remove this when iDefault will be made better
            delete m_gloobus_interface;
        } else {
            //m_plugin->destroy(m_gloobus_interface);
            delete m_gloobus_interface;
            delete m_plugin;
            m_plugin = 0;
        }
        m_gloobus_interface = 0;
    }
}

void Gloobus::fullscreen( void )
{
    if(ui->is_fullscreen()) {
        ui->set_fullscreen(false);
        m_gloobus_interface->draw_unfullscreen(GTK_CONTAINER(ui->get_container()));
    } else {
        ui->set_fullscreen(true);
        m_gloobus_interface->draw_fullscreen(GTK_CONTAINER(ui->get_container()));
        m_gloobus_interface->draw_floating_toolbar();
    }
}

void Gloobus::defaultapp( void )
{
    //m_gloobus_interface->launch_default_app();
    g_app_info_launch_default_for_uri(m_uri, NULL, NULL);
    quit();
}

// comparison, not case sensitive.
bool compare_nocase (const std::string& first, const std::string& second)
{
  unsigned int i=0;
  while ( (i<first.length()) && (i<second.length()) )
  {
    if (tolower(first[i])<tolower(second[i])) return true;
    else if (tolower(first[i])>tolower(second[i])) return false;
    ++i;
  }
  return ( first.length() < second.length() );
}

void Gloobus::load_folder_filenames  ( void )
{
    string folder;
        GFile *directory;

    directory = g_file_get_parent(m_file);

    GFileEnumerator * directory_files = g_file_enumerate_children(directory,
        G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, NULL);

    GFileInfo * info;

    g_debug("FOLDER FILES");
    do {
        info = g_file_enumerator_next_file   (directory_files, NULL, NULL);
        if(info) {
            g_debug("    Files: %s",g_file_info_get_name(info));
            m_files.push_back (string(g_file_get_uri(g_file_get_child(directory, g_file_info_get_name(info)))));

            g_object_unref(info);
        }
    }while(info!=NULL);

    g_object_unref(directory);
    g_object_unref(directory_files);

    m_it = m_files.begin();
    std::sort( m_files.begin(), m_files.end(), compare_nocase);
}

bool Gloobus::search_actual_filename ( void )
{
    for ( m_it=m_files.begin() ; m_it < m_files.end() ; m_it++ ) {
        string uri = *m_it;
        if(g_strcmp0 (uri.c_str(), m_uri) == 0) {
             return true;
        }
    }
    return false;
}

void Gloobus::clean_current( void )
{
    if(m_fileinfo)
        g_object_unref(m_fileinfo);
    if(m_file)
        g_object_unref(m_file);
    if(m_uri)
        g_free(m_uri);
}

void Gloobus::init_file ( string uri )
{
        printf ("test %s\n", uri.c_str());
    m_file = g_file_new_for_uri (uri.c_str());
        m_uri = (char *) uri.c_str();
        char *pname = g_file_get_parse_name(m_file);
        m_filename = string (pname);
        g_free (pname);
}

// ============================= Load Next File ============================== //
void Gloobus::next_file ( void )
{
    if (m_it == m_files.end()-1 or m_it == m_files.end()) {
        g_debug("Going to the start of file list");
        m_it = m_files.begin();
    } else {
        m_it++;
    }
        //clean_current();
        init_file (*m_it);
    //m_filename = *m_it;
    load_preview();
}
// =============================== Load prev File ============================ //
void Gloobus::prev_file ( void )
{
    if (m_it == m_files.begin())  {
        g_debug("Going to the end of file list");
        m_it = m_files.end()-1;
    } else {
        m_it--;
    }
        //clean_current();
        init_file (*m_it);
    //m_filename = *m_it;
    load_preview();
}
// =============================== TRASH FILE ================================ //
void Gloobus::trash_file( void )
{
    g_debug("Sending file to trash");
    //m_gloobus_interface->trash_file();
    g_file_trash(m_file,NULL,NULL);

    if(m_it == m_files.end()-1)
    {
        m_files.erase(m_it);
        m_it = m_files.begin();
    }
    else
    {
        m_it = m_files.erase(m_it);
    }

    m_filename = *m_it;
    load_preview();
}

// ===================================== KEY_PRESS =================================== //
// This function is called when there is a key press event
gboolean Gloobus::key_press_cb ( GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    //g_message ("Key Pressed %s (%i)",gdk_keyval_name(event->keyval),event->keyval);
    if(event->keyval == GDK_space || event->keyval == GDK_Escape) {
        Gloobus::instance()->quit();
        return true;
    }

    if(event->keyval == GDK_f) {
        Gloobus::instance()->fullscreen();
        return true;
    }

    if(event->keyval == GDK_s) {
        Gloobus::instance()->slideshow();
        return true;
    }

    if(event->keyval == GDK_Right) {
        Gloobus::instance()->next_file();
        return true;
    }


    if(event->keyval == GDK_Left) {
        Gloobus::instance()->prev_file();
        return true;
    }

    if(event->keyval == GDK_Return) {
        Gloobus::instance()->defaultapp();
        //Gloobus::instance()->quit();
        return true;
    }

    if(event->keyval == GDK_Delete) {
        Gloobus::instance()->trash_file();
        return true;
    }

    if(Gloobus::instance()->m_gloobus_interface) {
        return Gloobus::instance()->m_gloobus_interface->key_press_event(event->keyval);
        //return true; //TODO: make interface return true or false
    }
    return false;
}

void Gloobus::quit( void ) {
    g_debug("Gloobus is quitting");

    destroy_plugin();

    gtk_main_quit();
}

bool Gloobus::load_file( void ) {
    g_debug("Loading File");
        /* amtest: wrong place to unref */
    /*if(m_fileinfo)
        g_object_unref(m_fileinfo);
    if(m_file)
        g_object_unref(m_file);*/

    //m_file = g_file_new_for_path (m_filename.c_str () );
    m_fileinfo = g_file_query_info(m_file, "*", G_FILE_QUERY_INFO_NONE,
        NULL, &m_error);
    if( !m_fileinfo ) {
        if( !m_error ) {
            g_error_new(g_quark_from_string ("Gloobus"), 3,
                _("Error loading file info"));
        }
        return false;
    }
    return true;
}

void Gloobus::load_preview( void ) {
    //g_debug(">>>> Loading file: %s", m_filename.c_str());
    g_debug(">>>> Loading file: %s", m_uri);
    destroy_plugin();
    if(m_error) {
        g_error_free(m_error);
        m_error = 0;
    }
    if( load_file() ) {
        if( create_plugin() ) {
            ui->prepare_window();
                        char *basen = g_file_get_basename(m_file);
            ui->set_title(basen);
                        g_free(basen);
            m_gloobus_interface->show();
        } else {
            m_error = m_gloobus_interface->get_error();
            show_error();
        }
    } else {
        show_error();
    }
}

// ============================ SLIDESHOW ===================================== //
void Gloobus::slideshow(void){

    if(m_slideshow)        //stop slideshow
    {
        g_debug(_("Stop slideshow"));
        g_source_remove(m_slideshowID);
        m_slideshow = false;
    }
    else                    //start slideshow
    {
        g_debug(_("Start slideshow"));
        m_slideshow = true;
        m_slideshowID = g_timeout_add(4000, &slideshow_cb, this);
    }
}

gboolean Gloobus::slideshow_cb(gpointer data)
{
    //TODO: If is media, wait till it finishes
    Gloobus *g = (Gloobus*)data;
    if(!g->m_gloobus_interface->is_slideshow_blocked())
    {
        g->next_file();
    }
    return true;
}

/* ****************************** GET SECTION ******************************* */

GloobusConfig* Gloobus::get_config( void ) {
    return config;
}

GloobusUI* Gloobus::get_ui( void ) {
    return ui;
}

GFile* Gloobus::get_file( void ) {
    return m_file;
}

GFileInfo* Gloobus::get_fileinfo( void ) {
    return m_fileinfo;
}

string Gloobus::get_filetype( void ) {
    string type = g_file_info_get_attribute_string (m_fileinfo,
        "standard::content-type" );
    type = type.substr(0,type.find(";"));
    type = type.substr(0,type.find(","));
    if(type == "x-directory/normal")
        type = "folder";
    return type;
}

string Gloobus::get_filename( void ) {
    return m_filename;
}

goffset Gloobus::get_file_size()
{
    const gchar *type = g_file_info_get_content_type(m_fileinfo);
    if(g_strcmp0("inode/directory", type) == 0) {
        return -1;
    }
    return g_file_info_get_size(m_fileinfo);
}

int Gloobus::get_width(void) {
    return m_gloobus_interface->get_width();
}

int Gloobus::get_height(void) {
    return m_gloobus_interface->get_height();
}

/* ****************************** SHOW SECTION ****************************** */

void Gloobus::show_error( void ) {
    if( m_gloobus_interface ) {
        m_error = m_gloobus_interface->get_error();
        destroy_plugin();
        m_gloobus_interface = new iDefault();
    } else {
        m_gloobus_interface = new iError();
    }
    //g_warning(m_error->message);
    m_class = "default";
    m_gloobus_interface->set_filename(m_filename);
    m_gloobus_interface->set_error(g_error_copy(m_error));
    m_gloobus_interface->load();

    ui->prepare_window();
    string title = m_filename.substr(m_filename.rfind("/")+1,-1);
    ui->set_title(title.c_str());
    m_gloobus_interface->show();
}

void Gloobus::show_help( void )
{
    printf(
        "%s:\n"
        "   gloobus-preview [%s...] [URI]\n\n",
        _("Usage"), _("OPTION") );

    printf(
        "   %s\n\n", _(
        "If URI is not provided Gloobus-Preview checks clipboard "
        "for filename"));

    printf("%s:\n", _("Help options") );
    printf("   -h, --help                   %s\n",
        _("Show help options") );

    printf("%s:\n", _("Application options") );
    printf("   -d, --debug                  %s\n",
        _("Show debug messages") );
    printf("   --show-supported-filetypes   %s\n",
        _("Show supported filetypes") );

    printf("\n");
}

void Gloobus::show_version( void )
{
    printf ("%1$s %2$s\n", PACKAGE, VERSION);
}

void Gloobus::show_preview(char * target)
{
    //string filename;
    ui = new GloobusUI();

    gtk_widget_add_events(ui->get_window(), GDK_BUTTON_PRESS_MASK);

    g_signal_connect_after (ui->get_window(), "key-release-event",
        G_CALLBACK(key_press_cb), NULL);
/* amtest to remove
** is clipboard really needed since we support gloobus call from nautilus ? */
#if 0
    if(target) {
        filename = target;

        // check relative path:
        if (filename.c_str()[0] != '/') {
            filename = "./" + filename;
        }

        // strip final slash:
        if (filename.c_str()[filename.size()-1] == '/') {
            filename = filename.substr(0,filename.size()-1);
        }
    } else {
        g_debug("Getting file from clipboard");
        GtkClipboard *clip  = gtk_clipboard_get ( GDK_SELECTION_CLIPBOARD );
        gchar **uri         = gtk_clipboard_wait_for_uris ( clip );
        if(uri == NULL) {\
            m_error = g_error_new(g_quark_from_string ("Gloobus"), 1,
                _("There is no file in clipboard"));
        } else {
            filename = g_filename_from_uri(uri[0], NULL, NULL);
        }
    }
    //init( filename , win );
    m_filename = filename;
#endif
        m_file = g_file_new_for_commandline_arg (target);
        /*m_path = g_file_get_path (m_file);
        g_debug ("path: %s", m_path);*/
        m_uri = g_file_get_uri (m_file);
        g_debug ("uri: %s", m_uri);
        //m_filename = string (m_path);

        char *pname = g_file_get_parse_name(m_file);
        g_debug ("parse_name: %s\n", pname);
        m_filename = string (pname);
        g_free (pname);

        load_folder_filenames();
        if( !search_actual_filename()) {
            m_error = g_error_new(g_quark_from_string ("Gloobus"), 2,
                _("File not found"));
        } else {
            load_preview();
        }

    if(m_error)
        show_error();

        gtk_main();
    destroy_plugin(); //Destroy current plugin // TODO: do we really need this?
    end(); //Unload All plugins before leave // TODO: do we really need this?
}

void Gloobus::show_config( void )
{
    //TODO: make GloobusConfigWindow class and use it here
}

void Gloobus::show_supported_filetypes( void )
{
    PluginManager::load_plugins ();
    PluginManager::show_supported_filetypes();
    PluginManager::unload_plugins();
}

void Gloobus::log_handler  (const gchar *log_domain,
    GLogLevelFlags log_level,
    const gchar *message,
    gpointer debug)
{
    if(!(bool)debug && G_LOG_LEVEL_DEBUG == log_level) {
        //do nothing
    } else {
        char * add = NULL;
        if (log_level==  G_LOG_LEVEL_MESSAGE) {
            add = g_strdup(CLI_COLOR_BOLD    "[INFO]"     CLI_COLOR_RESET);
        } else  if (log_level == G_LOG_LEVEL_DEBUG)   {
            add = g_strdup(CLI_COLOR_BLUE    "[DEBUG]"    CLI_COLOR_RESET);
        } else  if (log_level==  G_LOG_LEVEL_WARNING) {
            add = g_strdup(CLI_COLOR_RED     "[WARNING]"  CLI_COLOR_RESET);
        } else if (log_level==  G_LOG_LEVEL_CRITICAL) {
            add = g_strdup(CLI_COLOR_DARKRED "[CRITICAL]" CLI_COLOR_RESET);
        }
        g_print("%s %s\n", add, message);
        g_free(add);
    }
}
