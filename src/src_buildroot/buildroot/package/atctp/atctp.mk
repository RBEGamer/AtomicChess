################################################################################
#
# atctp
#
################################################################################

ATCTP_VERSION = c86db334fa4e47d066d6accad1a4e060baf8e5d7
ATCTP_SITE = git@github.com:RBEGamer/AtomicChessOS.git
ATCTP_SITE_METHOD = git
ATCTP_LICENSE = GPL-2.0+


define ATCTP_BUILD_CMDS
	@echo ATCTP_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/src/src_atctestpackage
endef


define ATCTP_INSTALL_TARGET_CMDS
	@echo ATCTP_INSTALL!
	$(INSTALL) -D -m 0755 $(@D)/src/src_atctestpackage/hello $(TARGET_DIR)/usr/atc_test_package
endef

$(eval $(generic-package))
