// =================================== GLOOBUS =============================================== //
// ==																						== //	
// ==  Gloobus, a quicklook for linux														== //
// ==																						== //
// ==	Project site: http://launchpad.net/gloobus 											== //
// ==																						== //
// ==	This is a quicklook for linux, it is based on a plugin architecture so everyone can == //
// ==   develop his own plugin to preview any filetype and do whatever he wants				== //
// ==																						== //
// ==   Plase, consider to donate cause it takes me a lot of time and efforts, and you know,== //
// ==   getting a recompense for you job allways makes you do it better						== //
// ==																						== //
// ==																						== //
// ==	Auhtor: Jordi Hernandez Puigdellívol <guitarboy000@gmail.com>						== //
// =========================================================================================== //

#ifndef _PMANAGER_
#define _PMANAGER_

#include <map>
#include <string>
#include <vector>

#include "gloobus-preview-interface-main.h"

using namespace std;

typedef interface* create_t();
typedef void destroy_t(interface*);
typedef int version_t();

struct gp_plugin_filetype {
	string filetype;
	int priority;
};

struct gp_plugin_controller {
	create_t * create;
	destroy_t * destroy;
};

struct gp_plugin_container {
	string name;
	void * dl;
	map<string, int> types;
};

class PluginManager {
	private:
		static	vector<gp_plugin_container*>	plugins;
		static	map<string, int>				filetypes;
		static	int								current_plugin;

	public:
		static	void					register_plugin			( string );
		static	void					register_filetype		( string,int );
		static gp_plugin_controller*	get_plugin				( string );
		static void					load_plugins			( void );
		static void					unload_plugins			( void );
		static void					show_supported_filetypes( void );
};

#endif
