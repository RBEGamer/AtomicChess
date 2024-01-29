################################################################################
#
# ATCCHESSENGINESTOCKDORY; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################


ATCCHESSENGINESTOCKDORY_VERSION = 1.0.0
ATCCHESSENGINESTOCKDORY_SITE = ./package/atcchessenginestockdory/StockDory/
ATCCHESSENGINESTOCKDORY_SITE_METHOD = local
ATCCHESSENGINESTOCKDORY_LICENSE = GPL-2.0+
ATCCHESSENGINESTOCKDORY_INSTALL_TARGET = YES

define ATCCHESSENGINESTOCKDORY_BUILD_CMDS
	@echo ATCCHESSENGINESTOCKDORY_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all

endef


define ATCCHESSENGINESTOCKDORY_INSTALL_TARGET_CMDS
	@echo ATCCHESSENGINESTOCKDORY_INSTALL!
	ls $(@D)
	@echo -- make targetdir/usr/ATC/engines with -p --
	mkdir -p $(TARGET_DIR)/usr/ATC/engines
	@echo -- copy executable --
	cp $(@D)/chessengine $(TARGET_DIR)/usr/ATC/engines/atc_chess_engine_stockdory
	cp $(@D)/src/*.a $(TARGET_DIR)/usr/ATC/engines/
	cp $(@D)/src/*-static $(TARGET_DIR)/usr/ATC/engines/
endef

$(eval $(cmake-package))




