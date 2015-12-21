plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) $(GTKSOURCEVIEW_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS) $(GTKSOURCEVIEW_LIBS)

plugin_LTLIBRARIES = text.la 
text_la_SOURCES = plugin-text.cpp 
text_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)


noinst_HEADERS = plugin-text.h
