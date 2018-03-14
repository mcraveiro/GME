# makes .xme file from .mga format
# If directory specified as first parameter all .mga files in the directory will be converted
# the .xme files are placed into the directory specified by the 2nd parameter

import sys
from GPyUnit.util import DispatchEx
import os

def mgafiles( dir, begin_pattern = "", end_pattern = "" ):
	ld = os.listdir( dir)
	mgas = []
	for k in ld:
		if k.endswith( end_pattern + ".mga") and k.startswith( begin_pattern):
			mgas.append( k )
	
	return mgas


def doit( source, target):
	errors = 0
	errors_with_files = ""
	fs = []
	source_dir = ''
	target_dir = target

	if os.path.isdir( source):
		source_dir = source
		fs = mgafiles( os.path.abspath(os.path.join( os.path.curdir, source_dir)))
	elif os.path.isfile( source):
		fs.append( source)
	else:
		raise 'File/Directory not given consistently'
	
	for i in fs:
		mganame = os.path.join( source_dir, i)
		mganame = os.path.abspath( os.path.join( os.path.curdir, mganame))

		xmename = os.path.join( target_dir, os.path.split(i)[1] + ".xme")
		xmename = os.path.abspath( os.path.join( os.path.curdir, xmename))

		try:
			gme = DispatchEx("GME.Application")
			gme.Visible = 0
			gme.OpenProject( "MGA=" + mganame )
			
			gme.ExportProject( xmename )
			gme.CloseProject(0)
		except:
			errors = errors + 1
			errors_with_files = errors_with_files + mganame + "\n"
			gme.CloseProject(0)
	pass
	return ( errors, errors_with_files)


#main
if __name__ == "__main__":
	if len( sys.argv) < 3:
		source = "..\\models"
		target = "xme"
		print "="*79
		print "Usage: mga2xme.py Source Target"
		print "       where Source can be a directory or file"
		print "             Target must be a directory"
		print "i.e.: mga2xme.py", source, target
		print "i.e.: mga2xme.py ..\\models\\_tc5_sf_A.mga xme\\"
		print "="*79
		sys.exit(1)
	else:
		source = sys.argv[1]
		target = sys.argv[2]
	
	(nerrs, errfiles) = doit( source, target)
	if nerrs > 0:
		print "Problematic files: \n" + errfiles
