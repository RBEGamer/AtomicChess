#!/usr/bin/env python

"""
fix-table-color-bleed.py: take a file as argument or input from stdin,
remove @{} from table headers and print the result on stdout 
"""

import sys
from re import sub

def fix_table_color_bleed(infile):

   """
   Remove @{} from latex table entries and print on stdout
   """

   fixedfile = sub("(\\\\begin\{longtable\}\[\]\{)\@\{\}(.*?)\@\{\}",
        "\\1\\2", infile)
   print(fixedfile)

if __name__ == "__main__":
  if len(sys.argv) > 1:
     infile = open(sys.argv[1])
     filecontens = infile.read()
  else:
     filecontens = sys.stdin.read()

fix_table_color_bleed(filecontens)
