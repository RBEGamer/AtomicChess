################################################################################
#
# atcsrv; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCCTL_VERSION = 1.0.0
ATCCTL_SITE = ./package/atcsrv/src
ATCCTL_SITE_METHOD = local
ATCCTL_LICENSE = GPL-2.0+
ATCCTL_INSTALL_TARGET = YES

define ATCSRV_BUILD_CMDS
	@echo ATCSRV_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) CONFIG=DEBUGLOCAL TARGETNAME=atc_server CXXFLAGS='-std=c++17'

endef


define ATCSRV_INSTALL_TARGET_CMDS
	@echo ATCSRV_INSTALL!
	ls $(@D)
	@echo -- make targetdir/usr/ATC with -p --
	mkdir -p $(TARGET_DIR)/usr/ATC/
	@echo -- copy atc_srv_executable --
	cp $(@D)/atc_server $(TARGET_DIR)/usr/ATC/atc_server
	$(INSTALL) -D -m 0755 $(@D)/atc_server $(TARGET_DIR)/usr/ATC/atc_server
endef

$(eval $(cmake-package))




