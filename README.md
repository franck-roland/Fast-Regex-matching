Fast-Regex-matching
===================
This program is a fast pattern matching with a reduce vocabulary options.
This can be use as a python module or can be included in any C programs you want.
It is really faster than grep or awk and amazingly faster than python re.


This implementation is doing alignment on the fly instead of compiling a complete finite state machine.

Usage
===================
Syntax:	
	. equivalent to .* in common regex language
	.{min,max} for a variable field which size is between min and max. min <=max
	.{c} for a variable field of size c
	(smthg) for grouping expression

