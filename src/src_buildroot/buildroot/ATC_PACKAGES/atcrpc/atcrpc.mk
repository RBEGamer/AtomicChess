################################################################################
#
# atcrpc; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCRPC_VERSION = origin/master
ATCRPC_SITE = git@github.com:RBEGamer/rpclib.git
ATCRPC_SITE_METHOD = git
ATCRPC_LICENSE = MIT
ATCRPC_INSTALL_TARGET = YES

#
define ATCRPC_BUILD_CMDS
	@echo ATCRPC_BUILD!
#	ls $(@D)
#	@echo $(@D)
	#$(MAKE) --version
#	
	@echo -----------------
	cmake $(@D)
	#$(@D)/build
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
	#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/gpio
	#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/examples
endef


define ATCRPC_INSTALL_TARGET_CMDS
	@echo ATCRPC_INSTALL!
	#$(INSTALL) -D -m 0755 $(@D) $(TARGET_DIR)/usr
	#$(INSTALL) -D -m 0755 $(@D)/gpio $(TARGET_DIR)/usr
	#$(INSTALL) -D -m 0755 $(@D)/gpio $(TARGET_DIR)/examples
endef

$(eval $(generic-package))



#$(eval $(cmake-package))