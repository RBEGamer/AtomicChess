################################################################################
#
# atest
#
################################################################################

ATCGUI_VERSION = 821e0eb7b77ed6553769d24525a6cda82d6d38c4
ATCGUI_SITE = git@github.com:RBEGamer/AtomicChessOS.git
ATCGUI_SITE_METHOD = git
ATCGUI_LICENSE = GPL-2.0+
#ATCGUI_LICENSE_FILES = COPYING
# ATEST_DEPENDENCIES = host-pkgconf libev alsa-lib
# Fetched from Github, with no configure script
define ATCGUI_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
endef


define ATCGUI_INSTALL_TARGET_CMDS
	@echo Done!
	$(INSTALL) -D -m 0755 $(@D)/hello $(TARGET_DIR)/usr/atc_test_package
endef

$(eval $(generic-package))
