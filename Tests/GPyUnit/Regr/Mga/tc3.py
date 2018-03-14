import unittest
import win32com.client
import win32ui
import pythoncom
import os
import utils.Builder
from GPyUnit.util import DispatchEx
bd = utils.Builder

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestCase3( unittest.TestCase ):
	def setUp( self ):		## hook method
		pass

	def tearDown( self ):		## hook method
		pass

	def testA( self ):
		"""    testA

		Create				a project
		Close                           the project
		QueryProjectInfo
		
		"""

		# this file will be created:
		mganame = _adjacent_file("_tc3_A_sf.mga")
		project = DispatchEx("Mga.MgaProject")

		try:
			project.Create( "MGA=" + mganame, "SF")
		except:
			print "Project Create failed"
			raise
			
		project.Save()
		project.Close(0)

		t = list( project.QueryProjectInfo( "MGA=" + mganame))
		self.assertEqual(t[0], 2) # mgaversion
		assert t[1] == "SF"	# par name
		assert t[2] == ""	# par version
		assert t[4] == 0	# readonly access ?
		
		t3_guid = list( t[3] )	# convert binary part to a list
		
		t3_cvd = map( ord, t3_guid)
		
		isnull = lambda i: (i == 0 and [True] or [False])[0]
		len1 = filter( isnull, t3_cvd)
		assert len(len1) != 16

		
	def testB( self ):
		"""    testB

		Create				a project
		Close                           the project
		Open				a project
		QueryProjectInfo
		
		"""

		# this file will be created:
		mganame = _adjacent_file("_tc4_B_sf.mga")
		project = DispatchEx("Mga.MgaProject")

		try:
			project.Create( "MGA=" + mganame, "SF")
		except:
			self.fail('Project create failed')
			print "Project Create failed"
			
		project.Save()
		project.Close(0)

		# now this file has to exist: mganame
		try:
			project.Open( "MGA=" + mganame)
		except:
			print "Project open failed"
			raise

		project.Save()
		project.Close(0)

		t = list( project.QueryProjectInfo( "MGA=" + mganame))
		self.assertEqual(t[0], 2) # mgaversion
		assert t[1] == "SF"	# par name
		assert t[2] == ""	# par version
		assert t[4] == 0	# readonly access ?
		
		t3_guid = list( t[3] )	# convert binary part to a list
		
		t3_cvd = map( ord, t3_guid)
		
		isnull = lambda i: (i == 0 and [True] or [False])[0]
		len1 = filter( isnull, t3_cvd)
		assert len(len1) != 16 # it can be all null
		
		

"""
	Let's produce the test suites
"""
def suites():
	suite = unittest.makeSuite(TestCase3,'test')
	return suite

def suites2():
	suite = unittest.TestSuite()
	suite.addTest( TestCase3( "testA"))
	#suite.addTest( TestCase3( "testB"))
	#suite.addTest( TestCase3( "testC"))
	return suite

########
# main
########
if __name__ == "__main__":
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TEST BEGINS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	
	######## 1st way
	unittest.main()

	######## 2nd way
	# unittest.main(defaultTest='TestCase3')
	
	######## 3rd way
	# all = suites3()
	# runner = unittest.TextTestRunner()
	# runner.run( all )
