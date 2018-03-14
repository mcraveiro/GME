import unittest
import win32com.client
import os
import GPyUnit.Regr.Mga.utils.Builder
bd = GPyUnit.Regr.Mga.utils.Builder
from GPyUnit.util import DispatchEx

from GPyUnit.util import dec_disable_early_binding

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestCase1( unittest.TestCase ):
	"""
	avoid if possible the overriding of __init__, yet if you override use the following form:
	def __init__(self, methodName='runTest'):
		unittest.TestCase.__init__(self, methodName)
	"""

	def setUp( self ):		## hook method
		pass

	def tearDown( self ):		## hook method
		pass

	def populateA(self, p):
		folder1 = p.RootFolder.CreateFolder( bd.folder( p, 'SheetFolder'))
		folder1.Name = 'SheetFolder1'
		
		parsh1 = folder1.CreateRootObject( bd.kind( p, 'ParadigmSheet'))
		parsh1.Name = 'ParadigmSheet1'
		
		at1 = bd.new( p, parsh1, 'Atom')
		at1.Name = 'Atom1'

		at2 = bd.new( p, parsh1, 'Atom')
		at2.Name = 'Atom2'
		
		at3 = bd.new( p, parsh1, 'Atom')
		at3.Name = 'Atom3'

		ap1 = bd.new( p, parsh1, 'AtomProxy')
		ap1.Name = 'AtomProx1'

		ap2a = bd.new( p, parsh1, 'AtomProxy')
		ap2a.Name = 'AtomProx2a'
		
		ap2b = bd.new( p, parsh1, 'AtomProxy')
		ap2b.Name = 'AtomProx2b'

		ap1.Referred = at1
		ap2a.Referred = at2
		ap2b.Referred = at2
		

	@dec_disable_early_binding
	def testA( self ):
		"""    testA References
			testing getproperties of IMgaFCO
			 ReferencedBy([out, retval] IMgaFCOs **pVal)
		"""
		
		mganame = _adjacent_file("_tc1_A_me.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "MetaGME")
		if not project:
			self.fail("Create failed")
		try:
			self.populateA( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)


		# open the created project
		self.project = project
		try:
			project.Open( "MGA=" + mganame )
		except:
			project.Close(0)
			raise
			
		trans = project.BeginTransactionInNewTerr(0)
		
		at1 = bd.findInProj( project, "Atom1")
		at2 = bd.findInProj( project, "Atom2")
		at3 = bd.findInProj( project, "Atom3")
		atp1 = bd.findInProj( project, "AtomProx1")
		atp2a = bd.findInProj( project, "AtomProx2a")
		atp2b = bd.findInProj( project, "AtomProx2b")
		
		# test at1 with one reference
		assert at1.ReferencedBy.Count == 1
		self.assertEquals(at1.ReferencedBy.Item(1).ID, atp1.ID)
		#assert at1.ReferencedBy[0] == atp1

		# test at2 with two references
		refcoll = at2.ReferencedBy
		assert refcoll.Count == 2
		exp_refs = ( atp2a, atp2b )
		assert refcoll.Item( 1 ).ID in [obj.ID for obj in exp_refs]
		assert refcoll.Item( 2 ).ID in [obj.ID for obj in exp_refs]
		#assert refcoll[0] in exp_refs
		#assert refcoll[1] in exp_refs
		

		# test at3 with no references
		assert at3.ReferencedBy.Count == 0
		
		# abort trans (thus revert any changes)
		project.AbortTransaction()
		
		project.Save()
		project.Close(0)
		
		pass

	def populateB(self, p):
		folder1 = p.RootFolder.CreateFolder( bd.folder( p, 'SheetFolder'))
		folder1.Name = 'SheetFolder1'
		
		parsh1 = folder1.CreateRootObject( bd.kind( p, 'ParadigmSheet'))
		parsh1.Name = 'ParadigmSheet1'
		
		at1 = bd.new( p, parsh1, 'Atom')
		at1.Name = 'Atom1'

		at2 = bd.new( p, parsh1, 'Atom')
		at2.Name = 'Atom2'
		
		at3 = bd.new( p, parsh1, 'Atom')
		at3.Name = 'Atom3'

		asp1 = bd.new( p, parsh1, 'Aspect')
		asp1.Name = 'Aspect1'

		asp2 = bd.new( p, parsh1, 'Aspect')
		asp2.Name = 'Aspect2'
		
		asp3 = bd.new( p, parsh1, 'Aspect')
		asp3.Name = 'Aspect3'

		# atom1 added to all three aspects
		asp1.AddMember(at1)
		asp2.AddMember(at1)
		asp3.AddMember(at1)
		
		# atom2 added to aspect2
		asp2.addMember(at2)

	@dec_disable_early_binding
	def xtestB( self ):
		"""    testB Sets
			testing getproperties of IMgaFCO
			 MemberOfSets([out, retval] IMgaFCOs **pVal)
		"""
		
		mganame = _adjacent_file("_tc1_B_me.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "MetaGME")
		if not project:
			self.fail("Create failed")
		try:
			self.populateB( project)
		except:
			bd.saveP( project)
			raise

		bd.saveP( project)


		# open
		self.project = project
		try:
			project.Open( "MGA=" + mganame )
		except:
			project.Close(0)
			raise

		trans = project.BeginTransactionInNewTerr(0)
		
		as1 = bd.findInProj( project, "Aspect1")
		as2 = bd.findInProj( project, "Aspect2")
		as3 = bd.findInProj( project, "Aspect3")
		at1 = bd.findInProj( project, "Atom1")
		at2 = bd.findInProj( project, "Atom2")
		at3 = bd.findInProj( project, "Atom3")
		
		# test at3 with no membership
		assert at3.MemberOfSets.Count == 0

		# test at2 with one membership
		assert at2.MemberOfSets.Count == 1
		assert at2.MemberOfSets.Item( 1 ) == as2
		assert at2.MemberOfSets[0] == as2

		# test at1 with 3 memberships
		sets = at1.MemberOfSets
		assert sets.Count == 3
		exp_sets = ( as1, as2, as3 )
		assert sets.Item( 1 ) in exp_sets
		assert sets.Item( 2 ) in exp_sets
		assert sets.Item( 3 ) in exp_sets
		assert sets[0] in exp_sets
		assert sets[1] in exp_sets
		assert sets[2] in exp_sets
		
		# abort trans (thus revert any changes)
		project.AbortTransaction()
		
		project.Save()
		project.Close(0)
		
		pass

	def populateC(self, p):
		folder1 = p.RootFolder.CreateFolder( bd.folder( p, 'SheetFolder'))
		folder1.Name = 'SheetFolder1'
		
		parsh1 = folder1.CreateRootObject( bd.kind( p, 'ParadigmSheet'))
		parsh1.Name = 'ParadigmSheet1'
		
		mo1   = bd.new( p, parsh1, 'Model')
		mo1.Name = 'Model1'
		
		at1 = bd.new( p, parsh1, 'Atom')
		at1.Name = 'Atom1'

		at2 = bd.new( p, parsh1, 'Atom')
		at2.Name = 'Atom2'
		
		at3 = bd.new( p, parsh1, 'Atom')
		at3.Name = 'Atom3'

		# create a connection kind
		connection1 = bd.new( p, parsh1, 'Connection')
		connection1.Name = 'AssocClass1'
		
		# create a connector kind
		connector1 = bd.new( p, parsh1, 'Connector')
		connector1.Name = 'ConnPoint1'
		
		#HRESULT CreateSimpleConn([in] IMgaMetaRole *meta, [in] IMgaFCO *src, [in] IMgaFCO *dst,
		#			 [in] IMgaFCOs *srcrefs, [in] IMgaFCOs *dstrefs, [out] IMgaFCO **newobj);
		
		# this dummy collection will be used for those connections which do not involve connections
		z0 = DispatchEx("Mga.MgaFCOs")
		
		try:
		 # create a containment relation between atom1 and model1
		 cc1 = parsh1.CreateSimpleConn( bd.role(p, parsh1, 'Containment'), at1, mo1, z0, z0)
		 cc1.Name = 'Containment_a1_to_m'
		except:
		 print 'Could not create containment1'
		 raise
		
		try:
		 # create a containment relation between atom2 and model1
		 cc2 = parsh1.CreateSimpleConn( bd.role(p, parsh1, 'Containment'), at2, mo1, z0, z0)
		 cc2.Name = 'Containment_a2_to_m'
		except:
		 print 'Could not create containment2'
		 raise
		
		try:
		 # create a 'source2conn' relation between at2 and connector
		 sc = parsh1.CreateSimpleConn( bd.role(p, parsh1, 'SourceToConnector'), at1, connector1, z0, z0)
		 sc.Name = 'SourceToConnector_to_cp'
		except:
		 print 'Could not create source2conn'
		 raise

		try:
		 # create a 'conn2dest' relation between connector and at2
		 dc = parsh1.CreateSimpleConn( bd.role(p, parsh1, 'ConnectorToDestination'), connector1, at1, z0, z0)
		 dc.Name = 'ConnectorToDestination_from_cp'
		except:
		 print 'Could not create conn2dest'
		 raise
		
		try:
		 ac = parsh1.CreateSimpleConn( bd.role(p, parsh1, 'AssociationClass'), connection1, connector1, z0, z0)
		 ac.Name = 'AssocLink' 
		except:
		 print 'Could not create assoc'
		 raise


	@dec_disable_early_binding
	def testC( self ):
		"""    testC connections in MetaGME
			testing getproperties of IMgaFCO
			 PartOfConns([out, retval] IMgaConnPoints **pVal)
			
			and getproperties of IMgaConnPoint
			  Owner([out, retval] IMgaConnection **pVal)
			  ConnRole([out, retval] BSTR *pVal)
			  Target([out, retval] IMgaFCO **pVal)
			  References([out, retval] IMgaFCOs **pVal)

		"""
		mganame = _adjacent_file("_tc1_C_me.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "MetaGME")
		if not project:
			self.fail("Create failed")
		try:
			self.populateC( project)
		except:
			bd.saveP( project)
			raise

		bd.saveP( project)

		# open
		self.project = project
		try:
			project.Open( "MGA=" + mganame )
		except:
			project.Close(0)
			raise
					
		trans = project.BeginTransactionInNewTerr(0)
		
		mo1 = bd.findInProj( project, "Model1")
		at1 = bd.findInProj( project, "Atom1")
		at2 = bd.findInProj( project, "Atom2")
		at3 = bd.findInProj( project, "Atom3")
		cp1 = bd.findInProj( project, "ConnPoint1")
		
		# test at3 with no conns
		assert at3.PartOfConns.Count == 0

		# test at2 with one connection

		self.failUnless( at2.PartOfConns.Count == 1, 'Bad count:' +  str( at2.PartOfConns.Count))
		assert at2.PartOfConns.Item( 1 ).Target.ID == at2.ID
		assert at2.PartOfConns.Item( 1 ).Owner.Name == "Containment_a2_to_m"
		assert at2.PartOfConns.Item( 1 ).ConnRole == "src"
		assert at2.PartOfConns.Item( 1 ).References.Count == 0

		# test at1 with 3 connections
		cps = at1.PartOfConns		# RESU: Win32 exception releasing IUnknown
		self.failUnless( cps.Count == 3, 'Bad count:' + str( cps.Count))

		conn_name_a = "Containment_a1_to_m"
		conn_name_b = "SourceToConnector_to_cp"
		conn_name_c = "ConnectorToDestination_from_cp"

		conn_a = bd.findInProj( project, conn_name_a)
		conn_b = bd.findInProj( project, conn_name_b)
		conn_c = bd.findInProj( project, conn_name_c)
		
		# conn tuple a1 is expected to be part of
		exp_conns = ( conn_a, conn_b, conn_c )
		exp_conn_names = ( conn_name_a, conn_name_b, conn_name_c)

		# dict of [Name, Role]s ; the roles a1 is expected to participate in the connections
		exp_conn_role = { conn_name_a:"src", conn_name_b:"src", conn_name_c:"dst" }

		# check
		for cp in cps:
			assert cp.References.Count == 0
			assert cp.Target.ID == at1.ID # "Atom1" is the target of the connection point
			assert cp.Owner.ID in [obj.ID for obj in exp_conns ]# the connections
			#assert cp.Owner.Name in exp_conn_names	# like above

			assert exp_conn_role.has_key( cp.Owner.Name )
			assert exp_conn_role[ cp.Owner.Name ] == cp.ConnRole

		
		# test the connectionpoint and its links
		cps = cp1.PartOfConns

		conn_name_a = "AssocLink"
		conn_name_b = "SourceToConnector_to_cp"
		conn_name_c = "ConnectorToDestination_from_cp"

		conn_a = bd.findInProj( project, conn_name_a)
		conn_b = bd.findInProj( project, conn_name_b)
		conn_c = bd.findInProj( project, conn_name_c)
		
		# conn tuple cp1 is expected to be part of
		exp_conns = ( conn_a, conn_b, conn_c )
		exp_conn_names = ( conn_name_a, conn_name_b, conn_name_c)

		# dict of [Name, Role]s ; the roles cp1 is expected to participate in the connections
		exp_conn_role = { conn_name_a:"dst", conn_name_b:"dst", conn_name_c:"src" }

		# check
		for cp in cps:
			assert cp.References.Count == 0
			assert cp.Target.ID == cp1.ID		# cp1 is the target of the connection point
			assert cp.Owner.ID in [obj.ID for obj in exp_conns] # the connections
			#assert cp.Owner.Name in exp_conn_names	# like above

			assert exp_conn_role.has_key( cp.Owner.Name )
			self.failUnless( exp_conn_role[ cp.Owner.Name ] == cp.ConnRole, 'Got: ' + cp.ConnRole + ' Expected: ' + exp_conn_role[ cp.Owner.Name ] )


		
		# abort trans (thus revert any changes)
		project.AbortTransaction()
		
		project.Save()
		project.Close(0)

		
		pass

	def populateD(self, p):
		folder1 = p.RootFolder.CreateFolder( bd.folder( p, 'Folder'))
		folder1.Name = 'Folder1'
		
		folder2 = p.RootFolder.CreateFolder( bd.folder( p, 'Folder'))
		folder2.Name = 'Folder2'

		comp1 = p.RootFolder.CreateRootObject( bd.kind( p, 'Compound'))
		comp1.Name = 'Compound1'
		
		comp2 = p.RootFolder.CreateRootObject( bd.kind( p, 'Compound'))
		comp2.Name = 'Compound2'
		
		is1 = bd.new( p, comp1, 'InputSignals')
		is1.Name = 'Is'

		os1 = bd.new( p, comp1, 'OutputSignals')
		os1.Name = 'Os'

		compA = bd.new( p, comp1, 'CompoundParts')
		compA.Name = 'CompoundPartsA'

		compB = bd.new( p, comp1, 'CompoundParts')
		compB.Name = 'CompoundPartsB'
		
		portIsA = bd.new( p, compA, 'InputSignals')
		portIsA.Name = 'IsA1'
		portOsA = bd.new( p, compA, 'OutputSignals')
		portOsA.Name = 'OsA1'

		portIpA = bd.new( p, compA, 'InputParameters')
		portIpA.Name = 'IpA1'
		portOpA = bd.new( p, compA, 'OutputParameters')
		portOpA.Name = 'OpA1'

		portIsB = bd.new( p, compB, 'InputSignals')
		portIsB.Name = 'IsB1'
		portOsB = bd.new( p, compB, 'OutputSignals')
		portOsB.Name = 'OsB1'

		portIpB = bd.new( p, compB, 'InputParameters')
		portIpB.Name = 'IpB1'
		portOpB = bd.new( p, compB, 'OutputParameters')
		portOpB.Name = 'OpB1'

		# this dummy collection will be used for those connections which do not involve connections
		z0 = DispatchEx("Mga.MgaFCOs")
		
		try:
		 # create a DFC relation between is1 and os1
		 dfc0 = comp1.CreateSimpleConn( bd.role(p, comp1, 'DFC'), is1, os1, z0, z0)
		 dfc0.Name = 'DFC0'
		except:
		 print 'Could not create DFC0'
		 raise

		try:
		 # create a DFC relation between portIsA and portOsB
		 dfc1 = comp1.CreateSimpleConn( bd.role(p, comp1, 'DFC'), portIsA, portOsB, z0, z0)
		 dfc1.Name = 'DFC1'

		 # create a DFC relation between portIsB and portOsA
		 dfc2 = comp1.CreateSimpleConn( bd.role(p, comp1, 'DFC'), portIsB, portOsA, z0, z0)
		 dfc2.Name = 'DFC2'
		except:
		 print 'Could not create DFC1, DFC2'
		 raise
		
		try:
		 # create a PC relation between portIpA and portOpB
		 pc1 = comp1.CreateSimpleConn( bd.role(p, comp1, 'PC'), portIpA, portOpB, z0, z0)
		 pc1.Name = 'PC1'

		 # create a PC relation between portIpB and portOpA using connect method of builder.py
		 # we could use: comp1.CreateSimpleConn( bd.role(p, comp1, 'PC'), portIpB, portOpA, z0, z0)
		 pc2 = bd.connect( p, comp1, portIpB, portOpA, 'PC')
		 pc2.Name = 'PC2'
		except:
		 print 'Could not create PC1, PC2'
		 raise
		pass

	@dec_disable_early_binding
	def testD( self ):
		"""    testD Connections in SF
			testing getproperties of IMgaFCO
			 PartOfConns([out, retval] IMgaConnPoints **pVal)
			
			and getproperties of IMgaConnPoint
			  Owner([out, retval] IMgaConnection **pVal)
			  ConnRole([out, retval] BSTR *pVal)
			  Target([out, retval] IMgaFCO **pVal)
			  References([out, retval] IMgaFCOs **pVal)
		"""
		
		mganame = _adjacent_file("_tc1_D_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populateD( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)


		# open the created project
		self.project = project
		try:
			project.Open( "MGA=" + mganame )
		except:
			project.Close(0)
			raise
			
		trans = project.BeginTransactionInNewTerr()
		
		mo1 = bd.findInProj( project, "Compound1")
		is1 = bd.findInProj( project, "Is")
		os1 = bd.findInProj( project, "Os")
		dc0 = bd.findInProj( project, "DFC0")
		
		pISA = bd.findInProj( project, "IsA1")
		pOSA = bd.findInProj( project, "OsA1")
		pISB = bd.findInProj( project, "IsB1")
		pOSB = bd.findInProj( project, "OsB1")
		dc1 = bd.findInProj( project, "DFC1")
		dc2 = bd.findInProj( project, "DFC2")
		
		# test mo1 with no conns
		assert mo1.PartOfConns.Count == 0

		# test is1,os1 with one connection
		assert is1.PartOfConns.Count == 1
		assert os1.PartOfConns.Count == 1
		
		assert is1.PartOfConns.Item( 1 ).Target.ID == is1.ID
		assert is1.PartOfConns.Item( 1 ).Owner.Name == "DFC0"
		assert is1.PartOfConns.Item( 1 ).ConnRole == "src"
		assert is1.PartOfConns.Item( 1 ).References.Count == 0

		assert os1.PartOfConns.Item( 1 ).Target.ID == os1.ID
		assert os1.PartOfConns.Item( 1 ).Owner.Name == "DFC0"
		assert os1.PartOfConns.Item( 1 ).ConnRole == "dst"
		assert os1.PartOfConns.Item( 1 ).References.Count == 0

		assert dc1.ConnPoints.Count == 2
		for cp in dc1.ConnPoints:
			assert cp.Owner.ID == dc1.ID
			assert cp.References.Count == 0
			
			if cp.ConnRole == 'src':
				assert cp.Target.ID == pISA.ID
			elif cp.ConnRole == 'dst':
				assert cp.Target.ID == pOSB.ID

		assert dc2.ConnPoints.Count == 2
		for cp in dc2.ConnPoints:
			assert cp.Owner.ID == dc2.ID
			assert cp.References.Count == 0
			
			if cp.ConnRole == 'src':
				assert cp.Target.ID == pISB.ID
			elif cp.ConnRole == 'dst':
				assert cp.Target.ID == pOSA.ID


		# abort trans (thus revert any changes)
		project.AbortTransaction()
		
		project.Save()
		project.Close(0)

"""
	Let's produce the test suites
"""
# 1st way
def suites():
	suite = unittest.makeSuite(TestCase1,'test')
	return suite

# 2nd way
def suites2():
	suite = unittest.TestSuite()
	#suite.addTest( TestCase1( "testB"))
	#suite.addTest( TestCase1( "testC"))
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
	# unittest.main(defaultTest='TestCase1')
	
	######## 3rd way
	#all = suites2()
	#runner = unittest.TextTestRunner()
	#runner.run( all )
