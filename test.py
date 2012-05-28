#!/usr/bin/python
import _libRegex
REGEX = ["(aok.{4,5}r","(aok.{45sd}","(aok.{452937497237493}","","(aok.{6,5})","idy","..zef.",".","(a.{0,5})o(.)ok"]
PERSO = "aokokok"
#print PERSO
print "--------------------"
print

def test(option):
    for r in REGEX:
        print "______________________"
        print "regex: "+r
        try:
            l = _libRegex.match(r, PERSO, option)
            print "SUCCESS"
            print l
            print "#fields "+str(len(l))
        except Exception, e:
            print ": ".join(e)

test(0)
#test(1)
