################################################################################
#
# ATCCHESSENGINERYAN597; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCCHESSENGINERYAN597_VERSION = 1.0.0
ATCCHESSENGINERYAN597_SITE = ./package/atcchessengineryan597/src
ATCCHESSENGINERYAN597_SITE_METHOD = local
ATCCHESSENGINERYAN597_LICENSE = GPL-2.0+
ATCCHESSENGINERYAN597_INSTALL_TARGET = YES

define ATCCHESSENGINERYAN597_BUILD_CMDS
	@echo ATCCHESSENGINERYAN597_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) CXXFLAGS='-std=c++17'

endef


define ATCCHESSENGINERYAN597_INSTALL_TARGET_CMDS
	@echo ATCCHESSENGINERYAN597_INSTALL!
	ls $(@D)
	@echo -- make targetdir/usr/ATC/engines with -p --
	mkdir -p $(TARGET_DIR)/usr/ATC/engines
	@echo -- copy executable --
	cp $(@D)/src/NGin $(TARGET_DIR)/usr/ATC/engines/atc_chess_engine_ryan597
	cp $(@D)/src/*.a $(TARGET_DIR)/usr/ATC/engines/
endef

$(eval $(cmake-package))




