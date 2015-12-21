/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** gloobus-preview
**
** gloobus-preview-main.cpp - 
**
** Copyright (c) 2009-2010 Gloobus Developers
**
** Authors:
**    Jordi Puigdellívol Hernández <guitarboy000@gmail.com>
**    Alexandr Grigorcea <cahr.gr@gmail.com>
**
** This program is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License version 3, as published
** by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranties of
** MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
** PURPOSE.  See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

//Create the POT file
//xgettext -d gloobus-preview --keyword=_ -s -o gloobus-preview.pot gloobus-preview*.cpp

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gloobus-preview-singleton.h"

int main (int argc, char *argv[])
{
	setlocale( LC_ALL, "" );
	bindtextdomain	( "gloobus-preview", LOCALEDIR);
	textdomain		( "gloobus-preview" );

	//g_thread_init(NULL);
	//gdk_threads_init();
	XInitThreads();
	gtk_init (&argc, &argv);
	//gtk_rc_parse(PACKAGE_DATA_PATH "/gtkrc/customgtk.rc");

	return Gloobus::instance()->init( &argc , &argv );
}
