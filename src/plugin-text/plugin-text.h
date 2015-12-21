#ifndef _IPLAIN_
#define _IPLAIN_

#ifndef PLUGIN_LOADER

#include <gtksourceview/gtksource.h>
#include <gio/gio.h>

#include "../gloobus-preview-config.h"
#include "../gloobus-preview-interface-text.h"

//sudo ln -s /usr/include/gtksourceview-2.0/gtksourceview/ /usr/include/gtksourceview

class iPlain : public iText
{
	public:
		virtual	GtkSourceBuffer*	get_textbuf	(void);
};

extern "C" interface * create() {
	return new iPlain;
}

extern "C" void destroy(interface * p) {
	delete (iPlain*)p;
}

extern "C" int getVersion() {
	return MANAGER_VERSION;
}

#endif

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("text");

PluginManager::register_filetype("text/plain", 1);
PluginManager::register_filetype("text/csv", 1);
PluginManager::register_filetype("text/x-log", 1);
PluginManager::register_filetype("text/x-nfo", 1);
PluginManager::register_filetype("text/x-readme", 1);
PluginManager::register_filetype("application/x-extension-xhtml", 1);
PluginManager::register_filetype("application/x-subrip", 1);

GtkSourceLanguageManager * lm;
GtkSourceLanguage * lg;
const gchar * const * ids;

lm = gtk_source_language_manager_get_default ( );
ids = gtk_source_language_manager_get_language_ids(lm);
for(int i=0; ids[i] != NULL; i++) {
	lg = gtk_source_language_manager_get_language(lm,ids[i]);
	gchar ** mimes = gtk_source_language_get_mime_types(lg);
	g_debug("  Language %s", gtk_source_language_get_name(lg));

	if(mimes) {
		for(int j=0; mimes[j] != NULL; j++) {
			g_debug("    %s", mimes[j]);
			PluginManager::register_filetype(mimes[j], 1);
		}
	}
	g_strfreev(mimes);
}

#endif

#endif

