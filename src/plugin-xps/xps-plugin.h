#ifndef XPS_PLUGIN_H
#define XPS_PLUGIN_H

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("xps");

PluginManager::register_filetype("application/oxps", 1);
PluginManager::register_filetype("application/vnd.ms-xpsdocument",1);

#endif

#endif /* XPS_PLUGIN_H */
