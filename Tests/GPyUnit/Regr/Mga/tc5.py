import unittest
import win32com.client
import win32ui
import pythoncom
import os
import utils.Builder
bd = utils.Builder

from GPyUnit.util import DispatchEx
from GPyUnit.util import dec_disable_early_binding

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestCase5( unittest.TestCase ):
	"""
	avoid if possible the overriding of __init__, yet if you override use the following form:
	def __init__(self, methodName='runTest'):
		unittest.TestCase.__init__(self, methodName)
	"""

	def setUp( self ):		## hook method
		self.project = None

	def tearDown( self ):		## hook method
		if self.project:
			self.project.Close( 0 )


	"""
	def testA( self ):

		" ""    DESCRIPTION
			GetAbsMetaPath([in, out] BSTR *abspath)
			GetRelMetaPath([in] IMgaFCO *begfco, [in, out] BSTR *relpath, [in, defaultvalue(0)] IMgaFCOs *refs)
			
			RESU: relmetapath how can GetRelMetaPath be called from Python
		" ""
		
		pass
	"""

	def populate5B(self, p):
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'Folder'

		compA1 = bd.newObjInFold( p, folder1, 'Compound')
		compA1.Name = 'CompoundA'

		ppM1 = bd.new( p, compA1, 'CompoundParts')
		ppM1.Name = 'CompoundPartsA'

		s1 = bd.new( p, ppM1, 'InputSignals')
		s1.Name = 'inSig'

		s2 = bd.new( p, ppM1, 'OutputSignals')
		s2.Name = 'ouSig'

	@dec_disable_early_binding
	def testB( self ):

		"""    testB
			CreateCollection([in,out] IMgaFCOs **pVal)
		"""
		
		mganame = _adjacent_file("_tc5_B_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate5B( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		project.Open( "MGA=" + mganame )

		terr = project.CreateTerritory( None, None, None)
		trans = project.BeginTransaction( terr)

		fco = bd.findInProj( project, "CompoundA")
		s1  = bd.findInProj( project, "inSig")
		s2  = bd.findInProj( project, "ouSig")
		
		coll = fco.CreateCollection()
		assert coll.Count == 1
		assert coll.Item( 1 ).Name == fco.Name
		
		coll.Append( fco)
		assert coll.Count == 2
		assert coll.Item( 1 ) == fco
		assert coll.Item( 2 ) == fco
		assert coll.Item( 1 ).Name == fco.Name
		assert coll.Item( 2 ).Name == coll.Item( 1 ).Name
		
		col2 = s1.CreateCollection()
		assert col2.Count == 1
		assert col2.Item( 1 ) == s1
		assert col2.Item( 1 ).Name == s1.Name
		
		col2.Append( s2)
		assert col2.Count == 2
		assert col2.Item( 1 ) == s1
		assert col2.Item( 2 ) == s2
		assert col2.Item( 1 ).Name == s1.Name
		assert col2.Item( 2 ).Name == s2.Name
		
		col2.Append( fco)
		assert col2.Count == 3
		assert col2.Item( 1 ) == s1
		assert col2.Item( 2 ) == s2
		assert col2.Item( 3 ) == fco
		assert col2.Item( 1 ).Name == s1.Name
		assert col2.Item( 2 ).Name == s2.Name
		assert col2.Item( 3 ).Name == fco.Name

		# abort trans (thus revert any changes)
		project.AbortTransaction()
		
		project.Save()
		project.Close(0)

		pass

	def populate5C(self, p):
		folder0 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder0.Name = 'Folder0'

		comp0 = bd.newObjInFold( p, p.RootFolder, 'Compound')
		comp0.Name = 'Compound0'
		
		folder1 = bd.newFolder( p, folder0, 'Folder')
		folder1.Name = 'Folder1'

		folder2 = bd.newFolder( p, folder1, 'Folder')
		folder2.Name = 'Folder2'

		comp1 = bd.newObjInFold( p, folder0, 'Compound')
		comp1.Name = 'Compound1'

		comp2 = bd.newObjInFold( p, folder1, 'Compound')
		comp2.Name = 'Compound2'
		
		prim3 = bd.newObjInFold( p, folder2, 'Primitive')
		prim3.Name = 'Primitive3'

		pp3 = bd.new( p, comp2, 'CompoundParts')
		pp3.Name = 'CompoundParts3'

		is3 = bd.new( p, comp2, 'InputSignals')
		is3.Name = 'InSig3'
		
		pp4 = bd.new( p, pp3, 'CompoundParts')
		pp4.Name = 'CompoundParts4'

		ip4 = bd.new( p, pp3, 'InputParameters')
		ip4.Name = 'InPar4'

		pp5 = bd.new( p, pp4, 'CompoundParts')
		pp5.Name = 'CompoundParts5'

		os5 = bd.new( p, pp4, 'OutputSignals')
		os5.Name = 'OutSig5'
		
		is4 = bd.new( p, prim3, 'InputSignals')
		is4.Name = 'InputSig4'
		
		

	@dec_disable_early_binding
	def testC( self ):

		"""    testC
			testing property of IMgaFCO
				RootFCO
			
			it should return the first object which is in the hierarchy ( below folders )
		"""
		
		mganame = _adjacent_file("_tc5_C_sf.mga")
		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate5C( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		project.Open( "MGA=" + mganame )
		
		terr = project.CreateTerritory( None, None, None)
		trans = project.BeginTransaction( terr)
		
		cm0 = bd.findInProj( project, "Compound0")
		cm1 = bd.findInProj( project, "Compound1")
		cm2 = bd.findInProj( project, "Compound2")
		cm3 = bd.findInProj( project, "CompoundParts3")
		cm4 = bd.findInProj( project, "CompoundParts4")
		cm5 = bd.findInProj( project, "CompoundParts5")
		pm3 = bd.findInProj( project, "Primitive3")
		
		is3 = bd.findInProj( project, "InSig3")
		ip4 = bd.findInProj( project, "InPar4")
		os5 = bd.findInProj( project, "OutSig5")
		is4 = bd.findInProj( project, "InputSig4")
		
		assert cm0 == cm0.RootFCO
		assert cm1 == cm1.RootFCO
		assert cm2 == cm2.RootFCO
		assert cm2 == cm3.RootFCO
		assert cm2 == cm4.RootFCO
		assert cm2 == cm5.RootFCO
		
		assert cm2 == is3.RootFCO
		assert cm2 == ip4.RootFCO
		assert cm2 == os5.RootFCO

		assert pm3 == pm3.RootFCO
		assert pm3 == is4.RootFCO

		# the root objects are usually placed in folders
		assert cm1.ParentFolder		# it should be OK
		try:
		 assert cm1.ParentModel				# it should fail
		 self.fail("It should have failed")		# the previous line should have thrown an exception
		except:
		 #good, it doesn't have a model parent, so 'cm1.ParentModel' should be null
		 pass
		
		
		# their parents are folders
		assert cm2.ParentFolder
		assert pm3.ParentFolder
		
		# abort trans (thus revert any changes)
		project.AbortTransaction()
		
		project.Save()
		project.Close(0)

		pass


"""
	Let's produce the test suites
"""
# 1st way
def suites():
	suite = unittest.makeSuite(TestCase5,'test')
	return suite

# 2nd way
def suites3():
	suite = unittest.TestSuite()
	suite.addTest( TestCase5( "testA"))
	#suite.addTest( TestCase5( "testB"))
	#suite.addTest( TestCase5( "testC"))
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
	# unittest.main(defaultTest='TestCase5')
	
	######## 3rd way
	# all = suites3()
	# runner = unittest.TextTestRunner()
	# runner.run( all )
