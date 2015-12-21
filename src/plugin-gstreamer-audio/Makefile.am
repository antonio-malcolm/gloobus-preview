plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) $(GSTREAMER_CFLAGS) #$(TAGLIB_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS)  $(GSTREAMER_LIBS) #$(TAGLIB_LIBS)

plugin_LTLIBRARIES = audio.la 
audio_la_SOURCES = plugin-gstreamer-audio.cpp 
audio_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS = plugin-gstreamer-audio.h
