Fast-Regex-matching
===================
This program is a fast pattern matching with a reduce vocabulary options.
This can be use as a python module or can be included in any C programs you want.
It is really faster than grep or awk and amazingly faster than python re.


This implementation is doing alignment on the fly instead of compiling a complete finite state machine.

Usage
===================

### Syntax of regex:	
*	__.__ equivalent to __.*__ in common regex language
*	__.{min,max}__ for a variable field which size is between min and max. min <=max  
	You can also write **.{,max}** (min will be equal to 0)  
	or **.{min,}** for variable field with size at least equal to min
*	__.{c}__ for a variable field of size c.  
	if you want only 1 char that can be variable, write __.{1}__
*	__(exp)__ for grouping expression



### Use in a python script:
*	Import the python module _libRegex
*	_libRegex.match(REGEX,MESSAGE,OPTION)
*  	OPTION can be 0 or 1
	*	If equal to 0: get all group in when align the message
	*	If equal to 1: get only the group in parenthesis and variable fields when aligning
*	Put _libRegex.match(REGEX,MESSAGE,OPTION) in a (try,except) structure. 
	_libRegex.match raise an exception when cannot align message or when the regex contain syntax error

### Example:
**test.py**

	#!/usr/bin/python
	# -*- coding: latin-1 -*- 
	import _libRegex
	import sys


	REGEX = ["(Lorem.{4,5}.","(Lorem.{45})()sdc","(Lorem.{452937497237493})","","(Lorem.{6,5})","idy","..nulla.",".","(Lorem.{0,5})m(.)id.","Lorem.eu.","(Lorem.eu.)","Lorem.eu.(Donec.)(purus).","*zeoz*ef*zm:fz:.Sdcls:ùd:cs$d+(ç-_àè(	ç_)()="]
	PERSO = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque tristique nulla non justo ultrices tincidunt. Pellentesque sollicitudin erat id velit vestibulum adipiscing. Vestibulum eget turpis nulla. Vestibulum congue varius risus, vitae ultricies ante euismod et. Morbi venenatis nisi eu arcu ullamcorper suscipit. Morbi sit amet mi vel diam mollis semper in eu neque. Donec vitae purus non libero ultrices malesuada."

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

**hardtest.py**
 
