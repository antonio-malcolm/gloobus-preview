plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) 
AM_LDFLAGS = $(GTK_LIBS) 

plugin_LTLIBRARIES = pixbuf.la 
pixbuf_la_SOURCES = plugin-pixbuf.cpp 
pixbuf_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = plugin-pixbuf.h
