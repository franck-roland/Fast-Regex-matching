#!/usr/bin/python
# -*- coding: latin-1 -*- 
import _libRegex
import sys


REGEX = ["(Lorem.{4,5}.","(Lorem.{45})()sdc","(Lorem.{452937497237493})","","(Lorem.{6,5})","idy","..nulla.",".","(Lorem.{0,5})m(.)id.","Lorem.eu.","(Lorem.eu.)","Lorem.eu.(Donec.)(purus ).","*zeoz*ef*zm:fz:.Sdcls:ùd:cs$d+(ç-_àè(	ç_)()="]
PERSO = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque tristique nulla non justo ultrices tincidunt. Pellentesque sollicitudin erat id velit vestibulum adipiscing. Vestibulum eget turpis nulla. Vestibulum congue varius risus, vitae ultricies ante euismod et. Morbi venenatis nisi eu arcu ullamcorper suscipit. Morbi sit amet mi vel diam mollis semper in eu neque. Donec vitae purus non libero ultrices malesuada."
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
            l = _libRegex.match(r, PERSO, option)
            print "SUCCESS"
            print l
            print l.split('\n')
            print
            print "#fields "+str(len(l.split('\n')))
        except Exception, e:
            print ": ".join(e)

if(len(sys.argv) == 2):
	if(int(sys.argv[1])==0 or int(sys.argv[1])==1):
		test(int(sys.argv[1]))
#test(1)
