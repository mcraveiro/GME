import unittest
import win32com.client
import win32ui
import pythoncom
import os
import stat
import utils.Builder
bd = utils.Builder

from GPyUnit.util import DispatchEx
from GPyUnit.util import dec_disable_early_binding

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestCase4( unittest.TestCase ):
	#avoid if possible the overriding of __init__, yet if you override use the following form:
	#def __init__(self, methodName='runTest'):
	#	unittest.TestCase.__init__(self, methodName)

	def setUp( self ):		## hook method
		pass

	def tearDown( self ):		## hook method
		self.project.Close( 0 )
		pass
	
	def scramble_name( self, o_n):
		# shift letters by one character in the range of [32 - 126]
		n_n = ''
		for i in range( len( o_n)):
			n_n = n_n + chr( 32 + (1 + ( ord( o_n[i]) - 32) % (126-32) ))
		return n_n


	def populate4A(self, p):
		pass
	
	def testA( self ):
		"""   testA

		Open				a project
		Preferences			property inquired
		OperationsMask			property inquired
		CreateTerritory
		BeginTransaction
		Preferences			property inquired
		OperationsMask			property inquired
		Preferences			property set
		OperationsMask			property set
		CommitTransaction
		Preferences			property set to original value
		OperationsMask			property set to original value
		Save				project
		Close				project
		
		"""

		mganame = _adjacent_file("_tc4_A_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4A( project)
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
		
		o_pref = project.Preferences
		o_opms = project.OperationsMask
		
		terr = project.CreateTerritory( None, None, None)
		
		trans = project.BeginTransaction( terr)

		t_pref = project.Preferences
		t_opms = project.OperationsMask
		
		# compare the values when inquired inside a transaction with those out of transaction
		assert t_pref == o_pref
		assert t_opms == o_opms
		
		"""
		MGAPREF_NODUPLICATENAMES  = 0x00000001,		// make sure, names are never duplicated
		MGAPREF_NAMENEWOBJECTS    = 0x00000002,		// assign kind/rolename to new objects
		MGAPREF_IGNORECONNCHECKS =  0x00000004,		// do not complain on partially connected simple connections
		MGAPREF_FREEINSTANCEREFS =  0x00000008,		// do not bind refs in subtypes/instances to successors of ref target in basetype 
		MGAPREF_RELAXED_RDATTRTYPES =  0x00000010,	// attributes may be read as types other than the real type
		MGAPREF_RELAXED_WRATTRTYPES =  0x00000020,	// attributes may be written as types other than the real type
		MGAPREF_MANUAL_RELIDS		= 0x00000040,	// relid-s are assigned manually
		"""

		# try the put method inside transaction
		n_pref = 0x00000002
		n_opms = 0x40
		project.Preferences = n_pref
		project.OperationsMask = n_opms
		
		assert project.Preferences == n_pref
		assert project.OperationsMask == n_opms
		
		project.CommitTransaction()
		
		# try the put method outside transactions

		project.Preferences = o_pref
		project.OperationsMask = o_opms
		
		assert project.Preferences == o_pref
		assert project.OperationsMask == o_opms

		project.Save()
		project.Close(0)

	def populate4B(self, p):
		pass
		
	def testB( self ):
		"""    testB

		Open				a project
		CreateTerritory
		BeginTransaction
		Name				property changed
		Comment				property changed
		ActiveTerritory			inquired
		CommitTransaction
		Save				project
		Close				project
		
		"""

		mganame = _adjacent_file("_tc4_B_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4B( project)
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

		terr = project.CreateTerritory( None, None, None)
		
		trans = project.BeginTransaction( terr)

		old_name = project.Name
		new_name = self.scramble_name( old_name)

		# modify and test
		project.Name = new_name
		assert project.Name == new_name

		# revert and test
		project.Name = old_name
		assert project.Name == old_name

		old_comm = project.Comment
		new_comm = ' <sp> '.join( old_comm.split(' '))

		# modify and test
		project.Comment = new_comm
		assert project.Comment == new_comm
		
		# revert and test
		project.Comment = old_comm
		assert project.Comment == old_comm
		
		assert project.ActiveTerritory == terr

		project.CommitTransaction()

		project.Save()
		project.Close(0)

	def populate4C(self, p):
		pass

	def testC( self ):
		"""    testC

		ProjectStatus			inquired
		Open				a project
		CreateTerritory			once
		CreateTerritory			twice
		ProjectStatus			inquired
		BeginTransaction
		ProjectStatus			inquired
		Name				property changed
		Territories			inquired
		AbortTransaction
		ProjectStatus			inquired
		BeginTransaction readonly
		ProjectStatus			inquired
		Name				property inquired
		CommitTransaction
		ProjectStatus			inquired
		Save				project
		Close				project
		ProjectStatus			inquired
		
		Project status bits to be interpreted like:
		0 :	is open? 
		1 :	open r/o? 
		2 :	has changed? 
		3 :	in transaction?, 
		4 :	transaction r/o?, 
		31:	error

		"""

		mganame = _adjacent_file("_tc4_C_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4C( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		stat = project.ProjectStatus
		assert stat & 1 == 0	# is NOT open
		assert stat & 2 == 0	# is NOT read-only
		assert stat & 8 == 0	# is NOT in transaction

		# provoke error bit
		try :
			project.Open( "MJA=" + mganame ) #intentional error
			# it should fail
			assert 0 
		except:
			stat = project.ProjectStatus
			assert stat & 1 == 0	# is NOT open
			assert stat & 2 == 0	# is NOT read-only
			assert stat & (1 << 31) == 0	# ERROR flag is TRUE

		# open the created project
		self.project = project
		try:
			project.Open( "MGA=" + mganame )
		except:
			project.Close(0)
			raise

		terr1 = project.CreateTerritory( None, None, None)
		terr2 = project.CreateTerritory( None, None, None)
		
		stat = project.ProjectStatus
		assert stat & 1 == 1	# is open
		assert stat & 2 == 0	# is NOT read-only
		assert stat & 8 == 0	# is NOT in transaction

		trans1 = project.BeginTransaction( terr1)

		stat = project.ProjectStatus
		assert stat & 1 == 1	# is open
		assert stat & 2 == 0	# is NOT read-only
		assert stat & 8 == 8	# is in transaction

		old_name = project.Name
		new_name = self.scramble_name( old_name)

		# change the name
		project.Name = new_name
		
		terrs = project.Territories
		assert terrs.Count == 2
		assert terrs[0] in ( terr1, terr2 )
		assert terrs[1] in ( terr1, terr2 )

		# discard changes
		project.AbortTransaction()

		stat = project.ProjectStatus
		assert stat & 1 == 1	# is open
		assert stat & 2 == 0	# is NOT read-only
		assert stat & 4 == 0	# project DID NOT change
		assert stat & 8 == 0	# is NOT in transaction

		# open second TRANSACTION_READ_ONLY = 1
		trans2 = project.BeginTransaction( terr2, 1 )

		stat = project.ProjectStatus
		assert stat & 1 == 1	# is open
		assert stat & 2 == 0	# is NOT read-only
		assert stat & 8 == 8	# is in transaction
		assert stat & 16== 16	# is in READ-ONLY transaction

		# name must be equal with the original string since the prev transaction has been aborted
		assert project.Name == old_name

		project.CommitTransaction()

		stat = project.ProjectStatus
		assert stat & 1 == 1	# is open
		assert stat & 2 == 0	# is NOT read-only
		assert stat & 8 == 0	# is NOT in transaction

		project.Save()
		project.Close(0)

		stat = project.ProjectStatus
		assert stat & 1 == 0	# is NOT open



	def populate4D(self, p):
		#it could have been: folder1 = p.RootFolder.CreateFolder( bd.folder( p, 'Folder'))
		# but use the Builder method newInFold
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'
		
		#folder2 = p.RootFolder.CreateFolder( bd.folder( p, 'Folder'))
		folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder2.Name = 'FolderB'

		#it could have been: comp1 = folder1.CreateRootObject( bd.kind( p, 'Compound'))
		# but :
		comp1   = bd.newObjInFold( p, folder1, 'Compound')
		comp1.Name = 'CompoundA'
		
		#it could have been: comp2 = folder2.CreateRootObject( bd.kind( p, 'Compound'))
		# but :
		comp2   = bd.newObjInFold( p, folder2, 'Compound')
		comp2.Name = 'CompoundB'
		

	def testD( self ):
		"""    testD

		Open				a project
		BeginTransaction		1st
		RootFolder			accessed
		Name				changed
		CommitTransaction
		BeginTransaction		2nd
		RootFolder			accessed
		Children 			deleted
		CommitTransaction
		UndoRedoSize
		Undo
		BeginTransaction		3rd
		UndoRedoSize
		Redo
		BeginTransaction		4th
		UndoRedoSize
		FlushUndoQueue
		Save				project
		Close				project
		
		"""

		mganame = _adjacent_file("_tc4_D_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4D( project)
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

		terr = project.CreateTerritory( None, None, None)
		
		# 1st transaction: change rootfolder's name
		trans = project.BeginTransaction( terr)
		rf = project.RootFolder

		old_name = rf.Name
		new_name = self.scramble_name( old_name )
		
		rf.Name = new_name	# name changed

		project.CommitTransaction()

		# 2nd transaction: delete all objects
		tran2 = project.BeginTransaction( terr)
		rf = project.RootFolder

		ls_flds = rf.ChildFolders
		ls_chls = rf.ChildFCOs

		number_of_deleted_objects = ls_flds.Count + ls_chls.Count

		for fld_i in ls_flds:
			fld_i.DestroyObject()
		for chl_i in ls_chls:
			chl_i.DestroyObject()

		project.CommitTransaction()
		
		(u, r) = project.UndoRedoSize()
		assert u == 2
		self.assertEqual(r, 0)

		project.Undo()

		# 3rd transaction: check the number of objects after Undo
		tran3 = project.BeginTransaction( terr)
		rf = project.RootFolder
		ls_flds = rf.ChildFolders
		ls_chls = rf.ChildFCOs
		project.CommitTransaction()

		number_of_restored_objects = ls_flds.Count + ls_chls.Count
		
		assert number_of_restored_objects == number_of_deleted_objects

		(u, r) = project.UndoRedoSize()
		assert u == 1
		self.assertEqual(r, 0)

		project.Redo()

		# 4th transaction: check the number of objects after Redo
		tran4 = project.BeginTransaction( terr)
		rf = project.RootFolder
		ls_flds = rf.ChildFolders
		ls_chls = rf.ChildFCOs
		project.CommitTransaction()

		number_of_objects = ls_flds.Count + ls_chls.Count
		
		assert 0 == number_of_objects

		(u, r) = project.UndoRedoSize()
		assert u == 2
		self.assertEqual(r, 0)

		project.FlushUndoQueue()

		(u, r) = project.UndoRedoSize()
		assert u == 0
		self.assertEqual(r, 0)


		project.Save()
		project.Close(0)


	def register_old_sf_par(self):
		curdir = os.path.abspath(os.path.curdir)
		#curdir = ''
		par_conn_str = "MGA=" + curdir + "used_paradigms\\SF\\SF_older.mta"
		par_con2_str = "XML=" + curdir + "used_paradigms\\SF\\SF_older.xmp"
		ori_conn_str = "MGA=" + curdir + "used_paradigms\\SF\\SF_orig.mta"
		
		registrar = DispatchEx("Mga.MgaRegistrar")
		
		# 1: user registry 
		# 2: system registry
		register_in = 1
		newname = registrar.RegisterParadigmFromData( par_conn_str, None, register_in)
		
		# maybe we have to register the original sf.mta file once again
		newname = registrar.RegisterParadigmFromData( ori_conn_str, None, register_in)
		
		# {5D6FF99D-D6C2-46A5-9208-CCA15D5FA16C} guid of the SF_older.xmp
		# 
		char_guid = ['0x9d', '0xf9', '0x6f', '0x5d', '0xc2', '0xd6', '0xa5', '0x46', '0x92', '0x08', '0xcc', '0xa1', '0x5d', '0x5f', '0xa1', '0x6c']
		bin_guid =  [0x9d, 0xf9, 0x6f, 0x5d, 0xc2, 0xd6, 0xa5, 0x46, 0x92, 0x08, 0xcc, 0xa1, 0x5d, 0x5f, 0xa1, 0x6c]
		
		#return char_guid
		return bin_guid
		
		
	def populate4E1(self, p):
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'
		
		folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder2.Name = 'FolderB'

		comp1   = bd.newObjInFold( p, folder1, 'Compound')
		comp1.Name = 'CompoundA'
		
		comp2   = bd.newObjInFold( p, folder2, 'Compound')
		comp2.Name = 'CompoundB'
		
	def populate4E3(self, p):
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'
		
		folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder2.Name = 'FolderB'

		comp1   = bd.newObjInFold( p, folder1, 'Compound')
		comp1.Name = 'CompoundA'
		
		comp2   = bd.newObjInFold( p, folder2, 'Compound')
		comp2.Name = 'CompoundB'
		
	def populate4E4(self, p):
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'
		
		folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder2.Name = 'FolderB'

		comp1   = bd.newObjInFold( p, folder1, 'Compound')
		comp1.Name = 'CompoundA'
		
		comp2   = bd.newObjInFold( p, folder2, 'Compound')
		comp2.Name = 'CompoundB'
		
	def populate4E5(self, p):
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'
		
		folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder2.Name = 'FolderB'

		comp1   = bd.newObjInFold( p, folder1, 'Compound')
		comp1.Name = 'CompoundA'
		
		comp2   = bd.newObjInFold( p, folder2, 'Compound')
		comp2.Name = 'CompoundB'

	def populate4E6(self, p):
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'
		
		folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder2.Name = 'FolderB'

		comp1   = bd.newObjInFold( p, folder1, 'Compound')
		comp1.Name = 'CompoundA'
		
		comp2   = bd.newObjInFold( p, folder2, 'Compound')
		comp2.Name = 'CompoundB'

	@dec_disable_early_binding
	def testE1( self ):
		"""    testE1

		Open				for a readonly project
		"""

		mganame = _adjacent_file("_tc4_E1_readonly_sf.mga")

		# if file exists from previous testing remove its readonly flag and delete the file
		if os.path.isfile( mganame):
			# revert to read/write permission in order to delete it seamlessly
			os.chmod( mganame, stat.S_IWUSR)

			#delete:
			#os.remove( mganame)
		

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4E1( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# 1st test: open readonly
		# open the created project
		try:
			# make the file read-only
			os.chmod( mganame, stat.S_IRUSR)
			readonly = project.Open( "MGA=" + mganame )
			assert readonly
			
			project.Close(0)
		except:
			project.Close(0)
			raise

	@dec_disable_early_binding
	def testE2( self ):
		"""    testE2

		CreateEx			with an old, registered version of SF
		OpenEx				with an old, registered version of SF
		Meta				accessed to test existence of specific kind in the old paradigm
		Close( abort )

		"""
		mganame = _adjacent_file("_tc4_E2_sf.mga")

		self.project = project = DispatchEx("Mga.MgaProject")

		charform_guid = self.register_old_sf_par()
		
		try:
			project.CreateEx( "MGA=" + mganame, "SF", charform_guid)
		except:
			project.Close(0)
			raise
		
		project.BeginTransaction( project.CreateTerritory( None, None, None))
		
		try:
			metafco = bd.kind( project, 'Basic')
			if not metafco:
				print 'Basic kind not found'
				self.fail('Basic kind not found')
		except:
			print 'Kind not found'
			raise
		
		
		fol1 = bd.newFolder( p, p.RootFolder, 'Folder')
		fol1.Name = 'JustCreated'
		
		bas1 = bd.newObjInFold( p, p.RootFolder, 'Basic')
		bas1.Name = 'Basic1'
		
		project.CommitTransaction()
		project.Close(0)

		# 2nd test: OpenEx
		try:
			ro = project.OpenEx( "MGA=" + mganame, "SF", charform_guid)
		except:
			print 'OpenEx failed'
			raise

		try:
			assert not ro	# not readonly
			
			project.BeginTransaction( project.CreateTerritory( None, None, None))
			
			f1 = bd.findInProj( project, 'JustCreated')
			b1 = bd.findInProj( project, 'Basic1')

			assert f1
			assert b1
			
			assert f1.ID	# valid pointer
			assert b1.ID	# valid pointer
			
			project.CommitTransaction()
			project.Close(0)
		except:
			project.AbortTransaction()
			project.Close(0)
			raise

		pass
		
	@dec_disable_early_binding
	def testE3( self ):
		"""    testE3
		
		SaveAs 		under a different name, with "Don't keep oldname" option
		"""
		mganame = _adjacent_file("_tc4_E3_sf.mga")
		mganame_SavedAs = _adjacent_file("_tc4_E3_savedAs_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4E3( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		project.Open( "MGA=" + mganame)

		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cA = bd.findInProj( project, 'CompoundA')
		
		pA = bd.new( project, cA, 'InputParameters')
		pA.Name = 'InpPar'
		
		project.CommitTransaction()

		
		# SAVE_AS operation
		# should create a file, and activate that project
		project.Save( "MGA=" + mganame_SavedAs)
		project.Close(0)
		
		# open the saved as project
		project.Open( "MGA=" + mganame_SavedAs)

		project.BeginTransaction( project.CreateTerritory( None, None, None))

		pA = bd.findInProj( project, 'InpPar')
		
		assert pA
		assert pA.ID
		
		project.CommitTransaction()
		project.Close(0)

	@dec_disable_early_binding
	def testE4( self ):
		"""    testE4
		
		SaveAs 		under a different name, with "Don't keep oldname" option
		Apply changes	after the SaveAs operation
		"""
		mganame = _adjacent_file("_tc4_E4_sf.mga")
		mganame_SavedAs = _adjacent_file("_tc4_E4_savedAs_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4E4( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		project.Open( "MGA=" + mganame)

		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cA = bd.findInProj( project, 'CompoundA')
		
		pA = bd.new( project, cA, 'InputParameters')
		pA.Name = 'InpPar'
		
		project.CommitTransaction()

		
		# SAVE_AS operation
		# should create a file, and activate that project
		project.Save( "MGA=" + mganame_SavedAs)

		#
		# ... and create some other kinds in the project, right after the saveAs operation
		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cB = bd.findInProj( project, 'CompoundB')
		
		sA = bd.new( project, cB, 'InputSignals')
		sA.Name = 'InpSig'
		
		project.CommitTransaction()
		project.Close(0)
		
		# 
		# open and test the saved as project
		project.Open( "MGA=" + mganame_SavedAs)

		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cA = bd.findInProj( project, 'CompoundA')
		cB = bd.findInProj( project, 'CompoundB')
		pA = bd.findInProj( project, 'InpPar')
		sA = bd.findInProj( project, 'InpSig')
		
		assert sA
		assert sA.ID
		assert pA
		assert pA.ID
		
		project.CommitTransaction()
		project.Close(0)

		# 
		# open and test the OLD project
		project.Open( "MGA=" + mganame)

		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cA = bd.findInProj( project, 'CompoundA')
		cB = bd.findInProj( project, 'CompoundB')
		assert cA.ID
		assert cB.ID
		
		# 
		# ... should not contain the kind created BEFORE the SaveAs
		try:
			pA = bd.findInProj( project, 'InpPar')
			self.fail("Should have failed")
		except:
			#good, it is expected to fail
			pass
			
		#
		# ... should not contain the kind created AFTER the SaveAs
		try:
			sA = bd.findInProj( project, 'InpSig')
			self.fail("Should have failed")
		except:
			#goood, it is expected to fail
			pass
		
		project.CommitTransaction()
		project.Close(0)

	@dec_disable_early_binding
	def testE5( self ):
		"""    testE5
		
		SaveAs 		under a different name, with "Keep oldname" option
		"""
		mganame = _adjacent_file("_tc4_E5_sf.mga")
		mganame_SavedAs = _adjacent_file("_tc4_E5_savedAs_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4E5( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		project.Open( "MGA=" + mganame)

		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cA = bd.findInProj( project, 'CompoundA')
		
		pA = bd.new( project, cA, 'InputParameters')
		pA.Name = 'InpPar'
		
		project.CommitTransaction()

		#
		# SAVE_AS operation and KEEP OLD NAME
		# should create a file, and remain with the project name above
		keep_old_name = True
		project.Save( "MGA=" + mganame_SavedAs, keep_old_name)
		project.Close(0)
		
		#
		# open and test the OLD project
		project.Open( "MGA=" + mganame)
		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cA = bd.findInProj( project, 'CompoundA')
		pA = bd.findInProj( project, 'InpPar')

		assert cA.ID		# valid pointer
		assert pA.ID		# valid pointer
		
		project.CommitTransaction()
		project.Close(0)

		#
		# open and test the SAVEDAS project
		project.Open( "MGA=" + mganame_SavedAs)
		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cA = bd.findInProj( project, 'CompoundA')
		pA = bd.findInProj( project, 'InpPar')

		assert cA.ID		# valid pointer
		assert pA.ID		# valid pointer
		
		project.CommitTransaction()
		project.Close(0)

	@dec_disable_early_binding
	def testE6( self ):
		"""    testE6
		
		SaveAs 		under a different name, with "Keep oldname" option
		Apply changes	after the SaveAs operation
		"""
		mganame = _adjacent_file("_tc4_E6_sf.mga")
		mganame_SavedAs = _adjacent_file("_tc4_E6_savedAs_sf.mga")

		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4E6( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		project.Open( "MGA=" + mganame)

		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cA = bd.findInProj( project, 'CompoundA')
		
		pA = bd.new( project, cA, 'InputParameters')
		pA.Name = 'InpPar'
		
		project.CommitTransaction()

		#
		# SAVE_AS operation and KEEP OLD NAME
		# should create a file, and remain with the project name above
		keep_old_name = True
		project.Save( "MGA=" + mganame_SavedAs, keep_old_name)

		#
		# make some changes right after the SAVEAS operation !!!
		#
		project.BeginTransaction( project.CreateTerritory( None, None, None))

		cB = bd.findInProj( project, 'CompoundB')
		assert cB.ID
		
		pB = bd.new( project, cB, 'OutputParameters')
		pB.Name = 'OutPar'

		project.CommitTransaction()
		project.Close(0)

		#
		# open and test the OLD project
		project.Open( "MGA=" + mganame)
		project.BeginTransaction( project.CreateTerritory( None, None, None))

		#
		# should contain all kinds created
		#
		cA = bd.findInProj( project, 'CompoundA')
		pA = bd.findInProj( project, 'InpPar')
		pB = bd.findInProj( project, 'OutPar')

		assert cA.ID		# valid pointer
		assert pA.ID		# valid pointer
		assert pB.ID		# valid pointer
		
		project.CommitTransaction()
		project.Close(0)

		#
		# open and test the SAVEDAS project
		project.Open( "MGA=" + mganame_SavedAs)
		project.BeginTransaction( project.CreateTerritory( None, None, None))

		#
		# should contain kinds created BEFORE the SAVEDAS
		#
		cA = bd.findInProj( project, 'CompoundA')
		pA = bd.findInProj( project, 'InpPar')

		assert cA.ID		# valid pointer
		assert pA.ID		# valid pointer
		
		#
		# should NOT contain kinds created AFTER the SAVEDAS
		#
		try:
			pB = bd.findInProj( project, 'OutPar')
			self.fail("Should have failed")
		except:
			#goood, it is expected to throw
			pass

		project.CommitTransaction()
		project.Close(0)


	def populate4F(self, p):
		# will get relid 1
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'
		
		# will get relid 2
		folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder2.Name = 'FolderA'
		
		folder2.DestroyObject()

		# will get relid 3
		folder3 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder3.Name = 'FolderA'

		compa1 = bd.newObjInFold( p, folder1, 'Compound')
		compa1.Name = 'CompoundA'
		
		compa2 = bd.newObjInFold( p, folder3, 'Compound')
		compa2.Name = 'CompoundA'

		compb  = bd.newObjInFold( p, folder3, 'Compound')
		compb.Name = 'CompoundC'

	def testF( self ):

		"""    testF

		ObjectByPath		invoked with #relids
					invoked with @name|kind strings
							-for unique object
							-for non-unique object
							-for non-existing object
					invoked with name|kind|relpos string

		NthObjectByPath		invoked with name|kind|relpos string
							-for unique object
							-for non-unique object
							-for non-existing object

		"""
		
		mganame = _adjacent_file("_tc4_F_sf.mga")
		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4F( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		ro = project.Open( "MGA=" + mganame)
		assert not ro
		
		tran = project.BeginTransaction( project.CreateTerritory( None, None, None))
		
		# search based on relid path
		fco = project.ObjectByPath( "#1/#1" )
		assert fco.Name == "CompoundA"
		fco = project.ObjectByPath( "#3/#1" )
		assert fco.Name == "CompoundA"
		fco = project.ObjectByPath( "#3/#2" )
		assert fco.Name == "CompoundC"
		
		fol = project.ObjectByPath( "#3" )
		assert fol.Name == "FolderA"
		
		# find a unique object
		fco = project.ObjectByPath( "@FolderA|kind=Folder/@CompoundC|kind=Compound")
		assert fco.Name == "CompoundC"

		# ... find the same object with ignored relpos indicators
		fco = project.ObjectByPath( "@FolderA|kind=Folder|relpos=0/@CompoundC|kind=Compound|relpos=1")
		assert fco.Name == "CompoundC"
		
		# ... find the same (unique) object with misleading relpos indicators and NthObjectByPath
		fco = project.NthObjectByPath( 0, "@FolderA|kind=Folder|relpos=11/@CompoundC|kind=Compound|relpos=22")
		assert fco.Name == "CompoundC"
		
		# try a non-unique object search
		try:
			fco = project.ObjectByPath( "@FolderA|kind=Folder/@CompoundA|kind=Compound")
			# fco is (must be) 0, so let's provoke an error
			s_dummy = fco.Name
			# the prev line should have raised an exception
			assert 0
		except AttributeError:
			# Goood, since the fco is null
			assert 1
			
			# now try the NthObjectByPath for this
			fco = project.NthObjectByPath( 0, "@FolderA|kind=Folder|relpos=1/@CompoundA|kind=Compound|relpos=1")
			assert fco.Name == "CompoundA"
		except:
			# other error
			assert 0
		
		# try a no result search with ObjectByPath
		try:
			fco = project.ObjectByPath( "@FolderA|kind=Folder|relpos=1/@CompoundX|kind=Compound|relpos=1")
			s_dummy = fco.Name
 			# the prev line should have raised an exception
 			assert 0
		except AttributeError:
			# Goood, since the fco must be null
			assert 1
		except:
			# other error
			assert 0
		
		# ... and with NthObjectByPath
		try:
			fco = project.NthObjectByPath( 0, "@FolderA|kind=Folder|relpos=1/@CompoundX|kind=Compound|relpos=1")
			s_dummy = fco.Name
			# the prev line should have raised an exception
			assert 0
		except AttributeError:
			# Goood, since the fco must be null
			assert 1
		except:
			# other error
			assert 0

		project.CommitTransaction()
		project.Close(0)
		pass
		
	def populate4G(self, p):
		p.RootFolder.Name = '_tc4_G_sf'

		# will get id 'id-006a-00000002'
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'
		
		# will get id 'id-006a-00000003'
		folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder2.Name = 'FolderB'
		
		# will get id 'id-0065-00000001'
		comp1 = bd.newObjInFold( p, folder1, 'Compound')
		comp1.Name = 'Compound1'
		
		# will get id 'id-0065-00000002'
		prim1 = bd.newObjInFold( p, folder2, 'Primitive')
		prim1.Name = 'Primitive1'
		
		# will get id 'id-0065-00000003'
		comppA  = bd.new( p, comp1, 'CompoundParts')
		comppA.Name = 'CompoundPartsA'
		
		# will get id 'id-0065-00000004'
		primpA  = bd.new( p, comp1, 'PrimitiveParts')
		primpA.Name = 'PrimitivePartsA'
		
		# will get id 'id-0066-00000001'
		ip = bd.new( p, comppA, 'InputParameters')
		ip.Name = 'InputParameterP'
		
		# will get id 'id-0066-00000002'
		os = bd.new( p, comppA, 'OutputSignals')
		os.Name = 'OutputSignalQ'
	
	@dec_disable_early_binding
	def testG( self ):

		"""    testG

		GetObjectByID

		"""

		mganame = _adjacent_file("_tc4_G_sf.mga")
		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4G( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		ro = project.Open( "MGA=" + mganame )
		assert not ro
		
		tran = project.BeginTransaction( project.CreateTerritory( None, None, None))
		
		# search based on id
		to_find = "id-006a-00000001"
		obj = project.GetObjectByID( to_find )
		assert obj.ID   == to_find
		assert obj.Name == "_tc4_G_sf"		# rootfolder
		assert obj.RelID == 1
		
		to_find = "id-006a-00000002"
		obj = project.GetObjectByID( to_find )
		assert obj.ID   == to_find
		assert obj.Name == "FolderA"
		assert obj.RelID == 1

		to_find = "id-006a-00000003"
		obj = project.GetObjectByID( to_find )
		assert obj.ID   == to_find
		assert obj.Name == "FolderB"
		assert obj.RelID == 2

		to_find = "id-0065-00000001"
		obj = project.GetObjectByID( to_find )
		assert obj.ID   == to_find
		assert obj.Name == "Compound1"
		assert obj.RelID == 1
		
		to_find = "id-0065-00000002"
		obj = project.GetObjectByID( to_find )
		assert obj.ID   == to_find
		assert obj.Name == "Primitive1"
		assert obj.RelID == 1
		
		to_find = "id-0065-00000003"
		obj = project.GetObjectByID( to_find )
		assert obj.ID   == to_find
		assert obj.Name == "CompoundPartsA"
		assert obj.RelID == 1

		to_find = "id-0065-00000004"
		obj = project.GetObjectByID( to_find )
		assert obj.ID   == to_find
		assert obj.Name == "PrimitivePartsA"
		assert obj.RelID == 2
		
		to_find = "id-0066-00000001"
		obj = project.GetObjectByID( to_find )
		assert obj.ID   == to_find
		assert obj.Name == "InputParameterP"
		assert obj.RelID == 1

		to_find = "id-0066-00000002"
		obj = project.GetObjectByID( to_find )
		assert obj.ID   == to_find
		assert obj.Name == "OutputSignalQ"
		assert obj.RelID == 2

		project.CommitTransaction()
		project.Close(0)
		pass

	def populate4H(self, p):
		p.RootFolder.Name = '_tc4_H_sf'

		# will get id 'id-006a-00000002'
		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'
		
		# will get id 'id-006a-00000003'
		folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder2.Name = 'FolderA'
		
		# will get id 'id-0065-00000001'
		comp1 = bd.newObjInFold( p, folder1, 'Compound')
		comp1.Name = 'Compound1'
		
		# will get id 'id-0065-00000002'
		prim1 = bd.newObjInFold( p, folder2, 'Primitive')
		prim1.Name = 'Primitive1'
		
		# will get id 'id-0065-00000003'
		comppA  = bd.new( p, comp1, 'CompoundParts')
		comppA.Name = 'CompoundPartsA'
		
		# will get id 'id-0065-00000004'
		primpA  = bd.new( p, comp1, 'PrimitiveParts')
		primpA.Name = 'PrimitivePartsA'
		
		# will get id 'id-0066-00000001'
		ip = bd.new( p, comppA, 'InputParameters')
		ip.Name = 'InputParameterP'
		
		# will get id 'id-0066-00000002'
		os = bd.new( p, comppA, 'OutputSignals')
		os.Name = 'OutputSignalQ'
	
	@dec_disable_early_binding
	def testH( self ):

		"""    testH

		GetFCOByID	for folder (expecting 0)
				for fcos

		"""

		mganame = _adjacent_file("_tc4_H_sf.mga")
		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4H( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		ro = project.Open( "MGA=" + mganame )
		assert not ro
		
		tran = project.BeginTransaction( project.CreateTerritory( None, None, None))
		
		# search based on id
		# for a folder
		to_find = "id-006a-00000002"
		try:
			fco = project.GetFCOByID( to_find )
			assert fco.ID   == to_find
			assert fco.Name == "FolderA"
			assert fco.RelID == 1
			# this is wrong, fco should be 0, exception should have thrown
			assert 0
		except AttributeError:
			# goood, the GetFCOByID should not return a valid folder
			assert 1
		except:
			assert 1
		
		# for FCOs
		to_find = "id-0065-00000001"
		fco = project.GetFCOByID( to_find )
		assert fco.ID   == to_find
		assert fco.Name == "Compound1"
		assert fco.RelID == 1
		
		to_find = "id-0065-00000002"
		fco = project.GetFCOByID( to_find )
		assert fco.ID   == to_find
		assert fco.Name == "Primitive1"
		assert fco.RelID == 1
		
		to_find = "id-0065-00000003"
		fco = project.GetFCOByID( to_find )
		assert fco.ID   == to_find
		assert fco.Name == "CompoundPartsA"
		assert fco.RelID == 1

		to_find = "id-0065-00000004"
		fco = project.GetFCOByID( to_find )
		assert fco.ID   == to_find
		assert fco.Name == "PrimitivePartsA"
		assert fco.RelID == 2
		
		to_find = "id-0066-00000001"
		fco = project.GetFCOByID( to_find )
		assert fco.ID   == to_find
		assert fco.Name == "InputParameterP"
		assert fco.RelID == 1

		to_find = "id-0066-00000002"
		fco = project.GetFCOByID( to_find )
		assert fco.ID   == to_find
		assert fco.Name == "OutputSignalQ"
		assert fco.RelID == 2

		project.CommitTransaction()
		project.Close(0)
		pass

	def populate4I(self, p):
		pass
	def testI( self ):

		"""   testI

		BeginTransaction
		GetFCOsByName		TODO: implement the feature first, then the testcase
		GetFolderByPath		TODO: implement the feature first, then the testcase
		AbortTransaction

		"""

		mganame = _adjacent_file("_tc4_I_sf.mga")
		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4I( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		ro = project.Open( "MGA=" + mganame )
		assert not ro
		
		tran = project.BeginTransaction( project.CreateTerritory( None, None, None))
		
		# GetFCOsByName
		to_find = "Anything"
		try:
			fco = project.GetFCOsByName( to_find )
			# this is wrong, fco should be 0, exception should have thrown
			assert 0
		except:
			# goood, since the GetFCOsByName is not implemented yet (develop the test case if available)
			assert 1

		# GetFolderByPath
		to_find = "Anything"
		try:
			fol = project.GetFolderByPath( to_find )
			# this is wrong, fol should be 0, exception should have thrown
			assert 0
		except:
			# goood, since the GetFolderByPath is not implemented yet (develop the test case if available)
			assert 1

		project.AbortTransaction()
		project.Close(0)
		pass
		
	def populate4J(self, p):
		compM1 = bd.newObjInFold( p, p.RootFolder, 'Compound')
		compM1.Name = 'CompoundM'
		compM2 = bd.newObjInFold( p, p.RootFolder, 'Compound')
		compM2.Name = 'CompoundM'

		folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
		folder1.Name = 'FolderA'

		compA1 = bd.newObjInFold( p, folder1, 'Compound')
		compA1.Name = 'CompoundA'
		
		compA2 = bd.newObjInFold( p, folder1, 'Compound')
		compA2.Name = 'CompoundA'
		
		pp1  = bd.new( p, compA1, 'CompoundParts')
		pp1.Name = 'NewCompoundParts'
		
		pp2  = bd.new( p, compA1, 'CompoundParts')
		pp2.Name = 'NewCompoundParts'
		
		
		ip1 = bd.new( p, compA2, 'InputParameters')
		ip1.Name = 'NewInputParameters'
		ip2 = bd.new( p, compA2, 'InputParameters')
		ip2.Name = 'NewInputParameters'
		

	@dec_disable_early_binding
	def testJ( self ):
		"""   testJ

		BeginTransaction
		CreateFilter			
		AllFCOs				inquire all objects with an empty filter
		change filter
		AllFCOs				inquire some objects with a constrained filter
		CommitTransaction

		"""

		mganame = _adjacent_file("_tc4_J_sf.mga")
		# create the project with the needed kinds
		project = bd.creaP( mganame, "SF")
		if not project:
			self.fail("Create failed")
		try:
			self.populate4J( project)
		except:
			bd.saveP( project)
			raise
		bd.saveP( project)

		self.project = project

		# open the created project
		ro = project.Open( "MGA=" + mganame )
		assert not ro
		
		tran = project.BeginTransaction( project.CreateTerritory( None, None, None))

		# create a filter
		filter = project.CreateFilter()
		
	 	# use an empty filter
	 	all_fcos = project.AllFCOs( filter )

	 	all_names = []
	 	for f in all_fcos: all_names.append( f.Name )

	 	res = [ "CompoundA", "CompoundA", "CompoundM", "CompoundM", \
	 	"NewCompoundParts", "NewCompoundParts", "NewInputParameters", "NewInputParameters" ]

	 	all_names.sort()
	 	res.sort()

	 	assert 2 == all_names.count( "CompoundA" )
	 	assert 2 == all_names.count( "CompoundM" )
	 	assert 2 == all_names.count( "NewCompoundParts" )
	 	assert 2 == all_names.count( "NewInputParameters" )
	 	assert 8 == all_fcos.Count
	 	assert res == all_names		# sorted lists


		# use Name filter
		filter.Name = "CompoundM"
		some_fcos = project.AllFCOs( filter )

		namelist = []
		for f in some_fcos: namelist.append( f.Name )

		assert 2 == namelist.count( "CompoundM" )
		assert 2 == some_fcos.Count
		
		# use Kind filter #1
		filter.Name = ""		# reset Name filter value
		filter.Kind = "InputParam"

		some_fcos = project.AllFCOs( filter )
		namelist = []
		for f in some_fcos: namelist.append( f.Name )

		assert 2 == namelist.count( "NewInputParameters" )
		assert 2 == some_fcos.Count

		# use Kind filter #2
		filter.Kind = "Compound"

		some_fcos = project.AllFCOs( filter )

		namelist = []
		for f in some_fcos: namelist.append( f.Name )

		assert 2 == namelist.count( "NewCompoundParts" )
		assert 2 == namelist.count( "CompoundA" )
		assert 2 == namelist.count( "CompoundM" )
		assert 6 == some_fcos.Count

		# use Role filter
		filter.Kind = ""		# reset Kind filter value
		filter.Role = "CompoundParts"

		some_fcos = project.AllFCOs( filter )

		namelist = []
		for f in some_fcos: namelist.append( f.Name )

		assert 2 == namelist.count( "NewCompoundParts" )
		assert 2 == some_fcos.Count

		# use ObjType filter
		filter.Role = ""		# reset Role filter value
		filter.ObjType = "OBJTYPE_ATOM"

		some_fcos = project.AllFCOs( filter )

		namelist = []
		for f in some_fcos: namelist.append( f.Name )

		assert 2 == namelist.count( "NewInputParameters" )
		assert 2 == some_fcos.Count


		# use Level filter #1
		filter.ObjType = ""		# reset ObjType filter value
		filter.Level = 0

		some_fcos = project.AllFCOs( filter )

		namelist = []
		for f in some_fcos: namelist.append( f.Name )

		assert 2 == namelist.count( "CompoundA" )
		assert 2 == namelist.count( "CompoundM" )
		assert 4 == some_fcos.Count

		# use Level filter #2
		filter.Level = 1

		some_fcos = project.AllFCOs( filter )

		namelist = []
		for f in some_fcos: namelist.append( f.Name )

		assert 2 == namelist.count( "NewCompoundParts" )
		assert 2 == namelist.count( "NewInputParameters" )
		assert 4 == some_fcos.Count

		# test Project property of the filter
		assert f.Project == project
		
		project.CommitTransaction()
		project.Close(0)
		pass


"""
	Let's produce the test suites
"""
def suites():
	suite = unittest.makeSuite(TestCase4,'test')
	return suite

def suites3():
	suite = unittest.TestSuite()
	suite.addTest( TestCase4( "testA"))
	suite.addTest( TestCase4( "testB"))
	suite.addTest( TestCase4( "testC"))
	suite.addTest( TestCase4( "testD"))
	suite.addTest( TestCase4( "testE"))
	suite.addTest( TestCase4( "testF"))
	suite.addTest( TestCase4( "testG"))
	suite.addTest( TestCase4( "testH"))
	suite.addTest( TestCase4( "testI"))
	suite.addTest( TestCase4( "testJ"))
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
	# unittest.main(defaultTest='TestCase4')
	
	######## 3rd way
	# all = suites3()
	# runner = unittest.TextTestRunner()
	# runner.run( all )
