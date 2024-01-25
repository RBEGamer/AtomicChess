################################################################################
#
# atcsrv; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCSRV_VERSION = 1.0.0
ATCSRV_SITE = ./package/atcsrv/src
ATCSRV_SITE_METHOD = local
ATCSRV_LICENSE = GPL-2.0+
ATCSRV_INSTALL_TARGET = YES

define ATCSRV_BUILD_CMDS
	@echo ATCSRV_BUILD!
	@echo $(@D)
	@echo -----------------
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) CONFIG=DEBUGLOCAL TARGETNAME=atc_server CXXFLAGS='-std=c++17'

endef


define ATCSRV_INSTALL_TARGET_CMDS
	@echo ATCSRV_INSTALL!
	ls $(@D)
	@echo -- make targetdir/usr/ATC with -p --
	mkdir -p $(TARGET_DIR)/usr/ATC/
	@echo -- copy atc_srv_executable --
	cp $(@D)/atc_server $(TARGET_DIR)/usr/ATC/atc_server
	$(INSTALL) -D -m 0755 $(@D)/atc_server $(TARGET_DIR)/usr/ATC/atc_server

	# COPY BUILD LIBS OVER
	ls $(@D)
	cp $(@D)/*.a $(TARGET_DIR)/usr/ATC/ || true
	cp $(@D)/*.so* $(TARGET_DIR)/usr/lib/ || true

	# COPY STATIC FILES OVER
	cp -R $(@D)/html_static $(TARGET_DIR)/usr/ATC/ || true
endef

$(eval $(cmake-package))




