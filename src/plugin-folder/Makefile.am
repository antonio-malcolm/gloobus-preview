plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) 
AM_LDFLAGS = $(GTK_LIBS) 

plugin_LTLIBRARIES = folder.la 
folder_la_SOURCES = plugin-folder.cpp 
folder_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = plugin-folder.h
