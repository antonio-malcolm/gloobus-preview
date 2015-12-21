plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) $(FREETYPE_CFLAGS) $(CAIRO_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS) $(FREETYPE_LIBS) $(CAIRO_LIBS)

plugin_LTLIBRARIES = ttf.la 
ttf_la_SOURCES = plugin-ttf.cpp 
ttf_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = plugin-ttf.h
