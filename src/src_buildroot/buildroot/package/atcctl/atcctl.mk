################################################################################
#
# atcctl; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCCTL_VERSION = origin/controller
ATCCTL_SITE = git@github.com:RBEGamer/AtomicChessOS.git
ATCCTL_SITE_METHOD = git
ATCCTL_LICENSE = GPL-2.0+


define ATCCTL_BUILD_CMDS
	@echo ATCCTL_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/src/src_controller
endef


define ATCCTL_INSTALL_TARGET_CMDS
	@echo ATCCTL_INSTALL!
	$(INSTALL) -D -m 0755 $(@D)/src/src_controller/controller $(TARGET_DIR)/usr/ATC/atc_controller
endef

$(eval $(generic-package))
