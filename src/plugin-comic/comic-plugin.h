#ifndef COMIC_PLUGIN_H
#define COMIC_PLUGIN_H

#include "../gloobus-preview-interface-main.h"

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("comic");

if(interface::is_program_available("bsdtar")) {
	PluginManager::register_filetype("application/x-cbr", 1);
	PluginManager::register_filetype("application/x-cbt", 1);
	PluginManager::register_filetype("application/x-cbz", 1);
} else {
	if(interface::is_program_available("unrar")) {
		PluginManager::register_filetype("application/x-cbr", 1);
	}
	if(interface::is_program_available("tar")) {
		PluginManager::register_filetype("application/x-cbt", 1);
	}
	if(interface::is_program_available("unzip")) {
		PluginManager::register_filetype("application/x-cbz", 1);
	}
}

#endif

#endif /* COMIC_PLUGIN_H */

