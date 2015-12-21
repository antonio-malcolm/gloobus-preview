SUBDIRS = \
    gloobus-sushi \
    plugin-comic \
    plugin-folder \
    plugin-compressed \
    plugin-djvu \
    plugin-imagemagick \
    plugin-gstreamer-audio \
    plugin-gstreamer-video \
    plugin-pdf \
    plugin-pixbuf \
    plugin-ps \
    plugin-text \
    plugin-ttf \
    plugin-xps

#CXXFLAGS = -Werror -g    #If -O2  iDocument threads don't work
AM_CXXFLAGS = -Wall #-g -O0
AM_LDFLAGS = -rdynamic

AM_CPPFLAGS += \
	$(CAIRO_CFLAGS) \
	$(GTK_CFLAGS) \
	$(PTHREAD_CFLAGS) \
	$(GTKSOURCEVIEW_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(X11_CFLAGS) \
	$(BOOST_CPPFLAGS)

AM_LDADD = \
	$(PTHREAD_LIBS) \
	$(CAIRO_LIBS) \
	$(GTK_LIBS) \
	$(GTKSOURCEVIEW_LIBS) \
	$(GLIB_LIBS) \
	$(X11_LIBS) \
	-ldl

bin_PROGRAMS = gloobus-preview
dist_bin_SCRIPTS = gloobus-preview-configuration

gloobus_preview_SOURCES = \
    document-page.cpp \
    document-plugin.cpp \
    gloobus-preview-main.cpp \
    gloobus-preview-window.cpp \
    gloobus-preview-config.cpp \
    gloobus-preview-plugin-manager.cpp \
    gloobus-preview-interface-main.cpp \
    gloobus-preview-interface-default.cpp \
    gloobus-preview-interface-image.cpp \
    gloobus-preview-interface-text.cpp \
    gloobus-preview-interface-media.cpp \
    gloobus-preview-interface-document.cpp \
    gloobus-preview-interface-list.cpp \
    gloobus-preview-interface-free.cpp \
    gloobus-preview-theme.cpp \
    gloobus-preview-toolbar.cpp \
    gloobus-preview-singleton.cpp \
    task-manager.cpp

gloobus_preview_LDADD = $(AM_LDADD)
gloobus_preview_LDFLAGS = $(AM_LDFLAGS)

noinst_HEADERS = \
	document-loader.h \
    document-page.h \
    document-plugin.h \
    gloobus-preview-window.h \
    gloobus-preview-config.h \
    gloobus-preview-plugin-manager.h \
    gloobus-preview-interface-main.h \
    gloobus-preview-interface-image.h \
    gloobus-preview-interface-text.h \
    gloobus-preview-interface-default.h \
    gloobus-preview-interface-media.h \
    gloobus-preview-interface-document.h \
    gloobus-preview-interface-list.h \
    gloobus-preview-interface-free.h \
    gloobus-preview-theme.h \
    gloobus-preview-toolbar.h \
    gloobus-preview-singleton.h \
    gloobus-preview-defines.h\
    task.h \
    task-manager.h \
    utils.h
