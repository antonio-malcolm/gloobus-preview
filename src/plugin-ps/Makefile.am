plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) $(SPECTRE_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS) $(SPECTRE_LIBS)

plugin_LTLIBRARIES = ps.la
ps_la_SOURCES = ps-loader.cpp ps-plugin.cpp
ps_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = ps-loader.h ps-plugin.h
