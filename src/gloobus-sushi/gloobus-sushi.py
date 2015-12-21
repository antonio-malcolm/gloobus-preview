# Copyright (C) 2009-2010 Gloobus Developers
#
#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#as published by the Free Software Foundation; either version 2
#of the License, or (at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

import dbus
import dbus.service
from gi.repository import GObject
import os

from dbus.mainloop.glib import DBusGMainLoop

DBUS_NAME = 'org.gnome.NautilusPreviewer'
DBUS_PATH = '/org/gnome/NautilusPreviewer'

class DBusWrapper(dbus.service.Object):
    def __init__(self):
        bus_name = dbus.service.BusName(DBUS_NAME, bus=dbus.SessionBus())
        super(DBusWrapper, self).__init__(bus_name, DBUS_PATH)
        self.loop = GObject.MainLoop()
        self.loop.run()

    @dbus.service.method(DBUS_NAME)
    def Activate(self):
        print('gloobus-sushi: activate')

    @dbus.service.method(DBUS_NAME, in_signature='sib')
    def ShowFile(self, uri, xid, closeIfAlreadyShown):
        print('gloobus-sushi:', uri)
        os.system('gloobus-preview "%s" &' % uri)

    @dbus.service.method(DBUS_NAME)
    def Close(self):
        print('gloobus-sushi: quit')
        self.loop.quit()


if __name__ == '__main__':
    DBusGMainLoop(set_as_default=True)
    wrapper = DBusWrapper()
