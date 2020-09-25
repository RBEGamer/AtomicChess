################################################################################
#
# atcwiringpi; For ATC Project; Marcel Ochsendorf; marcelochsendorf.com
#
################################################################################

ATCWIRINGPI_VERSION = origin/master
ATCWIRINGPI_SITE = git@github.com:RBEGamer/WiringPi.git
ATCWIRINGPI_SITE_METHOD = git
ATCWIRINGPI_LICENSE = GPL-2.0+


define ATCWIRINGPI_BUILD_CMDS
	@echo ATCWIRINGPI_BUILD!
	ls $(@D)
	@echo $(@D)
	@echo -----------------
	#$(@D)/build
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/wiringPi
	#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/gpio
	#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/examples
endef


define ATCWIRINGPI_INSTALL_TARGET_CMDS
	@echo ATCWIRINGPI_INSTALL!
	$(INSTALL) -D -m 0755 $(@D)/wiringPi $(TARGET_DIR)/usr
	#$(INSTALL) -D -m 0755 $(@D)/gpio $(TARGET_DIR)/usr
	#$(INSTALL) -D -m 0755 $(@D)/gpio $(TARGET_DIR)/examples
endef

$(eval $(generic-package))
