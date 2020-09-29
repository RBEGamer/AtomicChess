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

$(eval $(cmake-package))