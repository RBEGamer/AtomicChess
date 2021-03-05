################################################################################
#
# atctp; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCTP_VERSION = 1.0.0
ATCTP_SITE = ./package/atctp/src
ATCTP_SITE_METHOD = local
ATCTP_LICENSE = GPL-2.0+


define ATCTP_BUILD_CMDS
	@echo ATCTP_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
endef


define ATCTP_INSTALL_TARGET_CMDS
	@echo ATCTP_INSTALL!
	$(INSTALL) -D -m 0755 $(@D)/hello $(TARGET_DIR)/usr/ATC/atc_testpackage
endef

$(eval $(generic-package))
