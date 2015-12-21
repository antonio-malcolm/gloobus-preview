#include "gloobus-preview-plugin-manager.h"
#include <dlfcn.h>
#include <gtksourceview/gtksource.h>

vector<gp_plugin_container*> PluginManager::plugins;
map<string, int> PluginManager::filetypes;
int PluginManager::current_plugin = -1;

void PluginManager::register_plugin(string name) {
    gp_plugin_container * p = new gp_plugin_container;
    p->name = name;
    p->dl = NULL;
    plugins.push_back(p);
    current_plugin++;
}

void PluginManager::register_filetype(string type, int priority) {
    int pid = current_plugin;
    if( filetypes.find(type) == filetypes.end() ) {
        filetypes[type] = pid;
    } else {
        if(plugins[filetypes[type]]->types[type] < priority)
            filetypes[type] = pid;
    }
    plugins[pid]->types[type] = priority;
}

gp_plugin_controller * PluginManager::get_plugin(string type) {
    if( filetypes.find(type) == filetypes.end() ) {
        g_debug("Type %s not found in registered filetypes", type.c_str());
        return 0;
    } else {
        gp_plugin_container * plugin = plugins[filetypes[type]];
        if( !plugin->dl ) {
            g_debug("Loading %s.so", plugin->name.c_str());
            string path(PACKAGE_LIB_PATH);
            path = path + "/" + plugin->name + ".so";
            plugin->dl = dlopen(path.c_str(), RTLD_LAZY);
            if( !plugin->dl ) {
                g_debug("error loading: %s.so", plugin->name.c_str());
                return 0; // TODO: pass error message to Gloobus
            }
        }
        gp_plugin_controller * controller = new gp_plugin_controller;
        const char* dl_error = 0;
        version_t* getVersion = (version_t*)dlsym(plugin->dl, "getVersion");
        dl_error = dlerror();
        if (dl_error) {
            g_debug("error loading 'getVersion' function");
            return 0; // TODO: pass error message to Gloobus
        } else if(getVersion() != MANAGER_VERSION) {
            return 0; // TODO: pass error message to Gloobus
        }

        controller->create = (create_t*)dlsym(plugin->dl, "create");
        dl_error = dlerror();
        if (dl_error) {
            g_debug("error loading 'create' function");
            return 0; // TODO: pass error message to Gloobus
        }

        controller->destroy = (destroy_t*)dlsym(plugin->dl, "destroy");
        dl_error = dlerror();
        if (dl_error) {
            g_debug("error loading 'destroy' function");
            return 0; // TODO: pass error message to Gloobus
        }

        return controller;
    }
}

void PluginManager::unload_plugins() {
    int size = plugins.size();
    for(int i=0; i < size; i++)
        if(plugins[i]->dl)
            dlclose(plugins[i]->dl);
}

void PluginManager::show_supported_filetypes() {
    int size = plugins.size();
    for(int i=0 ; i<size ; i++) {
        g_print("%s:\n", plugins[i]->name.c_str());
        map<string,int>::iterator it;
        for (it=plugins[i]->types.begin();it!=plugins[i]->types.end();it++)
            g_print("  %s\n", (*it).first.c_str());
    }
}

//=========================================================================== //
/* ****************** LOADING ALL THE PLUGINS HEADERS HERE ****************** */
// TODO: make this in a standalone file

void PluginManager::load_plugins() {
#define PLUGIN_LOADER

#include "plugin-comic/comic-plugin.h"
#include "plugin-compressed/plugin-compressed.h"
#include "plugin-djvu/djvu-plugin.h"
#include "plugin-folder/plugin-folder.h"
#include "plugin-gstreamer-audio/plugin-gstreamer-audio.h"
#include "plugin-gstreamer-video/plugin-gstreamer-video.h"
#include "plugin-pdf/office-plugin.h"
#include "plugin-pdf/pdf-plugin.h"
#include "plugin-pixbuf/plugin-pixbuf.h"
#include "plugin-ps/ps-plugin.h"
#include "plugin-text/plugin-text.h"
#include "plugin-ttf/plugin-ttf.h"
#include "plugin-xps/xps-plugin.h"
#include "plugin-imagemagick/plugin-imagemagick.h"
}
