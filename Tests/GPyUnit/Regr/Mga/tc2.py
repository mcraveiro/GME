import unittest
import win32com.client
import win32ui
import pythoncom
import os
import utils.Builder
bd = utils.Builder
from GPyUnit.util import DispatchEx

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

"""
tests properties of IMgaProject
"""
class TestCase2( unittest.TestCase ):
	m_errors = 0


	def setUp( self ):		## hook method: creates a new mga file all the times, and deletes one if existing
		self.project = DispatchEx("Mga.MgaProject")
		
		self.mganame = _adjacent_file("_tc2_all_sf.mga")

		try:
			os.remove( self.mganame)
		except:
			pass

		try:
			self.project.Create( "MGA=" + self.mganame, "SF")
		except:
			print "Project Create failed"
			raise
			
		self.terr = self.project.CreateTerritory( None, None, None)
		
		self.trans = self.project.BeginTransaction( self.terr, 0) # TRANSACTION_GENERAL
		
	def tearDown( self ):		## hook method
		#print "Project status is " + str( self.project.ProjectStatus())
		self.project.CommitTransaction()
		
		self.project.Save()
		self.project.Close(0)

	def testA( self ):		## test case 1
		proj_name = self.project.Name
		proj_name = proj_name + "_new"
		self.project.Name = proj_name
		assert self.project.Name == proj_name

	def testB( self ):		
		proj_vers = self.project.Version
		proj_vers = proj_vers + ".1"
		self.project.Version = proj_vers
		assert self.project.Version == proj_vers
		
	def testC( self ):
		proj_meta_name = self.project.MetaName
		assert proj_meta_name == "SF"
		
	def testD( self ):
		proj_meta_guid = list( self.project.MetaGUID )
		guid_conv = []
		all_zero = True
		for i in proj_meta_guid:
			guid_conv.append( hex( ord( i)))
			all_zero = all_zero and i == 0

		assert all_zero == False
		pass
		
	def testE( self ):
		proj_meta_vers = self.project.MetaVersion
		assert proj_meta_vers == ""
		
	def testF( self ):
		proj_create_time = self.project.CreateTime
		assert proj_create_time != ""
		
	def testG( self ):
		proj_change_time = self.project.ChangeTime
		assert proj_change_time != ""
		
	def testH( self ):
		proj_author = "GmeTest::TestCase2"
		self.project.Author = proj_author
		assert proj_author == self.project.Author
		
	def testI( self ):
		proj_comment = "created from python script"
		self.project.Comment = proj_comment
		assert proj_comment == self.project.Comment
		
	def testJ( self ):
		proj_conn_str = self.project.ProjectConnStr
		assert proj_conn_str.find("MGA=") == 0
		assert proj_conn_str.find( self.mganame) != -1
		
	def testK( self ):
		parad_conn_str = self.project.ParadigmConnStr
		assert parad_conn_str.find( "SF") != -1
		
	def testL( self ):
		# the rootfolder has always 1000 as metaref
		rootfld = self.project.MetaObj( 1000 )
		assert rootfld.Name == "RootFolder"
		assert rootfld.MetaRef == 1000
		
		metafld = self.project.MetaObj( 1053 )
		assert metafld.Name == "Folder"
		assert metafld.MetaRef == 1053
	

"""
	Let's produce the test suites
"""
def suites():
	suite = unittest.makeSuite(TestCase2,'test')
	return suite

def suites2():
	suite = unittest.TestSuite()
	suite.addTest( TestCase2( "testA"))
	#suite.addTest( TestCase2( "testB"))
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
	# unittest.main()

	######## 2nd way
	unittest.main(defaultTest='TestCase2')
	
	######## 3rd way
	# all = suites3()
	# runner = unittest.TextTestRunner()
        # runner.run( all )
