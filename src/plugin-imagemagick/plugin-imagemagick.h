#ifndef _IIMAGEMAGICK_
#define _IIMAGEMAGICK_

#ifndef PLUGIN_LOADER

#include <gdk-pixbuf/gdk-pixbuf.h>

//#include "../gloobus-preview-plugin-manager.h"
#include "../gloobus-preview-interface-image.h"

class iImagemagick : public iImage
{
	public:
	virtual		GdkPixbuf*		get_pixbuf	();
	private:
		gchar * getPngUri (GFile *file);
};

extern "C" interface * create() {
    return new iImagemagick;
}

extern "C" void destroy(interface * p) {
    delete (iImagemagick*)p;
}

extern "C" int getVersion() {
	return MANAGER_VERSION;
}

#endif

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("imagemagick");

if(interface::is_program_available("convert")) {
	PluginManager::register_filetype("application/dicom", 1);
	PluginManager::register_filetype("application/illustrator", 1);
	PluginManager::register_filetype("application/pdf", 1);
	PluginManager::register_filetype("application/postscript", 1);
	PluginManager::register_filetype("application/x-bzpdf", 1);
	PluginManager::register_filetype("image/bmp", 1);
	PluginManager::register_filetype("image/cgm", 1);
	PluginManager::register_filetype("image/dpx", 1);
	PluginManager::register_filetype("image/fax-g3", 1);
	PluginManager::register_filetype("image/fits", 1);
	PluginManager::register_filetype("image/gif", 1);
	PluginManager::register_filetype("image/ief", 1);
	PluginManager::register_filetype("image/jp2", 1);
	PluginManager::register_filetype("image/jpeg", 1);
	PluginManager::register_filetype("image/png", 1);
	PluginManager::register_filetype("image/rle", 1);
	PluginManager::register_filetype("image/svg+xml-compressed", 1);
	PluginManager::register_filetype("image/svg+xml", 1);
	PluginManager::register_filetype("image/tiff", 1);
	PluginManager::register_filetype("image/vnd.djvu", 1);
	PluginManager::register_filetype("image/vnd.dwg", 1);
	PluginManager::register_filetype("image/vnd.dxf", 1);
	PluginManager::register_filetype("image/vnd.adobe.photoshop", 1);
	PluginManager::register_filetype("image/vnd.microsoft.icon", 1);
	PluginManager::register_filetype("image/vnd.rn-realpix", 1);
	PluginManager::register_filetype("image/vnd.wap.wbmp", 1);
	PluginManager::register_filetype("image/webp", 1);
	PluginManager::register_filetype("image/x-3ds", 1);
	PluginManager::register_filetype("image/x-adobe-dng", 1);
	PluginManager::register_filetype("image/x-applix-graphics", 1);
	PluginManager::register_filetype("image/x-bzeps", 1);
	PluginManager::register_filetype("image/x-canon-cr2", 1);
	PluginManager::register_filetype("image/x-canon-crw", 1);
	PluginManager::register_filetype("image/x-cmu-raster", 1);
	PluginManager::register_filetype("image/x-compressed-xcf", 1);
	PluginManager::register_filetype("image/x-dds", 1);
	PluginManager::register_filetype("image/x-dib", 1);
	PluginManager::register_filetype("image/x-emf", 1);
	PluginManager::register_filetype("image/x-eps", 1);
	PluginManager::register_filetype("image/x-fits", 1);
	PluginManager::register_filetype("image/x-fpx", 1);
	PluginManager::register_filetype("image/x-fuji-raf", 1);
	PluginManager::register_filetype("image/x-gzeps", 1);
	PluginManager::register_filetype("image/x-icns", 1);
	PluginManager::register_filetype("image/x-ico", 1);
	PluginManager::register_filetype("image/x-iff", 1);
	PluginManager::register_filetype("image/x-ilbm", 1);
	PluginManager::register_filetype("image/x-jng", 1);
	PluginManager::register_filetype("image/x-kodak-dcr", 1);
	PluginManager::register_filetype("image/x-kodak-k25", 1);
	PluginManager::register_filetype("image/x-kodak-kdc", 1);
	PluginManager::register_filetype("image/x-lwo", 1);
	PluginManager::register_filetype("image/x-lws", 1);
	PluginManager::register_filetype("image/x-macpaint", 1);
	PluginManager::register_filetype("image/x-minolta-mrw", 1);
	PluginManager::register_filetype("video/x-mng", 1);
	PluginManager::register_filetype("image/x-msod", 1);
	PluginManager::register_filetype("image/x-niff", 1);
	PluginManager::register_filetype("image/x-nikon-nef", 1);
	PluginManager::register_filetype("image/x-olympus-orf", 1);
	PluginManager::register_filetype("image/x-panasonic-raw", 1);
	PluginManager::register_filetype("image/x-pcx", 1);
	PluginManager::register_filetype("image/x-pentax-pef", 1);
	PluginManager::register_filetype("image/x-photo-cd", 1);
	PluginManager::register_filetype("image/x-pict", 1);
	PluginManager::register_filetype("image/x-portable-anymap", 1);
	PluginManager::register_filetype("image/x-portable-bitmap", 1);
	PluginManager::register_filetype("image/x-portable-graymap", 1);
	PluginManager::register_filetype("image/x-portable-pixmap", 1);
	PluginManager::register_filetype("image/x-psd", 1);
	PluginManager::register_filetype("image/x-quicktime", 1);
	PluginManager::register_filetype("image/x-sgi", 1);
	PluginManager::register_filetype("image/x-sigma-x3f", 1);
	PluginManager::register_filetype("image/x-sony-arw", 1);
	PluginManager::register_filetype("image/x-sony-sr2", 1);
	PluginManager::register_filetype("image/x-sony-srf", 1);
	PluginManager::register_filetype("image/x-sun-raster", 1);
	PluginManager::register_filetype("image/x-tga", 1);
	PluginManager::register_filetype("image/x-win-bitmap", 1);
	PluginManager::register_filetype("image/x-wmf", 1);
	PluginManager::register_filetype("image/x-xbitmap", 1);
	PluginManager::register_filetype("image/x-xcf", 1);
	PluginManager::register_filetype("image/x-xcursor", 1);
	PluginManager::register_filetype("image/x-xfig", 1);
	PluginManager::register_filetype("image/x-xpixmap", 1);
	PluginManager::register_filetype("image/x-xwindowdump", 1);
}

#endif

#endif

