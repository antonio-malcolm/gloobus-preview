#ifndef _IPIXBUF_
#define _IPIXBUF_

#ifndef PLUGIN_LOADER

#include <gdk-pixbuf/gdk-pixbuf.h>

//#include "../gloobus-preview-plugin-manager.h"
#include "../gloobus-preview-interface-image.h"

class iPixbuf : public iImage
{
    private:
        GtkWidget*      menuItemBackground;

	public:
								iPixbuf();
								~iPixbuf();
		virtual	GdkPixbuf*		get_pixbuf();
        static void             set_as_background_cb(void);
};

extern "C" interface * create() {
    return new iPixbuf;
}

extern "C" void destroy(interface * p) {
    delete (iPixbuf*)p;
}

extern "C" int getVersion() {
	return MANAGER_VERSION;
}

#endif

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("pixbuf");

GSList * suportedFormats = gdk_pixbuf_get_formats ();
GSList * node = suportedFormats;
for ( ; node != NULL; node = g_slist_next (node)) {
	char** mimes = gdk_pixbuf_format_get_mime_types(
		(GdkPixbufFormat*)node->data);
	PluginManager::register_filetype(mimes[0], 1);
	g_strfreev(mimes);
}
g_slist_free(suportedFormats);

#endif

#endif

