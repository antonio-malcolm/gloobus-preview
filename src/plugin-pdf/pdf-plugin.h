#ifndef PDF_PLUGIN_H
#define PDF_PLUGIN_H

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("pdf");

PluginManager::register_filetype("application/pdf", 1);
PluginManager::register_filetype("application/illustrator",1);

#endif

#endif /* PDF_PLUGIN_H */