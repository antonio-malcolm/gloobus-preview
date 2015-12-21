# *** init ******************************************************************* #
AC_INIT([gloobus-preview], [0.5], [http://launchpad.net/gloobus])
AC_CONFIG_SRCDIR([src/gloobus-preview-main.cpp])
AM_INIT_AUTOMAKE([1.11])

AC_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])

# *** i18n ******************************************************************* #
GETTEXT_PACKAGE=gloobus-preview
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Package name for gettext])
AC_SUBST(GETTEXT_PACKAGE)

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])

# *** checks for programs **************************************************** #
AC_PROG_CXX             # Check g++
m4_ifdef([AM_PROG_AR], [AM_PROG_AR]) # required by automake 1.12
AC_PROG_LIBTOOL         # To create libraries
AC_PROG_INSTALL
AC_PROG_MAKE_SET        # Compile more than one Makefiles
AM_PATH_PYTHON([3.0])

# *** checks for boost ******************************************************* #
AX_BOOST_BASE([1.39.0])
#AX_BOOST_PROGRAM_OPTIONS
#AX_BOOST_SIGNALS

# *** checks for libraries *************************************************** #
AC_CHECK_LIB(dl, dlopen)            # Checks for libraries.
AC_CHECK_LIB(gthread, g_thread_init)
AC_CHECK_LIB(pthread, pthread_create)

# *** checks for headers ***************************************************** #
AC_CHECK_HEADERS([stdlib.h string.h pthread.h])

# *** checks for modules ***************************************************** #
PKG_CHECK_MODULES(GTK, gtk+-3.0 >= 3.0)
PKG_CHECK_MODULES(GTKSOURCEVIEW, gtksourceview-3.0)
PKG_CHECK_MODULES(CAIRO, cairo)
PKG_CHECK_MODULES(GSTREAMER, gstreamer-1.0)
PKG_CHECK_MODULES(GSTREAMER_PLUGINS, gstreamer-plugins-base-1.0)
PKG_CHECK_MODULES(GSTREAMER_VIDEO, gstreamer-video-1.0)
PKG_CHECK_MODULES(POPPLER, poppler-glib)
PKG_CHECK_MODULES(SPECTRE, libspectre >= 0.2.2)
PKG_CHECK_MODULES(DJVU, ddjvuapi)
#PKG_CHECK_MODULES(TAGLIB, taglib)
PKG_CHECK_MODULES(GXPS, libgxps)
PKG_CHECK_MODULES(FREETYPE,freetype2)
PKG_CHECK_MODULES(GLIB, glib-2.0)
PKG_CHECK_MODULES(X11, x11)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

ACX_PTHREAD()

# Checks for library functions.
AC_CHECK_FUNCS([strpbrk])

REVNO=`bzr revno`
AC_DEFINE_UNQUOTED(REVNO, "$REVNO", [Bazaar revision number])
AC_SUBST(REVNO)

AC_SUBST([AM_CPPFLAGS], ["-D'PACKAGE_DATA_PATH=\"\$(datadir)/gloobus\"' -D'PACKAGE_LIB_PATH=\"\$(libdir)/gloobus\"' -D'LOCALEDIR=\"\$(datadir)/locale\"'"])

AC_CONFIG_FILES([ po/Makefile.in
    Makefile
    src/Makefile
    src/plugin-comic/Makefile
    src/plugin-folder/Makefile
    src/plugin-compressed/Makefile
    src/plugin-djvu/Makefile
    src/plugin-imagemagick/Makefile
    src/plugin-gstreamer-audio/Makefile
    src/plugin-gstreamer-video/Makefile
    src/plugin-pdf/Makefile
    src/plugin-pixbuf/Makefile
    src/plugin-ps/Makefile
    src/plugin-text/Makefile
    src/plugin-ttf/Makefile
    src/plugin-xps/Makefile
    src/gloobus-sushi/Makefile
    data/Makefile
    data/images/Makefile
    data/gtkrc/Makefile
    data/icons/Makefile
    data/icons/hicolor/16x16/Makefile
    data/icons/hicolor/16x16/apps/Makefile
    data/icons/hicolor/22x22/Makefile
    data/icons/hicolor/22x22/apps/Makefile
    data/icons/hicolor/24x24/Makefile
    data/icons/hicolor/24x24/apps/Makefile
    data/icons/hicolor/32x32/Makefile
    data/icons/hicolor/32x32/apps/Makefile
    data/icons/hicolor/48x48/Makefile
    data/icons/hicolor/48x48/apps/Makefile
    data/icons/hicolor/64x64/Makefile
    data/icons/hicolor/64x64/apps/Makefile
    data/icons/hicolor/scalable/Makefile
    data/icons/hicolor/scalable/apps/Makefile
    data/icons/hicolor/Makefile
])

AC_OUTPUT

#echo "\033[1;37m Configuration OK \033[1;m"
#echo -e "\033[1;37mConfiguration OK"
echo
echo
echo        $PACKAGE $VERSION
echo

echo        Prefix............... : $prefix
echo        Rev no............... : `bzr revno`

echo        "Type make to compile"
echo
echo
