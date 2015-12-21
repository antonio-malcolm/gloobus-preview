plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) 
AM_LDFLAGS = $(GTK_LIBS) 

plugin_LTLIBRARIES = imagemagick.la 
imagemagick_la_SOURCES = plugin-imagemagick.cpp 
imagemagick_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = plugin-imagemagick.h
