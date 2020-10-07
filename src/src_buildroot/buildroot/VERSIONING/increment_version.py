#!/usr/bin/python
import os.path
from shutil import copyfile

VERSION_FILE_NAME = "./VERSION"
# IF NO VERSION FILE EXISTS CREATE ONE
if not os.path.isfile(VERSION_FILE_NAME):
	f = open(VERSION_FILE_NAME, "w")
	f.write("0.0.1")
	f.close()
	exit(0)
	
	
# INCREMENT VERSION
#https://stackoverflow.com/questions/52952905/python-increment-version-number-by-0-0-1
def increment_ver(version):
    version = version.split('.')
    ovl_patch = False
    ovl_minor = False
    if int(version[2]) >= 20:
    	version[2] = str('0') #PATCH INCREMENT
    	ovl_patch = True
    else:
    	version[2] = str(int(version[2]) + 1) #PATCH INCREMENT
    
    
    
    if ovl_patch:
    	if int(version[1]) >= 20:
    		version[1] = str('0') #MINOR INCREMENT
    		ovl_minor = True
    	else:
    		version[1] = str(int(version[1]) + 1) #MINOR INCREMENT
    
    if ovl_minor:
            version[0] = str(int(version[0]) + 1)
    #print(version)
    print(version)
    return '.'.join(version)
    
    
    
    
with open(VERSION_FILE_NAME) as f:
	content = f.readlines()
	if not content:
		content = []
		content[0] = "0.0.1"
	print("OLD_VERSION: " + content[0])
	new_version = increment_ver(content[0])
	print("NEW_VERSION: " + new_version)
	myfile = open(VERSION_FILE_NAME,"w")
	myfile.write(str(new_version))
	myfile.close()