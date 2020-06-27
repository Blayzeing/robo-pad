# This is a basic script that takes a folder of HTML files and produces a C header file containing the HTML file.
# It's a bit hacky, but it makes sense for the first version of RoboPad.

## TODO
# 1. Removal of comments (but HTML and JS)
# 2. Variable and function name compression (JS only; shorten variables down to a growing sequential list, eg: a,b,c...,ba,bb.. - avoiding keywords from both JS and HTML)
# 3. LZ77 compression on each file (requires decompression algorithm in C)
# 3a. Perhaps all-file compression, with unrolling on request?
# 3b. Perhaps separate CSS (and JS?) out to it's own variable with per-request hosting - maybe just smash all CSS together? ...probably just make it load a CSS file and make a special variable for it

import os
import sys
import getopt

def main(argv):
  
  USAGE_STRING = "USAGE:\n\nHTML-to-C-strings.y -i <HTML folder path (defaults to '../HTML/'> [-o <output header file name (defaults to 'htmlStrings.h')>]\n"
  htmlFolder = "../HTML/"
  outputFile = "htmlStrings.h"
  
  try:
    opts, args = getopt.getopt(argv, "hi:o:")
  except getopt.GetoptError:
    print (USAGE_STRING)

  for opt, arg in opts:
    if opt == "-h":
      print(USAGE_STRING)
      sys.exit()
    elif opt == "-i":
      htmlFolder = arg
    elif opt == "-o":
      outputFile = arg
  
  print("Looking for files in %s..."%htmlFolder)

  

  for filename in os.listdir(htmlFolder):
    print("Composing file '%s'..."%filename)

RESERVED_WORDS = [] # A list of reserved words from HTML and JS here (See: https://www.w3schools.in/javascript-tutorial/keywords/ )

if __name__ == "__main__":
  main(sys.argv[1:])
