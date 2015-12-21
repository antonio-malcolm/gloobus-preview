plugindir = $(prefix)/lib/gloobus/
officeplugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) $(GXPS_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS) $(GXPS_LIBS)

plugin_LTLIBRARIES = xps.la
xps_la_SOURCES = xps-plugin.cpp xps-loader.cpp
xps_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = \
	xps-loader.h \
	xps-plugin.h
