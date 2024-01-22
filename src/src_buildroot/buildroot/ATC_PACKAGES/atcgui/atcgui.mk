################################################################################
#
# atcgui; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCGUI_VERSION = 1.0.0
ATCGUI_SITE = ./package/atcgui/src
ATCGUI_SITE_METHOD = local
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
	cat $(@D)/atcgui.pro
	@echo -----------------
	
	$(QT5_QMAKE) -d -o $(@D)/Makefile $(@D)/atcgui.pro  
	@echo -----------------
	@echo ATCGUI_BUILD_MAKE!
	@echo -----------------
	ls $(@D)
	$(MAKE) $(TARGET_CONFIGURE_OPTS) CXXFLAGS="-std=c++17 -DUSES_QT" -C $(@D)/
	@echo -----------------
endef


define ATCGUI_INSTALL_TARGET_CMDS
	@echo ATCGUI_INSTALL!
	$(INSTALL) -D -m 0755 $(@D)/atcgui $(TARGET_DIR)/usr/ATC/atc_ui
endef

$(eval $(generic-package))
