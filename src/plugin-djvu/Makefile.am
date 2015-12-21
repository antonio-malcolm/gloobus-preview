plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) $(DJVU_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS) $(DJVU_LIBS)

plugin_LTLIBRARIES = djvu.la
djvu_la_SOURCES = djvu-loader.cpp djvu-plugin.cpp
djvu_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = djvu-loader.h djvu-plugin.h
