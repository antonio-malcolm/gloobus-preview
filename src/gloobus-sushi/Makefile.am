do_subst = sed \
	-e s![@]PYTHON[@]![@]PYTHON[@]! 	\
	-e s![@]prefix[@]!$(prefix)!g		\
	-e s![@]sysconfdir[@]!$(sysconfdir)!g		\
	-e s![@]datadir[@]!$(datadir)!g		\
	-e s![@]pkgdatadir[@]!$(pkgdatadir)!g	\
	-e s![@]libexecdir[@]!$(libexecdir)!g	\
	-e s![@]libdir[@]!$(libdir)!g		\
	-e s![@]PACKAGE[@]!$(PACKAGE)!g		\
	-e s![@]VERSION[@]!$(VERSION)!g		\
	-e s![@]REVNO[@]!$(REVNO)!g
	
bin_SCRIPTS = gloobus-sushi

gloobussushidir = $(datadir)/gloobus
gloobussushi_PYTHON = gloobus-sushi.py

EXTRA_DIST = gloobus-sushi.in

gloobus-sushi: gloobus-sushi.in
	$(AM_V_GEN) $(do_subst) $< > $@
	chmod +x $@
