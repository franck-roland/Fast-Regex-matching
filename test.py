#!/usr/bin/python
import _libRegex
reg = ["(aok.{4,5}r","(aok.{45sd}","","(aok.{6,5})","idy","..zef.","."]
PERSO = "aok."
#print PERSO
print "--------------------"
print
for r in reg:
    print "______________________"
    print "regex: "+r
    try:
        l = _libRegex.match(r, PERSO, 0)
        if len(r) < 100:
            print r
	print "SUCCESS"
        print l
	print len(l)
    except Exception, e:
        print ": ".join(e)
