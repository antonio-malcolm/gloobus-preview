#ifndef DJVU_PLUGIN_H
#define DJVU_PLUGIN_H

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("djvu");

PluginManager::register_filetype("image/vnd.djvu", 1);

#endif

#endif /* DJVU_PLUGIN_H */

