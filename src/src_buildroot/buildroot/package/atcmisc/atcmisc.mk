################################################################################
#
# atcmisc; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCMISC_VERSION = origin/master
ATCMISC_SITE = git@github.com:RBEGamer/WiringPi.git
ATCMISC_SITE_METHOD = git
ATCMISC_LICENSE = GPL-2.0+


define ATCMISC_BUILD_CMDS
	@echo ATCMISC_BUILD!
	ls $(@D)
	@echo $(@D)
	@echo -----------------
	#$(@D)/build
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/wiringPi
	#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/gpio
	#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/examples
endef


define ATCMISC_INSTALL_TARGET_CMDS
	@echo ATCMISC_INSTALL!
	$(INSTALL) -D -m 0755 $(@D)/wiringPi $(TARGET_DIR)/usr
	#$(INSTALL) -D -m 0755 $(@D)/gpio $(TARGET_DIR)/usr
	#$(INSTALL) -D -m 0755 $(@D)/gpio $(TARGET_DIR)/examples
endef

$(eval $(generic-package))
