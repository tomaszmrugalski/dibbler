#!/usr/bin/python

#import easy to use xml parser called minidom:
from xml.dom.minidom import parseString
import os
import sys


#<testsuites tests="1" failures="0" disabled="0" errors="0" time="0.059" name="AllTests">
#  <testsuite name="ServerTest" tests="1" failures="0" disabled="0" errors="0" time="0.053">
#    <testcase name="constructor" status="run" time="0.052" classname="ServerTest" />
#  </testsuite>
#</testsuites>

#all these imports are standard on most modern python implementations

if (len(sys.argv) > 1):
    dir = sys.argv[1]
else:
    dir = "."

#print("Looking for XMLs in %s" % dir)

def parseFile(xmlFile):
    #open the xml file for reading:
    file = open(xmlFile,'r')
    #convert to string:
    data = file.read()
    #close file because we dont need it anymore:
    file.close()
    #parse the xml you got from the file
    dom = parseString(data)

    #print (data)

    #retrieve the first xml tag (<tag>data</tag>) that the parser finds with name tagName:
    testsuites = dom.getElementsByTagName('testsuites')[0]

    total = int(testsuites.getAttribute("tests"))
    failed = int(testsuites.getAttribute("failures"))
    disabled = int(testsuites.getAttribute("disabled"))
    errors = int(testsuites.getAttribute("errors"))
    execTime = float(testsuites.getAttribute("time"))

    passed = total - failed - disabled - errors

    return (passed, failed, disabled, errors, total, execTime)


passedCnt = 0
failedCnt = 0
disabledCnt = 0
errorsCnt = 0
totalCnt = 0
execTimeCnt = 0.0

dirList = os.listdir(dir)
for fname in dirList:
    #print(fname)
    if (fname.count(".") != 1):
        continue
    name,ext = fname.split('.')
    if (ext != "xml"):
        continue
    #print("Parsing file %s" % fname)

    (passed, failed, disabled, errors, total, execTime) = parseFile(dir + "/" + fname)
    passedCnt += passed
    failedCnt += failed
    disabledCnt += disabled
    errorsCnt += errors
    totalCnt += total
    execTimeCnt = execTime

print("%d:%d:%d:%d:%d:%f"
      % (passedCnt, failedCnt, disabledCnt, errorsCnt, totalCnt, execTimeCnt))
