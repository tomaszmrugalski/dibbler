include $(TOPDIR)/rules.mk
PKG_NAME:=dibbler
PKG_VERSION:=0.5.0
PKG_RELEASE:=1

PKG_MD5SUM:=26a0e37512be7641a2deb0dc5b00e582
PKG_SOURCE_URL:=http://klub.com.pl/dhcpv6/dibbler
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-src.tar.gz

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DIBBLER_SERVER,dibbler-server,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,DIBBLER_CLIENT,dibbler-client,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,DIBBLER_RELAY,dibbler-relay,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	touch $@
$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		POSLIB_HOST="--host mipsel-linux"
	touch $@

$(IPKG_DIBBLER_SERVER):
	mkdir -p $(IDIR_DIBBLER_SERVER)/usr/sbin
	mkdir -p $(IDIR_DIBBLER_SERVER)/usr/lib
	mkdir -p $(IDIR_DIBBLER_SERVER)/etc/dibbler
	mkdir -p $(IDIR_DIBBLER_SERVER)/var/lib/dibbler
	cp $(PKG_BUILD_DIR)/dibbler-server $(IDIR_DIBBLER_SERVER)/usr/sbin/
	cp $(TOPDIR)/staging_dir_mipsel/lib/libstdc++.so.6.0.3 $(IDIR_DIBBLER_SERVER)/usr/lib/
	cp $(PKG_BUILD_DIR)/server*.conf $(IDIR_DIBBLER_SERVER)/etc/dibbler
	$(STRIP) $(IDIR_DIBBLER_SERVER)/usr/sbin/*
	$(STRIP) $(IDIR_DIBBLER_SERVER)/usr/lib/*
	$(IPKG_BUILD) $(IDIR_DIBBLER_SERVER) $(PACKAGE_DIR)

$(IPKG_DIBBLER_CLIENT):
	mkdir -p $(IDIR_DIBBLER_CLIENT)/usr/sbin
	mkdir -p $(IDIR_DIBBLER_CLIENT)/usr/lib
	mkdir -p $(IDIR_DIBBLER_CLIENT)/etc/dibbler
	mkdir -p $(IDIR_DIBBLER_CLIENT)/var/lib/dibbler
	cp $(PKG_BUILD_DIR)/dibbler-client $(IDIR_DIBBLER_CLIENT)/usr/sbin/
	cp $(TOPDIR)/staging_dir_mipsel/lib/libstdc++.so.6.0.3 $(IDIR_DIBBLER_CLIENT)/usr/lib/
	cp $(PKG_BUILD_DIR)/client*.conf $(IDIR_DIBBLER_CLIENT)/etc/dibbler
	$(STRIP) $(IDIR_DIBBLER_CLIENT)/usr/sbin/*
	$(STRIP) $(IDIR_DIBBLER_CLIENT)/usr/lib/*
	$(IPKG_BUILD) $(IDIR_DIBBLER_CLIENT) $(PACKAGE_DIR)

$(IPKG_DIBBLER_RELAY):
	mkdir -p $(IDIR_DIBBLER_RELAY)/usr/sbin
	mkdir -p $(IDIR_DIBBLER_RELAY)/usr/lib
	mkdir -p $(IDIR_DIBBLER_RELAY)/etc/dibbler
	mkdir -p $(IDIR_DIBBLER_RELAY)/var/lib/dibbler
	cp $(PKG_BUILD_DIR)/dibbler-relay $(IDIR_DIBBLER_RELAY)/usr/sbin/
	cp $(TOPDIR)/staging_dir_mipsel/lib/libstdc++.so.6.0.3 $(IDIR_DIBBLER_RELAY)/usr/lib/
	cp $(PKG_BUILD_DIR)/relay*.conf $(IDIR_DIBBLER_RELAY)/etc/dibbler
	$(STRIP) $(IDIR_DIBBLER_RELAY)/usr/sbin/*
	$(STRIP) $(IDIR_DIBBLER_RELAY)/usr/lib/*
	$(IPKG_BUILD) $(IDIR_DIBBLER_RELAY) $(PACKAGE_DIR)

