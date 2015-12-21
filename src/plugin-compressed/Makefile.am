plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) 
AM_LDFLAGS = $(GTK_LIBS) 

plugin_LTLIBRARIES = compressed.la 
compressed_la_SOURCES = plugin-compressed.cpp 
compressed_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = plugin-compressed.h
