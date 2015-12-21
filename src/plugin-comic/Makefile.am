plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS)

plugin_LTLIBRARIES = comic.la
comic_la_SOURCES = comic-loader.cpp comic-plugin.cpp
comic_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = comic-loader.h comic-plugin.h
