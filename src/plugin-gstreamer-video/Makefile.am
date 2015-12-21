plugindir = $(prefix)/lib/gloobus/

AM_CPPFLAGS += $(GTK_CFLAGS) $(GSTREAMER_CFLAGS) $(GSTREAMER_VIDEO_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS)  $(GSTREAMER_LIBS) $(GSTREAMER_VIDEO_LIBS)

plugin_LTLIBRARIES = video.la 
video_la_SOURCES = plugin-gstreamer-video.cpp 
video_la_LDFLAGS = -module -export-dynamic $(AM_LDFLAGS)

noinst_HEADERS= plugin-gstreamer-video.h
