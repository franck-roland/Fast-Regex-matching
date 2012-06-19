#!/usr/bin/python
# -*- coding: latin-1 -*- 
import _libRegex
import sys


REGEX = ["(Lorem.{4,5}.","(Lorem.{45})()sdc","(Lorem.{452937497237493})","","(Lorem.{6,5})","idy","..nulla.",".","(Lorem.{0,5})m(.)id.","Lorem.eu.","(Lorem.eu.)","Lorem.eu.(Donec.)(purus ).","*zeoz*ef*zm:fz:.Sdcls:ùd:cs$d+(ç-_àè(    ç_)()="]
PERSO = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque tristique nulla non justo ultrices tincidunt. Pellentesque sollicitudin erat id velit vestibulum adipiscing. Vestibulum eget turpis nulla. Vestibulum congue varius risus, vitae ultricies ante euismod et. Morbi venenatis nisi eu arcu ullamcorper suscipit. Morbi sit amet mi vel diam mollis semper in eu neque. Donec vitae purus non libero ultrices malesuada."
REGEX = ["(.{,18})(20)(.{,108})(20)(.{,86})(20)(.{,34})(20)(.{,122})(4c)(.{,24})(20)(.{,184})(20)(.{,32})(69)(.{,76})(74)(.{,578})(6d)(.{,60})(6c)(.{,52})(74)(.{,196})(69)(.{,44})(74)(.{,10})(6c)(.{,48})(69)(.{,22})(74)(.{,18})(6f)(.{,136})(74)(.{,6})(6e)(.{,20})(20)(.{,18})(66)(.{,80})(72)(.{,80})(74)(.{,30})(69)(.{,30})(74)(.{,12})(65)(.{,72})(0a)(.{,106})(6f)(.{,38})(65)(.{,54})(74)(.{,22})(6f)(.{,22})(20)(.{,22})(6c)(.{,104})(69)(.{,90})(65)(.{,3620})(0a)(.{,396})"]
PERSO = "3c21444f43545950452068746d6c205055424c494320222d2f2f5733432f2f4454442048544d4c20332e322046696e616c2f2f454e223e3c68746d6c3e0a3c7469746c653e4469726563746f7279206c697374696e6720666f72202f6e65747a6f622d656e646f646572636f6e69632f3c2f7469746c653e0a3c626f64793e0a3c68323e4469726563746f7279206c697374696e6720666f72202f6e65747a6f622d656e646f646572636f6e69632f3c2f68323e0a3c68723e0a3c756c3e0a3c6c693e3c6120687265663d222e6769742f223e2e6769742f3c2f613e0a3c6c693e3c6120687265663d222e67697469676e6f7265223e2e67697469676e6f72653c2f613e0a3c6c693e3c6120687265663d22415554484f52532e747874223e415554484f52532e7478743c2f613e0a3c6c693e3c6120687265663d226275696c642f223e6275696c642f3c2f613e0a3c6c693e3c6120687265663d22434f5059494e472e747874223e434f5059494e472e7478743c2f613e0a3c6c693e3c6120687265663d22646f632f223e646f632f3c2f613e0a3c6c693e3c6120687265663d226c69622f223e6c69622f3c2f613e0a3c6c693e3c6120687265663d224d414e49464553542e696e223e4d414e49464553542e696e3c2f613e0a3c6c693e3c6120687265663d226e65747a6f62223e6e65747a6f623c2f613e0a3c6c693e3c6120687265663d224e4557532e747874223e4e4557532e7478743c2f613e0a3c6c693e3c6120687265663d22504b472d494e464f2e747874223e504b472d494e464f2e7478743c2f613e0a3c6c693e3c6120687265663d22524541444d452e747874223e524541444d452e7478743c2f613e0a3c6c693e3c6120687265663d227265736f75726365732f223e7265736f75726365732f3c2f613e0a3c6c693e3c6120687265663d2273657475702e636667223e73657475702e6366673c2f613e0a3c6c693e3c6120687265663d2273657475702e7079223e73657475702e70793c2f613e0a3c6c693e3c6120687265663d227372632f223e7372632f3c2f613e0a3c6c693e3c6120687265663d22746573742f223e746573742f3c2f613e0a3c2f756c3e0a3c68723e0a3c2f626f64793e0a3c2f68746d6c3e0a"
#print PERSO
print "--------------------"
print

def test(option):
    print "------------------------"
    print "option: "+str(option)
    print "------------------------"
    print
    for r in REGEX:
        print "______________________"
        print 
        print "regex: "+r
        try:
            print PERSO
            l = _libRegex.match(r, PERSO, option)
            print "SUCCESS"
            print l.split('\n')
            print
            print "#fields "+str(len(l.split('\n')))
        except Exception, e:
            print ": ".join(e)

if(len(sys.argv) == 2):
    if(int(sys.argv[1])==0 or int(sys.argv[1])==1):
        test(int(sys.argv[1]))
else:
    print "Usage: ./test.py [0 or 1]"
    print
