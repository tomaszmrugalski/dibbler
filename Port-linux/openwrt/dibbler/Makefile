include $(TOPDIR)/rules.mk
PKG_NAME:=dibbler
PKG_VERSION:=0.5.0
PKG_RELEASE:=1
PKG_MD5SUM:=26a0e37512be7641a2deb0dc5b00e582
PKG_SOURCE_URL:=http://klub.com.pl/dhcpv6/dibbler/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-src.tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DIBBLER,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

#$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
#	#Since there is no configure script, we can directly go to the building step
#	touch $@

$(PKG_BUILD_DIR)/.configured:
	$(SCRIPT_DIR)/gen_busybox_config.pl $(TOPDIR)/.config > $(PKG_BUILD_DIR)/.config
	$(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CROSS)g++ CROSS="$(TARGET_CROSS)" HOST="mipsel-linux-uclibc" -C $(PKG_BUILD_DIR)
#	yes '' | $(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CROSS)g++ CROSS="$(TARGET_CROSS)" HOST="mipsel-linux-uclibc" -C $(PKG_BUILD_DIR) 
	touch $@

#$(PKG_BUILD_DIR)/.built:
#	mkdir -p $(PKG_INSTALL_DIR)/usr/bin
#Note here that we pass cross-compiler as default compiler to use
#	$(MAKE) -C $(PKG_BUILD_DIR) \
#		CC=$(TARGET_CC) \
#		$(TARGET_CONFIGURE_OPTS) \
#	#	cfg_filename="/etc/dibbler/server.conf"
#		prefix="$(PKG_INSTALL_DIR)/usr"
#	$(CP) $(PKG_BUILD_DIR)/dibbler-server $(PKG_INSTALL_DIR)/usr/bin
#	touch $@

$(PKG_BUILD_DIR)/.built:
	 mkdir -p $(PKG_INSTALL_DIR)/usr/bin
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(IDIR_DIBBLER)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR) server
	$(CP) $(PKG_BUILD_DIR)/dibbler-server $(PKG_INSTALL_DIR)/usr/bin
	touch $@

#$(IPKG_DIBBLER):
#	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(IDIR_DIBBLER)" \
#		EXTRA_CFLAGS="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR) install
#	$(STRIP) $(IDIR_DIBBLER)/bin/dibbler
#	$(IPKG_BUILD) $(IDIR_DIBBLER) $(PACKAGE_DIR)

$(IPKG_DIBBLER):
	install -d -m0755 $(IDIR_DIBBLER)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/dibbler $(IDIR_DIBBLER)/usr/bin
	$(RSTRIP) $(IDIR_DIBBLER)
	$(IPKG_BUILD) $(IDIR_DIBBLER) $(PACKAGE_DIR)
mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built

