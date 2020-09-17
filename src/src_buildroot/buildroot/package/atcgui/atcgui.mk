################################################################################
#
# atcgui; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCGUI_VERSION = origin/qtui #821e0eb7b77ed6553769d24525a6cda82d6d38c4
ATCGUI_SITE = git@github.com:RBEGamer/AtomicChessOS.git
ATCGUI_SITE_METHOD = git
ATCGUI_LICENSE = GPL-2.0+

# NOW ADD QT DEPENDENCIES
ATCGUI_DEPENDENCIES = qt5quickcontrols2 qt5base qt5quickcontrols qt5declarative

define ATCGUI_BUILD_CMDS
	@echo ATCGUI_BUILD!
	@echo -----------------
	@echo $(@D)
	@echo -----------------
	@echo ATCGUI_BUILD_QMAKE!
	@echo -----------------
	$(QT5_QMAKE) -d -o $(@D)/src/src_qtui/rpidisplayqt/Makefile $(@D)/src/src_qtui/rpidisplayqt/rpidisplayqt.pro 
	@echo -----------------
	@echo ATCGUI_BUILD_MAKE!
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/src/src_qtui/rpidisplayqt/
	@echo -----------------
endef


define ATCGUI_INSTALL_TARGET_CMDS
	@echo ATCGUI_INSTALL!
	$(INSTALL) -D -m 0755 $(@D)/src/src_qtui/rpidisplayqt/rpidisplayqt $(TARGET_DIR)/usr/ATC/atc_ui
endef

$(eval $(generic-package))
