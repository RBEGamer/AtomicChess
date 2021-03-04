################################################################################
#
# atcctl; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCCTL_VERSION = origin/controller
ATCCTL_SITE = git@github.com:RBEGamer/AtomicChessOS.git
ATCCTL_SITE_METHOD = git
ATCCTL_LICENSE = GPL-2.0+


define ATCCTL_BUILD_CMDS
	@echo ATCCTL_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/src/src_controller/controller/controller CONFIG=DEBUGLOCAL TARGETNAME=atc_controller CXXFLAGS='-std=c++17'

endef


define ATCCTL_INSTALL_TARGET_CMDS
	@echo ATCCTL_INSTALL!
	ls $(@D)/src/src_controller/controller/controller
	@echo -- make targetdir/usr/ATC with -p --
	mkdir -p $(TARGET_DIR)/usr/ATC/
	@echo -- copy atc_controller_executable --
	cp $(@D)/src/src_controller/controller/controller/Debug/atc_controller $(TARGET_DIR)/usr/ATC/atc_controller
	@echo -- copy needed files (WEBSERVER STATIC ..) --
	cp -R $(@D)/src/src_controller/controller/controller/WEBSERVER_STATIC $(TARGET_DIR)/usr/ATC/
	#$(INSTALL) -D -m 0755 $(@D)/src/src_controller/controller $(TARGET_DIR)/usr/ATC/atc_controller
endef

$(eval $(generic-package))
