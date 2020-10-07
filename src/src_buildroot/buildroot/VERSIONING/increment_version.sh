#!/usr/bin/python
# ATC Project Marcel Ochsendorf marcelochsendorf.com
# This script increments a versionnumber before each build
# its normally called by the ../build.sh script
print("INCREMENT VERSION SCRIPT")


def increment_ver(version):
    version = version.split('.')
    version[2] = str(int(version[2]) + 1)
    return '.'.join(version)

