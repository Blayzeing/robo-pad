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
import gzip

DEBUG = False
SEPARATOR = "//===========================================================" # Separator string for output header files
HEX_CHARS_PER_LINE = 20 # The number of hex characters to put on each line of the compressed HTML header file

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
  IN_CSS_BRACE_BLOCK        = 12

# Converts a camel-case filename (eg /some/place/fileName.html) to a constant-style capitalized name (eg 'FILE_NAME') for guard and variable construction.
def filepathToVariable(filepath):
  output = ".".join(filepath.split("/")[-1].split(".")[:-1]).replace(".", "_")
  for find in re.findall("[a-z][A-Z]", output):
    output = output.replace(find, str(find[0] + "_" + find[1]))
  output = output.upper()
  return output

def main(argv):
  USAGE_STRING = """USAGE:\n\nHTML-to-C-strings.y -i <HTML folder path (defaults to '../HTML/')> [-o <output header file name (defaults to 'htmlStrings.h')> | -d <path to debug output folder> | -v | -c]\n\n
                      -i  The input folder that stores HTML files
                      -o  The output header file name
                      -d  Path to a folder to store debug output
                      -v  Enable verbose DEBUG output, shows tokens as they are parsed
                      -c  Enable compressed HTML string saving (will save to the output header file name + "-compressed.h")
                 """
  htmlFolder = "../HTML/"
  outputFile = "htmlStrings.h"
  compressedFile = None
  debug = None
  
  try:
    opts, args = getopt.getopt(argv, "hi:o:d:vc")
  except getopt.GetoptError:
    print (USAGE_STRING)
    sys.exit()

  for opt, arg in opts:
    if opt == "-h":
      print(USAGE_STRING)
      sys.exit()
    elif opt == "-i":
      htmlFolder = arg
    elif opt == "-o":
      outputFile = arg
    elif opt == "-d":
      debug = arg
    elif opt == "-v":
      global DEBUG
      DEBUG = True

  if not htmlFolder.endswith("/"):
    htmlFolder = htmlFolder + "/"

  if not outputFile.endswith(".h"):
    outputFile = outputFile + ".h"

  for opt, arg in opts:
      compressedFile = ".".join(outputFile.split(".")[:-1]) + "-compressed.h"

  if debug != None and not debug.endswith("/"):
    debug = debug + "/"
  
  print("Writing to file '%s'..."%outputFile)
  with open(outputFile, "w") as outfile:
    guard = filepathToVariable(outputFile)
    outfile.write("#ifndef "+guard+"_H\n")
    outfile.write("#define "+guard+"_H\n")

    if(compressedFile):
      compressedFile = open(compressedFile, "w")
      compressedFile.write("#ifndef "+guard+"_COMPRESSED_H\n")
      compressedFile.write("#define "+guard+"_COMPRESSED_H\n")

    print("Looking for files in %s..."%htmlFolder)
    for filename in os.listdir(htmlFolder):
      if filename.endswith(".html") or filename.endswith(".css"):
        print("\nComposing file '%s'..."%filename)
        print(" Condensing...")
        condensedHTML = removeVerbosity(htmlFolder + filename)
        #print(condensedHTML)
        # Create the gzip'd version:
        print(" Compressing...")
        compressedHTML = gzip.compress(condensedHTML.encode('utf-8'))
        # Debug outputs:
        if debug != None:
          condensedName = ".".join(filename.split('.')[:-1])+"-condensed."+filename.split('.')[-1]
          encodedFile = condensedHTML.encode('utf-8')
          # Condensed html file
          with open(debug + condensedName, "wb") as chtml:
            chtml.write(encodedFile)
            print(" [DEBUG] Written condensed html to " + condensedName)
          compressedName = ".".join(filename.split('.')[:-1])+".gzstr"
          # Compressed data file
          with open(debug + compressedName, "wb") as chtml:
            chtml.write(compressedHTML)
            print(" [DEBUG] Written compressed html to " + compressedName)
          # Debug compression info readout
          with open(htmlFolder + filename, 'r') as originalFile:
            originalFileBytes = (originalFile.read()).encode('utf-8')
            olen = len(originalFileBytes)
            compression = 100-len(compressedHTML)/olen * 100
            condension = 100-len(condensedHTML)/olen * 100
            print(" [DEBUG] Compression levels: {0:.2f}% condensed, {1:.2f}% condensed+compressed".format(condension, compression))
            print("         Final compressed filesize is {0:.2f}% original size.".format(100-compression))

        print(" Adding condensed HTML to c file...")
        lines = condensedHTML.split("\n")
        lines = list(map(lambda n: n.replace("\\", "\\\\").replace("\"", "\\\"") + "\\n", lines))
        variableName = filepathToVariable(filename) + '_' + filename.split('.')[-1].upper()
        outfile.write(SEPARATOR+"\nconst char " + variableName + "[] = \"" + lines[0] + "\\\n")
        for line in lines[1:-1]:
          outfile.write(line + "\\\n")
        outfile.write(lines[-1] + "\";\n")

        if(compressedFile):
          print(" Adding compressed HTML to c file...")
          byteArray = "{" + ", ".join([("\n" if index%HEX_CHARS_PER_LINE == 0 else "") + str(hex(byte)) for (index, byte) in enumerate(compressedHTML)]) + "\n};"
          compressedFile.write("\nconst uint8_t " + variableName + "[] PROGMEM = " + byteArray +"\n")
          compressedFile.write("const size_t %s_SIZE = %i;\n"%(variableName, len(compressedHTML)))
        
      else:
        print("Ignoring file '%s'..."%filename)
    outfile.write("#endif")
    if(compressedFile):
      compressedFile.write("#endif")
      compressedFile.close()

# Ingests the file and gets rid of unwanted characters
def removeVerbosity(filepath):
  output = ""
  state = {"html": State.IN_HTML, "css": State.IN_CSS}[filepath.split('.')[-1]] # state variable to track what sort of tokens we'll be parsing (starts either in HTML or CSS depending on the filename
  with open(filepath) as infile:
    for line in infile:
      result, state = parseLine(line, state)
      #print("RESULT: ", state, result)
      if result.strip() != "":
        output = output + result.strip() + "\n"
  return output

RESERVED_WORDS = [] # A list of reserved words from HTML and JS here (See: https://www.w3schools.in/javascript-tutorial/keywords/ )

# The state transition table, in the form: [<Current State>][<string found>][<next state>]
TRANSITION_TABLE = { State.IN_HTML: {"<!--" : State.IN_HTML_COMMENT,
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
                     State.IN_JS_STRING_LITERAL2: {"[^\\\\]\"": State.IN_JS},
                     State.IN_CSS: {"</style>" : State.IN_HTML,
                                    "/\*" : State.IN_CSS_BLOCK_COMMENT,
                                    "'" : State.IN_CSS_STRING_LITERAL1,
                                    "\"" : State.IN_CSS_STRING_LITERAL2,
                                    "{": State.IN_CSS_BRACE_BLOCK},
                     State.IN_CSS_BLOCK_COMMENT: {"\*/" : State.IN_CSS},
                     State.IN_CSS_STRING_LITERAL1: {"[^\\\\]'" : State.IN_CSS},
                     State.IN_CSS_STRING_LITERAL2: {"[^\\\\]\"" : State.IN_CSS},
                     State.IN_CSS_BRACE_BLOCK: {"}" : State.IN_CSS}
                    }
COMMENT_STATES = [State.IN_HTML_COMMENT, State.IN_JS_SINGLE_LINE_COMMENT, State.IN_JS_BLOCK_COMMENT, State.IN_CSS_BLOCK_COMMENT]
COMMENT_STRINGS = ["<!--", "-->", "/*", "*/", "//"]

REGEX_TO_LITERAL = {"/\*": "/*", "\*/": "*/", "[^\\\\]'": " '", "[^\\\\]\"": " \""}
def convertRegexToLiteral(regex):
    try:
        return REGEX_TO_LITERAL[regex]
    except:
        return regex

# Parses a string and a state variable, processes the string and returns an updated state and resultant string
def parseLine(line, inState):
  output = ""
  outState = inState

  if DEBUG:
    print("PARSING AS "+str(inState)+": ", line)

  if line != "" and len(TRANSITION_TABLE[inState].keys()) > 0:
    # Store the indicies of the first of each of the associated potential transition strings in the line:
    #occuranceIndicies = [line.find(k) for k in TRANSITION_TABLE[inState].keys()]
    occuranceIndicies = [re.search(k, line) for k in TRANSITION_TABLE[inState].keys()] # Use regex instead of find
    occuranceIndicies = list(map(lambda n: -1 if n == None else n.span()[0], occuranceIndicies))# Convert Nones to -1s so that the next bit works
    # Argmin to find the first (if existing) ocuring match, store the state that implies will be next
    closestIndex, value = argmin(occuranceIndicies, len(line))
    if DEBUG:
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
    tokenString = convertRegexToLiteral(tokenString)
    if DEBUG:
        print("FOUND TOKEN: " + tokenString)
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

# Takes a tokenless string and alters it based on the contents and the state
def stateSpecificMasking(line, state):
  # Remove whitespace around operators in CSS and JS:
  if state == State.IN_CSS or state == State.IN_JS:
    operators = ['=',':','-','+','/','*','(',')','[','[','{','}', ';', ',']
    for op in operators:
      line = re.sub("\s*"+re.escape(op)+"\s*", op, line)
  # Can't remember if any of these have special meaning in CSS so only replacing them in JS:
  if state == State.IN_JS:
    operators = ['==', '||', '&&', '!']
    for op in operators:
      line = re.sub("\s*"+re.escape(op)+"\s*", op, line)
  # Remove newlines in CSS tag bodies # Eh, nah, that'd take doing all that other stuff.
  #if state == State.IN_CSS_BRACE_BLOCK:
  #  line = line.rstrip()
  # Could get rid of all semicolons at EOL in JS (and just rely on the newline character)?
  # Could store JS variable names and replace them out with shorter ones as and when they're referred to (watch the variables referenced in HTML though :/)
  return line

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

if __name__ == "__main__":
  main(sys.argv[1:])
