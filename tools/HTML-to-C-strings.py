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
import re

class State(Enum):
  IN_HTML                   = 1
  IN_HTML_COMMENT           = 2
  IN_JS                     = 3
  IN_JS_SINGLE_LINE_COMMENT = 4
  IN_JS_BLOCK_COMMENT       = 5
  IN_JS_STRING_LITERAL1     = 6  # '
  IN_JS_STRING_LITERAL2     = 7  # "
  IN_CSS                    = 8
  IN_CSS_BLOCK_COMMENT      = 9
  IN_CSS_STRING_LITERAL1    = 10 # '
  IN_CSS_STRING_LITERAL2    = 11 # "

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
  state = State.IN_HTML # state variable to track what sort of tokens we'll be parsing
  with open(filepath) as infile:
    for line in infile:
      result, state = parseLine(line, state)
      #print("RESULT: ", state, result)
      if result.strip() != "":
        output = output + result.strip() + "\n"
    print(output)

RESERVED_WORDS = [] # A list of reserved words from HTML and JS here (See: https://www.w3schools.in/javascript-tutorial/keywords/ )

# The state transition table, in the form: [<Current State>][<string found>][<next state>]
BASE_TRANSITION_TABLE = { State.IN_HTML: {"<!--" : State.IN_HTML_COMMENT,
                                    "<script>" : State.IN_JS,
                                    "<style>" : State.IN_CSS},
                     State.IN_HTML_COMMENT: {"-->" : State.IN_HTML},
                     State.IN_JS: {"</script>" : State.IN_HTML,
                                   "/*" : State.IN_JS_BLOCK_COMMENT,
                                   "'" : State.IN_JS_STRING_LITERAL1,
                                   "\"" : State.IN_JS_STRING_LITERAL2,
                                   "//" : State.IN_JS_SINGLE_LINE_COMMENT},
                     State.IN_JS_SINGLE_LINE_COMMENT: {"\n" : State.IN_JS},
                     State.IN_JS_BLOCK_COMMENT: {"*/" : State.IN_JS},
                     State.IN_JS_STRING_LITERAL1: {"[^\]'": State.IN_JS},
                     State.IN_JS_STRING_LITERAL2: {"[^\}\"": State.IN_JS},
                     State.IN_CSS: {"</style>" : State.IN_HTML,
                                    "/*" : State.IN_CSS_BLOCK_COMMENT,
                                    "'" : State.IN_CSS_STRING_LITERAL1,
                                    "\"" : State.IN_CSS_STRING_LITERAL2},
                     State.IN_CSS_BLOCK_COMMENT: {"*/" : State.IN_CSS},
                     State.IN_CSS_STRING_LITERAL1: {"[^\]'" : State.IN_CSS},
                     State.IN_CSS_STRING_LITERAL2: {"[^\]\"" : State.IN_CSS}
                    }
STYLE_TRANSITION_TABLE = { State.IN_HTML: { "<style>" : State.IN_CSS},
                     State.IN_HTML_COMMENT: {},
                     State.IN_JS: {},
                     State.IN_JS_BLOCK_COMMENT: {},
                     State.IN_JS_SINGLE_LINE_COMMENT: {"\n" : State.IN_JS},
                     State.IN_JS_STRING_LITERAL1: {},
                     State.IN_JS_STRING_LITERAL2: {},
                     State.IN_CSS: {"</style>" : State.IN_HTML},
                     State.IN_CSS_BLOCK_COMMENT: {},
                     State.IN_CSS_STRING_LITERAL1: {},
                     State.IN_CSS_STRING_LITERAL2: {}
                    }
EXTENDED_TRANSITION_TABLE = { State.IN_HTML: {"<!--" : State.IN_HTML_COMMENT,
                                    "<script>" : State.IN_JS,
                                    "<style>" : State.IN_CSS},
                     State.IN_HTML_COMMENT: {"-->" : State.IN_HTML},
                     State.IN_JS: {"</script>" : State.IN_HTML},
                     State.IN_JS_SINGLE_LINE_COMMENT: {"\n" : State.IN_JS},
                     State.IN_JS_BLOCK_COMMENT: {},
                     State.IN_JS_STRING_LITERAL1: {},
                     State.IN_JS_STRING_LITERAL2: {},
                     State.IN_CSS: {"</style>" : State.IN_HTML},
                     State.IN_CSS_BLOCK_COMMENT: {},
                     State.IN_CSS_STRING_LITERAL1: {},
                     State.IN_CSS_STRING_LITERAL2: {}
                    }
NOSTRINGS_TRANSITION_TABLE = { State.IN_HTML: {"<!--" : State.IN_HTML_COMMENT,
                                    "<script>" : State.IN_JS,
                                    "<style>" : State.IN_CSS},
                     State.IN_HTML_COMMENT: {"-->" : State.IN_HTML},
                     State.IN_JS: {"</script>" : State.IN_HTML,
                                   "/\*" : State.IN_JS_BLOCK_COMMENT,
                                   "//" : State.IN_JS_SINGLE_LINE_COMMENT},
                     State.IN_JS_SINGLE_LINE_COMMENT: {"\n" : State.IN_JS},
                     State.IN_JS_BLOCK_COMMENT: {"\*/" : State.IN_JS},
                     State.IN_JS_STRING_LITERAL1: {},
                     State.IN_JS_STRING_LITERAL2: {},
                     State.IN_CSS: {"</style>" : State.IN_HTML,
                                    "/\*" : State.IN_CSS_BLOCK_COMMENT},
                     State.IN_CSS_BLOCK_COMMENT: {"\*/" : State.IN_CSS},
                     State.IN_CSS_STRING_LITERAL1: {},
                     State.IN_CSS_STRING_LITERAL2: {}
                    }
BASE_TRANSITION_TABLE = { State.IN_HTML: {"<!--" : State.IN_HTML_COMMENT,
                                    "<script>" : State.IN_JS,
                                    "<style>" : State.IN_CSS},
                     State.IN_HTML_COMMENT: {"-->" : State.IN_HTML},
                     State.IN_JS: {"</script>" : State.IN_HTML,
                                   "/\*" : State.IN_JS_BLOCK_COMMENT,
                                   "'" : State.IN_JS_STRING_LITERAL1,
                                   "\"" : State.IN_JS_STRING_LITERAL2,
                                   "//" : State.IN_JS_SINGLE_LINE_COMMENT},
                     State.IN_JS_SINGLE_LINE_COMMENT: {"\n" : State.IN_JS},
                     State.IN_JS_BLOCK_COMMENT: {"\*/" : State.IN_JS},
                     State.IN_JS_STRING_LITERAL1: {"[^\\\\]'": State.IN_JS},
                     State.IN_JS_STRING_LITERAL2: {"[^\\\\}\"": State.IN_JS},
                     State.IN_CSS: {"</style>" : State.IN_HTML,
                                    "/\*" : State.IN_CSS_BLOCK_COMMENT,
                                    "'" : State.IN_CSS_STRING_LITERAL1,
                                    "\"" : State.IN_CSS_STRING_LITERAL2},
                     State.IN_CSS_BLOCK_COMMENT: {"\*/" : State.IN_CSS},
                     State.IN_CSS_STRING_LITERAL1: {"[^\\\\]'" : State.IN_CSS},
                     State.IN_CSS_STRING_LITERAL2: {"[^\\\\]\"" : State.IN_CSS}
                    }
TRANSITION_TABLE = BASE_TRANSITION_TABLE
COMMENT_STATES = [State.IN_HTML_COMMENT, State.IN_JS_SINGLE_LINE_COMMENT, State.IN_JS_BLOCK_COMMENT, State.IN_CSS_BLOCK_COMMENT]

COMMENT_STRINGS = ["<!--", "-->", "/\*", "\*/", "//"]

# Parses a string and a state variable, processes the string and returns an updated state and resultant string
def parseLine(line, inState):
  output = ""
  outState = inState

  print("PARSING AS "+str(inState)+": ", line)

  if line != "" and len(TRANSITION_TABLE[inState].keys()) > 0:
    # Store the indicies of the first of each of the associated potential transition strings in the line:
    #occuranceIndicies = [line.find(k) for k in TRANSITION_TABLE[inState].keys()]
    occuranceIndicies = [re.search(k, line) for k in TRANSITION_TABLE[inState].keys()] # Use regex instead of find
    occuranceIndicies = list(map(lambda n: -1 if n == None else n.span()[0], occuranceIndicies))# Convert Nones to -1s so that the next bit works
    # Now zip(? don't. Just keep them both as ordered lists) the occurance indicies with the states that they produce,
    # Argmin to find the first (if existing) ocuring match, store the state that implies will be next
    closestIndex, value = argmin(occuranceIndicies, len(line))
    print("DATA:", "'"+line.strip()+"'\n", "PROCESSING:", list(TRANSITION_TABLE[inState].keys()), occuranceIndicies, closestIndex, value, "END DATA")
    nextState = inState # For when the state doesn't change
    if closestIndex == -1:
      # None of the strings were found, so set the value to the end of the line
      value = len(line)
    else:
      nextState = TRANSITION_TABLE[inState][list(TRANSITION_TABLE[inState].keys())[closestIndex]]
      #print("State transition found!", nextState)
    # Process (depending on state [write/don't write, eliminate spaces etc]) the stuff up until and including (depending on if the previous state or next state is a COMMENT_STATE) the first match
    tokenString = list(TRANSITION_TABLE[inState].keys())[closestIndex]
    leftHalf = line[:value + (len(tokenString) if tokenString not in COMMENT_STRINGS else 0)]# The string up to and including the transition string (if it's not a comment)
    rightHalf = line[value + len(tokenString):]# The remaining half of the line (excluding the transition string)
    
    if(inState in COMMENT_STATES):
      # If we're in a comment state, just move on
      output, outState = parseLine(rightHalf, nextState)
    else:
      # If we're not, then perform state-specific masking of the output
      output = stateSpecificMasking(leftHalf, inState)
      parsedRight, outState = parseLine(rightHalf, nextState)
      output = output + parsedRight

  # If the line is empty, return input data as it's reached it's terminal recursion
  return output, outState

# Takes a tokenless string and alters it based on the contents
def stateSpecificMasking(line, state):
  if state == State.IN_CSS_STRING_LITERAL1 or state == State.IN_CSS_STRING_LITERAL2 :
    line = line.replace(" ", "@")
  return line # For now just pass back the entire string. Maybe Do things like variable contraction or operator space removal in the future

# Returns the (index, and then the value of the lowest number in a list):
#argmin = lambda ls, maxInt: reduce(lambda indexWithTotal, n: n if n[1]<indexWithTotal[1] and n[0] != -1 else indexWithTotal, enumerate(ls), (-1,maxInt))
# (index in search list, value)
#argmin = lambda ls, maxInt: reduce(lambda indexWithTotal, n: n if n[1]<indexWithTotal[1] else indexWithTotal, enumerate(ls), (0,maxInt))
def argmin(ls, mx):
  best = mx
  bestI = -1
  for i, l in enumerate(ls):
    if l < best and l>=0:
      best = l
      bestI = i
  return bestI, best

#  if inState == State.IN_HTML:
#    # Check for start of HTML comment, start of JS, start of CSS
#    # Then parse the leftovers
#  elif inState == State.IN_HTML_COMMENT:
#    # Check for end of HTML comment
#    # Parse leftovers
#  elif inState == State.IN_JS:
#    # Remove eol comments
#    line = line.split("//")[0];
#    # Check for end of JS
#    # Parse leftovers
#  elif inState == State.IN_JS_BLOCK_COMMENT:
#    # Check for end of JS block comment
#    # Parse leftovers
#  elif inState == State.IN_JS_STRING_LITERAL:
#    # Check for end of JS string literal
#    # Parse leftovers
#  elif inState == State.IN_CSS:
#    # Check for end of CSS, start of CSS block comment, start of CSS string literal
#    # Parse leftovers
#  elif inState == State.IN_CSS_BLOCK_COMMENT:
#    # Check for end of CSS block comment
#    # Parse leftovers
#  elif inState == State.IN_CSS_STRING_LITERAL:
#    # Check for end of CSS string literal
#    # Parse leftovers
#  else:
#    # Bleh. Not sure what happens here.
#    # Find the first ins

#def searchForFirst(words, <LIST OF STRINGS AND THEIR RELATIVE STATE CHANGES (perhaps this should be a dict?)>)


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
#def isSurrounded(start, position?, end):
#  return Eh


if __name__ == "__main__":
  main(sys.argv[1:])
