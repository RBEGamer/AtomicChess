################################################################################
#
# atctp; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCTP_VERSION = 1.0.0
ATCTP_SITE = ./package/atcdgt3000/src
ATCTP_SITE_METHOD = local
ATCTP_LICENSE = GPL-2.0+
ATCTP_INSTALL_TARGET = YES

#define ATCTP_BUILD_CMDS
#	@echo ATCTP_BUILD!
#	@echo $(@D)
#	@echo -----------------
#	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
#endef
#
#
define ATCDGT3000_INSTALL_TARGET_CMDS
	@echo ATCDGT3000_INSTALL!
	$(INSTALL) -D -m 0755 $(@D)/atcdgt3000 $(TARGET_DIR)/usr/ATC/atc_dgt3000
endef

#$(eval $(generic-package))

$(eval $(cmake-package))
