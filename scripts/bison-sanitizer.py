#!/usr/bin/python

# Note: this script is not needed. It is kept for sentimental reasons

import re
import sys

def expandMacro(text, macro_name):
    # do the magic here
    members = re.compile(r'#define (YY_ClntParser_MEMBERS) ([^#]*)', re.MULTILINE|re.DOTALL)
    m = members.search(text)
    if m:
        # found MEMBERS!
        print(">>>>%s<<<<" % m.group(2))
    else:
        print("ClntParser_MEMBERS not found\n")
        return text

    members_used = re.compile('^ YY_ClntParser_MEMBERS', re.MULTILINE)
    puthere = members_used.search(text)
    if puthere:
        text = text[0:puthere.start()] + m.group(2) + text[puthere.end():]
        text = text[0:m.start()] + text[m.end():]
    return text

if (len(sys.argv) < 2):
    print("Usage: bison-sanitizer.py input-file.h")
    sys.exit(-1)
else:
    infile = sys.argv[1]

if (len(sys.argv) >=3):
    outfile = sys.argv[2]
else:
    outfile = infile + "-new";

f = open(infile, 'r')
content = f.read()
f.close()

output = expandMacro(content, "YY_ClntParser_MEMBERS")

fout = open(outfile, 'w')
fout.write(output)
fout.close()

