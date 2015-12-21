plugindir = $(prefix)/lib/gloobus/
officeplugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) $(POPPLER_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS) $(POPPLER_LIBS)

plugin_LTLIBRARIES = pdf.la
pdf_la_SOURCES = pdf-plugin.cpp pdf-loader.cpp
pdf_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

officeplugin_LTLIBRARIES = office.la
office_la_SOURCES = office-loader.cpp office-plugin.cpp pdf-loader.cpp
office_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = \
	office-loader.h \
	office-plugin.h \
	pdf-loader.h \
	pdf-plugin.h
