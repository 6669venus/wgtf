#! /usr/bin/python

import sys
import time
import re
import os.path


def replaceBuildDate( filePath ):
	"""
	Replace the formatted build date string in the file specified by filePath.
	"""
	try:
		now = time.strftime( '%H:%M:%S %b %d %Y', time.localtime() )		
		if len(now) != 20:
			print 'Length of now string is not 20: ', now
			return 1

		dateStr = '<bwBuildDate>' + now + '\x00</bwBuildDate>'
		datePattern = '<bwBuildDate>.{20}\x00</bwBuildDate>'

		f =  open( filePath, 'r+b' )
		data = f.read()

		matches = len( re.findall( datePattern, data ) )
		print "[BUILD] Replaced %d matched build date string(s) in %s" % \
			( matches, os.path.basename( filePath ) )

		f.seek( 0 )
		f.write( re.sub( datePattern, dateStr, data ) )

		f.close()
	except Exception, ex:
		print 'Failed to open', filePath, ':', ex
		return 1

	return 0


if __name__ == '__main__':
	if len( sys.argv ) != 2:
		print 'Usage: ', sys.argv[0], '<path of the file to modify>'
		sys.exit( 1 )

	sys.exit( replaceBuildDate( sys.argv[1] ) )
