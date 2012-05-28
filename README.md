Fast-Regex-matching
===================
This program is a fast pattern matching with a reduce vocabulary options.
This can be use as a python module or can be included in any C programs you want.
It is really faster than grep or awk and amazingly faster than python re.


This implementation is doing alignment on the fly instead of compiling a complete finite state machine.

Usage
===================

### Syntax of regex:	
*	. equivalent to .* in common regex language
*	.{min,max} for a variable field which size is between min and max. min <=max
*	.{c} for a variable field of size c
*	(smthg) for grouping expression

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

	import _libRegex
	reg = ["(aok.{4,5}r","(aok.{45sd}","(aok.{452937497237493}","","(aok.{6,5})","idy","..zef.",".","(a.{0,5})o."]
	PERSO = "aok."
	print "--------------------"
	print

	def test(option):
	    for r in reg:
	        print "______________________"
	        print "regex: "+r
	        try:
	            l = _libRegex.match(r, PERSO, option)
	            print "SUCCESS"
	            print l
	        except Exception, e:
	            print ": ".join(e)

	test(0)
	test(1)

**hardtest.py**
 
