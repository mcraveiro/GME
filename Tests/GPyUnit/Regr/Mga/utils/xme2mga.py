# creates .mga file from .xme file
# If directory specified as first parameter all .xme files in the directory will be converted
# the .mga files are placed into the directory specified by the 2nd parameter
#
# the paradigm used is guessed based on the file name:
# _tc_sf_*.xme => the substring between the 2nd and 3rd '_' characters represents the paradigm name, in this case SF 
#
#
import sys
from GPyUnit.util import DispatchEx
import os

def xmefiles( dir, begin_pattern = "", end_pattern = "" ):
	ld = os.listdir( dir)
	xmes = []
	for k in ld:
		if k.endswith( end_pattern + ".xme") and k.startswith( begin_pattern):
			xmes.append( k )
	
	return xmes


def doit( source, target):
	errors = 0
	errors_with_files = ""

	fs = []
	source_dir = ''
	target_dir = target

	if os.path.isdir( source):
		source_dir = source
		dir2load = os.path.abspath(os.path.join( os.path.curdir, source_dir))
		fs = xmefiles( dir2load)
	elif os.path.isfile( source):
		fs.append( source)
	else:
		raise 'File/Directory not given consistently'

	
	for i in fs:
		#fname   = os.path.join( target_dir,  s.path.split(i)[1] + ".xme" )
		xmename = os.path.abspath( os.path.join( os.path.curdir, i))

		try:
			gme = DispatchEx("GME.Application")
			gme.Visible = 0
			
			paradigm = "MetaGME"
			# naming pattern: _tc5_A_sf.mga.xme
			# the part between the last '_' and the first '.' char indicates the paradigm name
			j = i[ : i.find('.')]
			if j.count('_') >= 1:
			    u = j.rfind('_')
			    p = j[ u + 1 : ]
			    if p == 'sf':
			        paradigm = "SF"
			    elif p == 'fl':
			        paradigm = "FloatAttr"
			    elif p == 'me':
			        paradigm = "MetaGME"
			    else:
			        print 'Other paradigm !!!'
			        pass
			else:
			    print 'Other naming pattern !!!'
			    pass
			
			name = 'newproject'
			i = os.path.split(i)[1]
			if i.count('.') > 0:
			    name = i[:i.rfind('.')]

			mgan = os.path.abspath( os.path.join( target_dir, name ))
			gme.CreateProject( paradigm, "MGA=" + mgan)
			
			gme.ImportProject(xmename)
			gme.SaveProject()
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
		source = "xme"
		target = "./"
		print "="*79
		print "Usage: xme2mga.py Source Target"
		print "       where Source can be a directory or file"
		print "             Target must be a directory"
		print "i.e.: xme2mga.py", source, target
		print "i.e.: xme2mga.py xme\\_tc5_sf_A.mga.xme models\\"
		print "="*79
		sys.exit(1)
	else:
		source = sys.argv[1]
		target = sys.argv[2]

	(nerrs, errfiles) = doit( source, target)
	if nerrs > 0:
		print "Problematic files: \n" + errfiles
