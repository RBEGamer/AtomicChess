################################################################################
#
# atcctl; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCCTL_VERSION = 1.0.0
ATCCTL_SITE = ./package/atcctl/src
ATCCTL_SITE_METHOD = local
ATCCTL_LICENSE = GPL-2.0+
ATCCTL_INSTALL_TARGET = YES

define ATCCTL_BUILD_CMDS
	@echo ATCCTL_BUILD!
	@echo $(@D)
	@echo -----------------
	#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) CONFIG=DEBUGLOCAL TARGETNAME=atc_controller CXXFLAGS='-std=c++17'

endef


define ATCCTL_INSTALL_TARGET_CMDS
	@echo ATCCTL_INSTALL!
	ls $(@D)
	@echo -- make targetdir/usr/ATC with -p --
	mkdir -p $(TARGET_DIR)/usr/ATC/
	@echo -- copy atc_controller_executable --
	cp $(@D)/atc_controller $(TARGET_DIR)/usr/ATC/atc_controller
	$(INSTALL) -D -m 0755 $(@D)/atc_controller $(TARGET_DIR)/usr/ATC/atc_controller
endef

$(eval $(cmake-package))
#$(eval $(generic-package))
