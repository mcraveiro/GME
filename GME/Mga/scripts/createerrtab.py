#!/usr/bin/env python
#
# Copyright (c) 2006-2009 ISIS, Vanderbilt Univeristy 
# Author: Peter Volgyesi (peter.volgyesi@vanderbilt.edu)
#

"""Generates cpp file for supporting IErrorInfo based on the idl file
   Replaces (deprecates) the original bash/sed script"""

import os
import re
import sys
import time
import getpass

rbegin = re.compile('MGAERRORTABLE\s+BEGIN')
rhelpstring = re.compile('\s*\[\s*helpstring\s*\(\s*("[^"]*")\s*\)\s*\]')
rhresult = re.compile('\s*(E_\S+)\s*=\s*0x')
rend = re.compile('MGAERRORTABLE\s+END')
intable = False

if not os.environ.has_key('GME_ROOT'):
	print "Please, set the GME_ROOT environment variable correctly."
	sys.exit(1)

root_path = os.environ['GME_ROOT']
i_path = os.path.join(root_path, 'GME', 'Interfaces', 'Mga.idl');
o_path = os.path.join(root_path, 'GME', 'Mga', 'MgaErr.c');

print 'Generating error info from', i_path, 'to', o_path, "...", 

i = open(i_path, 'r')
o = open(o_path, 'w')

header = '''// MgaErr.c
// Error table file automatically generated from Interfaces/Mga.idl
// Genarated on %s 
//           by %s 
//           from %s
//           to %s
//           using script %s

const struct errtab MgaErrTab[]= {
''' % (time.ctime(), getpass.getuser(), i_path, o_path, sys.argv[0]) 

o.write(header)

l = i.readline()
while l:
	if intable:
		m = rhelpstring.match(l)
		if m:
			hs = m.group(1)
		  
		m = rhresult.match(l)
		if m:
			ecode = m.group(1)
			if not hs:
				print "WARNING: could not find helpstring for error", ecode
				hs = "Unkown error"
			o.write('{%s, L%s},\n' % (ecode, hs))
			hs = None

	if rbegin.search(l):
		intable = True

	if rend.search(l):
		intable = False
	
	l = i.readline()
	
footer = '''	{0}};\n'''

o.write(footer)

	
o.close()
i.close()

print 'Done.'
