/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** gloobus-preview
**
** gloobus-preview-singleton.h - singleton class that handles all plugins and
**                               files, provides file information and
**                               keyboard interraction
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

#ifndef _GLOOBUS_
#define _GLOOBUS_

#include <string>
#include <vector>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <X11/Xlib.h>

#include "gloobus-preview-config.h"
#include "gloobus-preview-defines.h"
#include "gloobus-preview-window.h"

struct gp_plugin_controller;
class interface;

using namespace std;

class Gloobus {
        // =========== Singleton ========= //
        static Gloobus *m_instance;
        Gloobus();
        Gloobus& operator=(Gloobus&);  // Disallowed
        Gloobus(const Gloobus&);       // Disallowed

private:
        GloobusConfig           *config;
        GloobusUI           *ui;

        gp_plugin_controller            *m_plugin;
        GFile               *m_file;
        GFileInfo           *m_fileinfo;
        string              m_class;
        vector<string>          m_files;
        vector<string>::iterator    m_it;

        string                  m_filename; /* amtest to remove */
        //char                            *m_path;
        char                            *m_uri;
        interface           *m_gloobus_interface; //Polymorphism
        GError              *m_error;
        bool                            m_slideshow;
        int                             m_slideshowID;

public:
        /* return pointer to Gloobus singleton object
         * */
        static  Gloobus*        instance( void );

        /* initilizes Gloobus class
         * */
        int             init            ( int* , char *** );
        void    clean_current( void );
        void    init_file ( string uri);

        /* show the preview of selected file
         * */
        void            show_preview    ( char* );

        /* show configuration window
         * */
        void            show_config     ( void );

        /* print to stdout all supported mime types
         * */
        void            show_supported_filetypes( void );

        /* print to stdout help message
         * */
        void            show_help       ( void );

        /* print to stdout version information
         * */
        void            show_version       ( void );

        /* shows error message window if file can not be accessed
         * or loads default plugin if file can be accessed
         * */
        void            show_error      ( void );

        /* unloads all the plugins
         * */
        void            end             ( void );

        /* loads the plugin for current file
         * */
        bool            create_plugin   ( void );

        /* destroys the plugin for current file
         * */
        void            destroy_plugin  ( void );

        /* makes windows fullscreen
         * */
        void            fullscreen      ( void );

        /* launches default application for current file
         * */
        void            defaultapp      ( void );

        /* Starts or stops the slideshow
         * */
        void            slideshow       ( void );
        static gboolean slideshow_cb    ( gpointer );

        /* load all files in current working directory
         * */
        void            load_folder_filenames( void );

        /* searches file in current working directory
         * and moves iterator to this file
         * if file is not found, iterator is moved at the begining
         * */
        bool            search_actual_filename( void );

        /* moves interator to the next file and calls load_preview
         * */
        void            next_file       ( void );

        /* moves interator to the previous file and calls load_preview
         * */
        void            prev_file       ( void );

        /* deletes current file
         * */
        void            trash_file      ( void );

        /* key press event callback
         * */
        static  gboolean    key_press_cb    ( GtkWidget * , GdkEventKey*, gpointer );


        /* stops everything and exits
         * */
        void            quit            ( void );

        /* loads current file info
         * */
        bool            load_file       ( void );

        /* load file, load plugin, prepares window and shows preview
         * if not first file, previous plugin is destroyed
         * */
        void            load_preview    ( void );

        /* return current files GFile object
         * */
        GFile*          get_file        ( void );

        /* return current files file info
         * */
        GFileInfo*      get_fileinfo    ( void );

        /* return current files mime type
         * */
        string          get_filetype    ( void );

        /* return current files filename
         * */
        string          get_filename    ( void );

        goffset get_file_size();

        /* return a pointer to gloobus-preview configuration object
         * */
        GloobusConfig*          get_config      ( void );

        /* returns a pointer to gloobus-preview ui class
         * */
        GloobusUI*      get_ui          ( void );

        /* return current plugin height
         * */
        int         get_height      ( void );

        /* return current plugin width
         * */
        int         get_width       ( void );

        static  void        log_handler  (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer debug);
};

#endif
