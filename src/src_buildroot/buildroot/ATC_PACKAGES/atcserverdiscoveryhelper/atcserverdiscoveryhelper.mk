################################################################################
#
# aatcserverdiscoveryhelpertcctl; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCSERVERDISCOVERYHELPER_VERSION = 1.0.0
ATCSERVERDISCOVERYHELPER_SITE = ./package/atcserverdiscoveryhelper/src
ATCSERVERDISCOVERYHELPER_SITE_METHOD = local
ATCSERVERDISCOVERYHELPER_LICENSE = GPL-2.0+
ATCSERVERDISCOVERYHELPER_INSTALL_TARGET = YES

define ATCSERVERDISCOVERYHELPER_BUILD_CMDS
	@echo ATCSERVERDISCOVERYHELPER_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) CONFIG=DEBUGLOCAL TARGETNAME=atc_controller CXXFLAGS='-std=c++17'

endef


define ATCSERVERDISCOVERYHELPER_INSTALL_TARGET_CMDS
	@echo ATCSERVERDISCOVERYHELPER_INSTALL!
	ls $(@D)
	@echo -- make targetdir/usr/ATC with -p --
	mkdir -p $(TARGET_DIR)/usr/ATC/
	@echo -- copy atc_discovery_helper executable --
	cp $(@D)/atc_discovery_helper $(TARGET_DIR)/usr/ATC/atc_discovery_helper
	$(INSTALL) -D -m 0755 $(@D)/atc_discovery_helper $(TARGET_DIR)/usr/ATC/atc_discovery_helper
	cp $(@D)/*.a $(TARGET_DIR)/usr/ATC/
endef

$(eval $(cmake-package))




