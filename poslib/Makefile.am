SUBDIRS = poslib 

libposlib_configdir = $(includedir)/$(PACKAGE)-$(VERSION)
libposlib_config_DATA = config.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = poslib-@GENVERSION@.pc 

poslib-@GENVERSION@.pc: poslib.pc
	cp $< $@
	
CLEANFILES = poslib-@GENVERSION@.pc 
