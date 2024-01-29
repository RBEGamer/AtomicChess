################################################################################
#
# ATCCHESSENGINETHEOLEPAGE; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################


ATCCHESSENGINETHEOLEPAGE_VERSION = 1.0.0
ATCCHESSENGINETHEOLEPAGE_SITE = ./package/atcchessenginetheolepage/src
ATCCHESSENGINETHEOLEPAGE_SITE_METHOD = local
ATCCHESSENGINETHEOLEPAGE_LICENSE = GPL-2.0+
ATCCHESSENGINETHEOLEPAGE_INSTALL_TARGET = YES

define ATCCHESSENGINETHEOLEPAGE_BUILD_CMDS
	@echo ATCCHESSENGINETHEOLEPAGE_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all

endef


define ATCCHESSENGINETHEOLEPAGE_INSTALL_TARGET_CMDS
	@echo ATCCHESSENGINETHEOLEPAGE_INSTALL!
	ls $(@D)
	@echo -- make targetdir/usr/ATC/engines with -p --
	mkdir -p $(TARGET_DIR)/usr/ATC/engines
	@echo -- copy executable --
	cp $(@D)/chessengine $(TARGET_DIR)/usr/ATC/engines/atc_chess_engine_theolepage
	cp $(@D)/src/*.a $(TARGET_DIR)/usr/ATC/engines/
	cp $(@D)/src/*-static $(TARGET_DIR)/usr/ATC/engines/
endef

$(eval $(cmake-package))




