#ifndef PS_PLUGIN_H
#define PS_PLUGIN_H

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("ps");

PluginManager::register_filetype("application/postscript", 1);
PluginManager::register_filetype("application/x-bzpostscript", 1);
PluginManager::register_filetype("application/x-gzpostscript", 1);
PluginManager::register_filetype("image/x-eps", 1);
PluginManager::register_filetype("image/x-bzeps", 1);
PluginManager::register_filetype("image/x-gzeps", 1);

#endif

#endif /* PS_PLUGIN_H */

