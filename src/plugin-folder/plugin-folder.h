#ifndef _IFOLDER_
#define _IFOLDER_

#ifndef PLUGIN_LOADER

//#include <cstdlib>
//#include <string>

/*Translations*/
//#include <libintl.h>
//#include <locale.h>
//#define _(String) gettext(String)


//#include <sys/types.h>
//#include <dirent.h>
//#include <errno.h>
#include <vector>
#include <string>
//#include <iostream>
//#include <algorithm>

//#include <gio/gio.h>

//#include "../gloobus-preview-plugin-manager.h"
#include "../gloobus-preview-interface-list.h"

class iFolder : public iList
{
	private:
	vector<string>			m_files;
	vector<string>			m_size;
	vector<GIcon*>			m_icons;
	vector<GdkPixbuf*>		m_icon_info;
	int n_files;
	int n_current;
	
	public:
				iFolder();
				~iFolder();

	virtual		GdkPixbuf*		get_pixbuf  		( void );
	virtual		GtkListStore*  	get_list_store		( void );
	virtual		int 			get_n_columns 		( void );
	virtual		string			get_column_title 	( int i );
	virtual		int 			get_n_items 		( ) ;
	virtual		string 			get_item			( int i, int j) ;
	virtual		GdkPixbuf*		get_item_pixbuf		( int i, int j) ;
	virtual		bool			is_column_pixbuf	( int i);
	virtual		bool			has_items			( void );
				int				get_folder_files	( string, vector<string>&);
};


extern "C" interface * create() {
    return new iFolder;
}

extern "C" void destroy(interface * p) {
    delete (iFolder*)p;
}

extern "C" int getVersion() {
	return MANAGER_VERSION;
}

#endif

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("folder");

PluginManager::register_filetype("inode/directory", 1);

#endif

#endif

