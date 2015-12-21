#ifndef _ICOMPRESSED_
#define _ICOMPRESSED_

#define GP_COMPRESSED_7Z       "application/x-7z-compressed"
#define GP_COMPRESSED_AR       "application/x-archive"
#define GP_COMPRESSED_CAB      "application/vnd.ms-cab-compressed"
#define GP_COMPRESSED_CPIO     "application/x-cpio"
#define GP_COMPRESSED_DEB      "application/x-deb"
#define GP_COMPRESSED_ISO      "application/x-cd-image"
#define GP_COMPRESSED_LZIP     "application/x-lzip"
#define GP_COMPRESSED_RAR      "application/x-rar"
#define GP_COMPRESSED_RPM      "application/x-rpm"
#define GP_COMPRESSED_TAR      "application/x-tar"
#define GP_COMPRESSED_TAR_BZIP "application/x-bzip-compressed-tar"
#define GP_COMPRESSED_TAR_GZIP "application/x-compressed-tar"
#define GP_COMPRESSED_TAR_LZMA "application/x-lzma-compressed-tar"
#define GP_COMPRESSED_TAR_LZOP "application/x-tzo"
#define GP_COMPRESSED_TAR_XZ   "application/x-xz-compressed-tar"
#define GP_COMPRESSED_TAR_Z    "application/x-tarz"
#define GP_COMPRESSED_ZIP      "application/zip"

#ifndef PLUGIN_LOADER

//#include <cstdlib>
//#include <csignal>
//#include <stdlib.h>
//#include <string>

/*Translations*/
//#include <libintl.h>
//#include <locale.h>
//#define _(String) gettext(String)


//#include <sys/types.h>
//#include <dirent.h>
//#include <errno.h>
#include <string>
#include <vector>
//#include <iostream>
//#include <algorithm>

//#include <gio/gio.h>

//#include "../gloobus-preview-plugin-manager.h"
#include "../gloobus-preview-interface-list.h"

class iCompressed : public iList
{
	private:
		vector<string>		m_files;
		//vector<string>		m_size;
		bool				m_finished;
		GIOChannel		*	m_io_channel;
		vector<string>		cmd_args;
		GPid				cmd_pid;
		bool				m_list_started;
		guint				wait_id;

		bool (iCompressed::*line_parser)( gchar *);
	
	public:
			iCompressed();
			~iCompressed();
		virtual	GdkPixbuf*		get_pixbuf  		( void );
		virtual	GtkListStore*	get_list_store		( void );
		virtual	int				get_n_columns 		( void );
		virtual	string			get_column_title 	( int i);
		virtual	int				get_n_items 		( ) ;
		virtual	string			get_item			( int i, int j ) ;
		virtual	GdkPixbuf*		get_item_pixbuf		( int i, int j) {return NULL;};
		virtual	bool			is_column_pixbuf	( int i);
		virtual	bool			has_items			( void );
				bool				get_archive_files	(  );

		static void wait_child( GPid, gint, gpointer );

	private:
		void add_cmd_arg( string );
		bool exec_cmd( void );

		bool parse_line_7z( gchar * );
		bool parse_line( gchar * );
};


extern "C" interface * create() {		
    return new iCompressed;
}

extern "C" void destroy(interface * p) {
    delete (iCompressed*)p;
}

extern "C" int getVersion() {
	return MANAGER_VERSION;
}

#endif

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("compressed");

if(interface::is_program_available("bsdtar")) {
	PluginManager::register_filetype(GP_COMPRESSED_7Z, 1);
	PluginManager::register_filetype(GP_COMPRESSED_AR, 1);
	PluginManager::register_filetype(GP_COMPRESSED_CAB, 1);
	PluginManager::register_filetype(GP_COMPRESSED_CPIO, 1);
	PluginManager::register_filetype(GP_COMPRESSED_DEB, 1);
	PluginManager::register_filetype(GP_COMPRESSED_ISO, 1);
	PluginManager::register_filetype(GP_COMPRESSED_LZIP, 1);
	PluginManager::register_filetype(GP_COMPRESSED_RAR, 1);
	PluginManager::register_filetype(GP_COMPRESSED_RPM, 1);
	PluginManager::register_filetype(GP_COMPRESSED_TAR, 1);
	PluginManager::register_filetype(GP_COMPRESSED_TAR_BZIP, 1);
	PluginManager::register_filetype(GP_COMPRESSED_TAR_GZIP, 1);
	PluginManager::register_filetype(GP_COMPRESSED_TAR_LZMA, 1);
	PluginManager::register_filetype(GP_COMPRESSED_TAR_LZOP, 1);
	PluginManager::register_filetype(GP_COMPRESSED_TAR_XZ, 1);
	PluginManager::register_filetype(GP_COMPRESSED_TAR_Z, 1);
	PluginManager::register_filetype(GP_COMPRESSED_ZIP, 1);
} else {
	if(interface::is_program_available("7z")) {
		PluginManager::register_filetype(GP_COMPRESSED_7Z, 1);
	}

	if(interface::is_program_available("unrar")) {
		PluginManager::register_filetype(GP_COMPRESSED_RAR, 1);
	}

	if(interface::is_program_available("tar")) {
		PluginManager::register_filetype(GP_COMPRESSED_TAR, 1);
		PluginManager::register_filetype(GP_COMPRESSED_TAR_BZIP, 1);
		PluginManager::register_filetype(GP_COMPRESSED_TAR_GZIP, 1);
		PluginManager::register_filetype(GP_COMPRESSED_TAR_LZMA, 1);
		PluginManager::register_filetype(GP_COMPRESSED_TAR_LZOP, 1);
		PluginManager::register_filetype(GP_COMPRESSED_TAR_XZ, 1);
		PluginManager::register_filetype(GP_COMPRESSED_TAR_Z, 1);
	}

	if(interface::is_program_available("zipinfo")) {
		PluginManager::register_filetype(GP_COMPRESSED_AR, 1);
		PluginManager::register_filetype(GP_COMPRESSED_CPIO, 1);
		PluginManager::register_filetype(GP_COMPRESSED_ISO, 1);
		PluginManager::register_filetype(GP_COMPRESSED_ZIP, 1);
	}
}

#endif

#endif
