# This is a basic script that takes a folder of HTML files and produces a C header file containing the HTML file.
# It's a bit hacky, but it makes sense for the first version of RoboPad.

## TODO
# 1. Removal of comments (but HTML and JS)
# 2. Variable and function name compression (JS only; shorten variables down to a growing sequential list, eg: a,b,c...,ba,bb.. - avoiding keywords from both JS and HTML)
# 3. LZ77 compression on each file (requires decompression algorithm in C) - this should probably be zlib for ease of use (or, heck we can implement LZ77? But I don't think that'll be as optimal)
# 3a-i. Scratch the below, we can probably just store it compressed and send it to the client as is, specifying compression.
# 3a. Perhaps all-file compression, with unrolling on request?
# 3b. Perhaps separate CSS (and JS?) out to it's own variable with per-request hosting - maybe just smash all CSS together? ...probably just make it load a CSS file and make a special variable for it

# JS comment parsing notes:
#  A multi-line comment can end within a single-line, but cannot start within one
#  As soon as a multi-line comment end is found, the block is ended

import os
import sys
import getopt
from functools import reduce
from enum import Enum

class State(Enum):
  IN_HTML               = 1
  IN_HTML_COMMENT       = 2
  IN_JS                 = 3
  IN_JS_BLOCK_COMMENT   = 4
  IN_JS_STRING_LITERAL  = 5
  IN_CSS                = 6
  IN_CSS_BLOCK_COMMENT  = 7
  IN_CSS_STRING_LITERAL = 8

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

  if not htmlFolder.endswith("/"):
    htmlFolder = htmlFolder + "/"
  
  print("Writing to file '%s'..."%outputFile)
  with open(outputFile, "w") as outfile:

    print("Looking for files in %s..."%htmlFolder)
    for filename in os.listdir(htmlFolder):
      if filename.endswith(".html") or filename.endswith(".css"):
        print("Composing file '%s'..."%filename)
        condensedHTML = removeVerbosity(htmlFolder + filename)
        # Then some code to actually save the file
      else:
        print("Ignoring file '%s'..."%filename)

# Ingests the file and gets rid of unwanted characters
def removeVerbosity(filepath):
  output = ""
  State state = State.IN_HTML # state variable to track what sort of tokens we'll be parsing
  for line in htmlFile:
    result, state = parseString(line, state)
    output = output + result

# Parses a string and a state variable, processes the string and returns an updated state and resultant string
def parseString(code, inState):
  output = ""
  outputState = inState
  if inState == State.IN_HTML:
    # Check for start of HTML comment, start of JS, start of CSS
    # Then parse the leftovers
  elif inState == State.IN_HTML_COMMENT:
    # Check for end of HTML comment
    # Parse leftovers
  elif inState == State.IN_JS:
    # Remove eol comments
    line = line.split("//")[0];
    # Check for end of JS
    # Parse leftovers
  elif inState == State.IN_JS_BLOCK_COMMENT:
    # Check for end of JS block comment
    # Parse leftovers
  elif inState == State.IN_JS_STRING_LITERAL:
    # Check for end of JS string literal
    # Parse leftovers
  elif inState == State.IN_CSS:
    # Check for end of CSS, start of CSS block comment, start of CSS string literal
    # Parse leftovers
  elif inState == State.IN_CSS_BLOCK_COMMENT:
    # Check for end of CSS block comment
    # Parse leftovers
  elif inState == State.IN_CSS_STRING_LITERAL:
    # Check for end of CSS string literal
    # Parse leftovers
  else:
    # Bleh. Not sure what happens here.
    # Find the first ins



#    # Get rid of comments
#    singleLineCommentSplit = line.split("//")
#    line = singleLineCommentSplit[0] # Remove any single-line Javascript comments
#
#    # New design: Go through every character, track entrances and exits of string literals and multi-line comments (Deal with opening multiline comments after a single-line comment starts?)
#    for i in len(line):
#      if line[i] == " " and line[i+1] 
#
#    # Finally, look at the potentially removed single-line comments and see if they end any block started comments:
#    inBlockComment = inBlockComment and not ( len(singleLineCommentSplit) > 1 and reduce((lambda: x, y: x or y), ["*/" in section for section in singleLineCommentSplit[1:]]) )
#    # Note the above is equivilant to:
#    #  if inBlockComment and len(singleLineCommentSplit) > 1 and reduce((lambda: x, y: x or y), ["*/" in section for section in singleLineCommentSplit[1:]]):
#    #    inBlockComment = False
#
#
#
#    # Get rid of whitespace eitherside of operators (excluding anything within quote marks)
#    # Get rid of leading and trailling whitespace and newlines
#    #line = line.strip()
#    print(line, end='')

# Returns whether the position is between these two things within the code. This should probably be tracked line at a time...
def isSurrounded(start, position?, end):
  return Eh

RESERVED_WORDS = [] # A list of reserved words from HTML and JS here (See: https://www.w3schools.in/javascript-tutorial/keywords/ )

if __name__ == "__main__":
  main(sys.argv[1:])
