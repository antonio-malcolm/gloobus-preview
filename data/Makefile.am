SUBDIRS = icons images gtkrc

desktopdir = $(datadir)/applications
desktop_DATA = gloobus-preview.desktop

servicedir = $(datadir)/dbus-1/services
service_in_files = net.gloobus.Sushi.service.in
service_DATA = $(service_in_files:.service.in=.service)

net.gloobus.Sushi.service: net.gloobus.Sushi.service.in Makefile
	$(AM_V_GEN) sed -e "s|\@bindir\@|$(bindir)|" $< > $@

man_MANS = gloobus-preview.1 gloobus-preview-configuration.1

EXTRA_DIST = \
	$(desktop_DATA) \
	$(service_in_files) \
	$(man_MANS)

CLEANFILES = \
	$(service_DATA)
