#!/usr/bin/env python

#
# Usage: qrc_generator.py <target.qrc> <prefix> <directory>
#

import os
import sys
import fnmatch

qrc_file = sys.argv[1]
prefix = sys.argv[2]
directory = os.path.abspath(sys.argv[3]).replace( '\\', '/' )

if directory[-1] != '/':
	directory += '/'

if not os.path.isdir(directory):
	sys.exit(-1)

files = []

for base, dirnames, filenames in os.walk(directory):
	for filename in fnmatch.filter(filenames, '*'):
		files.append(os.path.join(base, filename).replace( '\\', '/' ))

with open(qrc_file,'w') as qrc:
	qrc.write( '<RCC>\n\t<qresource prefix="/%s">\n' % prefix )
	for f in files:
		qrc.write('\t\t<file alias="%s">%s</file>\n' % \
			( f[len(directory):], f ) )

	qrc.write('\t</qresource>\n</RCC>\n')
