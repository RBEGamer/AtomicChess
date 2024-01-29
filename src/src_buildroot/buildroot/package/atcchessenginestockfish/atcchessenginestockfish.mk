################################################################################
#
# ATCCHESSENGINESTOCKFISH; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCCHESSENGINESTOCKFISH_VERSION = 1.0.0
ATCCHESSENGINESTOCKFISH_SITE = ./package/atcchessenginestockfish/src
ATCCHESSENGINESTOCKFISH_SITE_METHOD = local
ATCCHESSENGINESTOCKFISH_LICENSE = GPL-2.0+
ATCCHESSENGINESTOCKFISH_INSTALL_TARGET = YES

define ATCCHESSENGINESTOCKFISH_BUILD_CMDS
	@echo ATCCHESSENGINESTOCKFISH_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) build $(TARGET_CONFIGURE_OPTS) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) CXXFLAGS='-std=c++17'

endef


define ATCCHESSENGINESTOCKFISH_INSTALL_TARGET_CMDS
	@echo ATCCHESSENGINESTOCKFISH_INSTALL!
	ls $(@D)
	@echo -- make targetdir/usr/ATC/engines with -p --
	mkdir -p $(TARGET_DIR)/usr/ATC/engines
	@echo -- copy executable --
	cp $(@D)/stockfish $(TARGET_DIR)/usr/ATC/engines/atc_chess_engine_stockfish16
	$(INSTALL) -D -m 0755 $(@D)/stockfish $(TARGET_DIR)/usr/ATC/engines/atc_chess_engine_stockfish16
endef

$(eval $(generic-package))




