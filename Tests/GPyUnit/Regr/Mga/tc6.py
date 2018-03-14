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

class TestCase6( unittest.TestCase ):
    def __init__(self, methodName='runTest'):
        unittest.TestCase.__init__(self, methodName)
        self.project = None

    def setUp( self ):        ## hook method
        pass

    def tearDown( self ):        ## hook method
        if self.project:
            self.project.Close( 0 )

    def populate6A(self, p):
        f1 = bd.newFolder( p, p.RootFolder, 'Folder')
        f1.Name = 'NewFolder'
        
        c1 = bd.newObjInFold( p, p.RootFolder, 'Compound')
        c1.Name = 'NewCompound'
        
        p1 = bd.newObjInFold( p, f1, 'Primitive')
        p1.Name = 'NewPrimitive'
        
    @dec_disable_early_binding
    def testA( self ):

        """    testA
            test properties of IMgaObject such as
                Name
                ID
                AbsPath
                GetParent
                ObjType
                MetaBase
                Project
                Territory

            methods such as
                ObjectByPath
                IsEqual
        """

        mganame = _adjacent_file("_tc6_A_sf.mga")
        project = bd.creaP( mganame, "SF")
        if not project:
            self.fail("Create failed")
        try:
            self.populate6A( project)
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

        rf = project.RootFolder

        fol = rf.ObjectByPath( "@NewFolder|kind=Folder")
        assert fol.Name == "NewFolder"
        assert fol.AbsPath == "/@NewFolder|kind=Folder|relpos=0"

        fco = rf.ObjectByPath( "@NewFolder|kind=Folder/@NewPrimitive|kind=Primitive")
        assert fco.Name == "NewPrimitive"
        assert fco.AbsPath == "/@NewFolder|kind=Folder|relpos=0/@NewPrimitive|kind=Primitive|relpos=0"
        assert fco.GetParent() == ( fol, 6 )        # a tuple with parent and its type: OBJTYPE_FOLDER = 6
        
        # use ObjectByPath with NewFolder as root
        fcp = fol.ObjectByPath( "@NewPrimitive|kind=Primitive")
        assert fcp.Name == "NewPrimitive"
        assert fcp.Name == fco.Name
        assert fcp.AbsPath == fco.AbsPath
        
        # the object pointer should be equal (they are in the same territory)
        assert fco == fcp
        # this must be True regardless of territories
        assert fco.IsEqual( fcp)

        fc2 = rf.ObjectByPath( "@NewCompound|kind=Compound")
        assert fc2.Name == "NewCompound"
        assert fc2.AbsPath == "/@NewCompound|kind=Compound|relpos=0"
        assert fc2.GetParent() == ( rf, 6)        # a tuple with parent and its type: OBJTYPE_FOLDER = 6
        
        fco.Name = "OldPrimitive"
        fc2.Name = "OldCompound"
        fol.Name = "OldFolder"
        
        t_fol = rf.ObjectByPath( "@OldFolder|kind=Folder")
        assert t_fol.Name == "OldFolder"

        t_fco = rf.ObjectByPath( "@OldFolder|kind=Folder/@OldPrimitive|kind=Primitive")
        assert t_fco.Name == "OldPrimitive"

        t_fc2 = rf.ObjectByPath( "@OldCompound|kind=Compound")
        assert t_fc2.Name == "OldCompound"
        
        # test IsEqual for "should be false" cases
        assert ~t_fc2.IsEqual( t_fco)
        assert ~t_fc2.IsEqual( t_fol)

        assert t_fol.IsEqual( fol)
        assert t_fol.ID == fol.ID
        assert t_fol.MetaBase == fol.MetaBase
        assert t_fol.MetaBase.Name == "Folder"
        assert t_fol.ObjType == fol.ObjType
        assert t_fol.ObjType == 6            # OBJTYPE_FOLDER
        
        assert t_fco.IsEqual( fco)            
        assert t_fco.ID == fco.ID
        assert t_fco.MetaBase == fco.MetaBase
        assert t_fco.MetaBase.Name == "Primitive"
        assert t_fco.ObjType == fco.ObjType
        assert t_fco.ObjType == 1            # OBJTYPE_MODEL
        
        assert t_fc2.IsEqual( fc2)
        assert t_fc2.ID == fc2.ID
        assert t_fc2.MetaBase == fc2.MetaBase
        assert t_fc2.MetaBase.Name == "Compound"
        assert t_fc2.ObjType == fc2.ObjType
        assert t_fc2.ObjType == 1            # OBJTYPE_MODEL

        fco.Name = "NewPrimitive"
        fc2.Name = "NewCompound"
        fol.Name = "NewFolder"
        
        assert fco.Project == project
        assert fco.Territory == terr

        assert fc2.Project == project
        assert fc2.Territory == terr

        assert fol.Project == project
        assert fol.Territory == terr

        # commit trans
        project.CommitTransaction()

        # create another territory
        ter2 = project.CreateTerritory( None, None, None)
        tran2 = project.BeginTransaction( ter2)

        a_rf = project.RootFolder
        # the objects must be equal in spite of different territories
        assert a_rf.IsEqual( rf)

        a_fco = a_rf.ObjectByPath( "@NewFolder|kind=Folder/@NewPrimitive|kind=Primitive")
        assert a_fco.Name == "NewPrimitive"
        assert a_fco != fco
        assert a_fco.IsEqual( fco)
        
        # commit tran2
        project.CommitTransaction()
        

        project.Save()
        project.Close(0)


        pass

    def populate6C(self, p):
        c1 = bd.newObjInFold( p, p.RootFolder, 'Compound')
        c1.Name = 'NewCompound'
        
        p1 = bd.new( p, c1, 'PrimitiveParts')
        p1.Name = 'NewPrimitiveParts'
        
    @dec_disable_early_binding
    def testC( self ):
        """    testC

            for a READ-ONLY project test methods like:
            IsLibObject
            IsWritable
            Status
            
            then
            
            create another project
            AttachLibrary attach the prev project as a library
            IsLibObject, IsWritable check on its objects
        """

        mg1name = _adjacent_file("_tc6_C_readonly_sf.mga")

        # if file exists from previous testing remove its readonly flag and delete the file
        for file in (mg1name, mg1name + "tmp"):
            if os.path.isfile(file):
                # revert to read/write permission in order to delete it seamlessly
                os.chmod(file, stat.S_IWUSR)
                #delete:
                #os.remove( mg1name)
        

        # create the project with the needed kinds
        project1 = bd.creaP( mg1name, "SF")
        if not project1:
            self.fail("Create failed")
        try:
            self.populate6C( project1)
        except:
            bd.saveP( project1)
            raise
        bd.saveP( project1)

        self.project = project1

        # make the file read-only and 
        # ... open the created project
        try:
            # make the file read-only
            os.chmod( mg1name, stat.S_IRUSR)

            readonly = project1.Open( "MGA=" + mg1name )
            # assert readonly
        except:
            project1.Close(0)
            raise

        self.project = project1

        terr1 = project1.CreateTerritory( None, None, None)
        # begin transaction as TRANSACTION_READ_ONLY = 1
        trans1 = project1.BeginTransaction( terr1, 1)
        rf = project1.RootFolder
        
        obj = rf.ObjectByPath( "#1")
        assert obj.IsWritable                   # though the project is ReadOnly, the object itself is writable
        assert not obj.IsLibObject              # not a lib object
        assert obj.Status == 0            # OBJECT_EXISTS defd in Mga.idl
        
        # commit trans
        project1.CommitTransaction()
        
        # begin a regular transaction
        trans2 = project1.BeginTransaction( terr1)

        # and create an object
        cmp1 = bd.newObjInFold( project1, rf, 'Compound')
        cmp1.Name = "ThisObjectWillNotSurvive"
        
        # commit trans
        project1.CommitTransaction()
        
        # now try to save it
        try:
            project1.Save() 
            assert False                    # it should have failed
        except:
            pass                            # it is OK to fail, since a Read-only file cannot not be saved modified

        project1.Close(True)
        
        mg2name = _adjacent_file("_tc6_C_libuser_sf.mga")

        # create another project
        project2 = bd.creaP( mg2name, "SF")
        if not project2:
            self.fail("Create failed for " + mg2name + " project")

        bd.saveP( project2)

        # open the created project
        self.project = project2
        try:
            project2.Open( "MGA=" + mg2name )
        except:
            project2.Close(0)
            raise


        terr2 = project2.CreateTerritory( None, None, None)
        trans3 = project2.BeginTransaction( terr2)
        rf2 = project2.RootFolder

        # attach mg1name to it
        try:
            # attach the read-only file as a library
            libroot = project2.RootFolder.AttachLibrary( "MGA=" + mg1name)
            pass
        except:
            print 'AttachLibrary failed'
            raise


        
        flds = project2.RootFolder.ChildFolders
        assert flds.Count == 1
        
        #the only subfolder of RootFolder is the library root
        obj = flds.Item(1).GetChildFCODisp( "NewCompound")
        assert obj.IsWritable                   # though the object is in a library it report itself as writable
        assert obj.IsLibObject                  # a lib object
        assert obj.Status == 0                  # OBJECT_EXISTS defd in Mga.idl
        
        # commit trans
        project2.CommitTransaction()
        
        project2.Save()
        project2.Close(0)

    def populate6D(self, p):
        f1 = bd.newFolder( p, p.RootFolder, 'Folder')
        f1.Name = 'NewFolder'
        
        f2 = bd.newFolder( p, p.RootFolder, 'Folder')
        f2.Name = 'NewFolder'

        c1 = bd.newObjInFold( p, p.RootFolder, 'Compound')
        c1.Name = 'NewCompound'
        
        p1 = bd.newObjInFold( p, f1, 'Primitive')
        p1.Name = 'NewPrimitive'
        
        ip = bd.new( p, p1, 'InputParameters')
        ip.Name = 'NewInputParameters'
        
        is0 = bd.new( p, p1, 'InputSignals')
        is0.Name = 'NewInputSignals'
        
        op = bd.new( p, p1, 'OutputParameters')
        op.Name = 'NewOutputParameters'
        
        os = bd.new( p, p1, 'OutputSignals')
        os.Name = 'NewOutputSignals'
        
        pp = bd.new( p, p1, 'Parameters')
        pp.Name = 'NewParameters'

        p2 = bd.newObjInFold( p, f1, 'Primitive')
        p2.Name = 'NewPrimitive'
        
        op = bd.new( p, p2, 'OutputParameters')
        op.Name = 'NewOutputParameters'
        
        p2 = bd.newObjInFold( p, f2, 'Primitive')
        p2.Name = 'NewPrimitive'

        op = bd.new( p, p2, 'OutputParameters')
        op.Name = 'NewOutputParameters'
        
        cp1 = bd.new( p, c1, 'CompoundParts')
        cp1.Name = 'NewCompoundParts'
        
        pp1 = bd.new( p, c1, 'PrimitiveParts')
        pp1.Name = 'NewPrimitiveParts'
        

    @dec_disable_early_binding
    def testD( self ):

        """    testD

            Check
            CheckTree
            ChildObjects
            ChildObjectByRelID
        """

        mganame = _adjacent_file("_tc6_D_sf.mga")
        
        # create the project with the needed kinds
        project = bd.creaP( mganame, "SF")
        if not project:
            self.fail("Create failed")
        try:
            self.populate6D( project)
        except:
            bd.saveP( project)
            raise
        bd.saveP( project)

        self.project = project

        # open the created project
        self.project = project
        try:
            project.Open( "MGA=" + mganame )
        except:
            project.Close(0)
            raise

        terr = project.CreateTerritory( None, None, None)
        trans = project.BeginTransaction( terr)

        rf = project.RootFolder
        
        #objs = list( rf.ChildObjects )
        objs = rf.ChildObjects
        names = [ "NewFolder", "NewCompound"]
        for obj in objs:
            obj.Check()
            obj.CheckTree()
            rid = obj.RelID
            one_obj = rf.ChildObjectByRelID( rid)
            assert one_obj
            assert one_obj.RelID == rid
            name_dummy = one_obj.Name             # it should not throw exc
            assert name_dummy in names
        
        one_fol = rf.ChildObjectByRelID( 1 )
        assert one_fol.Name == names[0]
        
        one_pri = one_fol.ChildObjectByRelID( 2 )
        assert one_pri.Name == "NewPrimitive"
        one_pri = one_fol.ChildObjectByRelID( 1 )
        assert one_pri.Name == "NewPrimitive"
        
        names = [ "NewInputParameters", "NewInputSignals", "NewOutputParameters", "NewOutputSignals", "NewParameters"]
        name2relid = { "NewInputParameters" : 1, 
                "NewInputSignals" : 2, 
                "NewOutputParameters" : 3, 
                "NewOutputSignals" : 4, 
                "NewParameters" : 5
        }
        
        pri_objs = one_pri.ChildObjects
        for pri_obj in pri_objs:
            rid = pri_obj.RelID
            one_obj = one_pri.ChildObjectByRelID( rid)
            assert one_obj
            assert one_obj.RelID == rid
            s_name = one_obj.Name
            assert s_name in names
            assert name2relid[ s_name ] == rid
        
        # commit trans
        project.CommitTransaction()
        
        project.Save()
        project.Close(0)


    @dec_disable_early_binding
    def testE( self ):

        """  testE

            ObjectByPath
            NthObjectByPath
        """

        mganame = _adjacent_file("_tc6_D_sf.mga")                      # uses the same as D
        
        # create the project with the needed kinds - the same ad testD
        project = bd.creaP( mganame, "SF")
        if not project:
            self.fail("Create failed")
        try:
            self.populate6D( project)
        except:
            bd.saveP( project)
            raise
        bd.saveP( project)

        self.project = project

        # open the created project
        self.project = project
        try:
            project.Open( "MGA=" + mganame )
        except:
            project.Close(0)
            raise
        
        terr = project.CreateTerritory( None, None, None)
        
        trans = project.BeginTransaction( terr)
        rf = project.RootFolder
        
        objs = list( rf.ChildObjects )
        
        one_fol = rf.ChildObjectByRelID( 2 )
        assert one_fol.Name == "NewFolder"
        one_fol = rf.ChildObjectByRelID( 1 )
        assert one_fol.Name == "NewFolder"

        ### try the methods for the rootfolder looking for "NewFolder"
        res = rf.ObjectByPath( "@NewFolder|kind=Folder")    # search for a non-unique object, returns NULL
        try:
            assert not res
            assert res == None
            # this should throw 'AttributeError'
            s_dummy = res.Name
            assert 0
        except AttributeError:
            # goood, res is null
            pass
        except:
            assert 0        

        res = rf.NthObjectByPath( 0, "@NewFolder|kind=Folder|relpos=0")    # should be OK
        assert res.Name == "NewFolder"
        
        
        one_pri = one_fol.ChildObjectByRelID( 2 )
        assert one_pri.Name == "NewPrimitive"
        one_pri = one_fol.ChildObjectByRelID( 1 )
        assert one_pri.Name == "NewPrimitive"
        
        ### try the methods for the "NewFolder" looking for the primitive
        res = one_fol.ObjectByPath( "@NewPrimitive|kind=Primitive")    # search for a non-unique object, returns NULL
        try:
            # this should throw
            s_dummy = res.Name
            assert 0
        except AttributeError:
            # goood, res is null
            pass
        except:
            assert 0            

        res = one_fol.NthObjectByPath( 0, "@NewPrimitive|kind=Primitive|relpos=0")    # should be OK
        assert res
        assert res.Name == "NewPrimitive"
        assert res == one_pri
        
        ### ... now looking for an atom
        res = one_fol.ObjectByPath( "@NewPrimitive|kind=Primitive/@NewOutputParameters|kind=OutputParam")    # search for a non-unique object, returns NULL
        try:
            # this should throw
            s_dummy = res.Name
            assert 0
        except AttributeError:
            # goood, res is null
            pass
        except:
            assert 0            

        res = one_fol.NthObjectByPath( 0, "@NewPrimitive|kind=Primitive|relpos=0/@NewOutputParameters|kind=OutputParam|relpos=0")    # should be OK
        assert res.Name == "NewOutputParameters"
        assert res.RelID == 3

        res = one_fol.NthObjectByPath( 0, "@NewPrimitive|kind=Primitive|relpos=1/@NewOutputParameters|kind=OutputParam|relpos=0")    # should be OK
        assert res.Name == "NewOutputParameters"
        assert res.RelID == 1

        ### try the methods for the "NewPrimitive" looking for the atom
        res = one_pri.ObjectByPath( "@NewOutputParameters|kind=OutputParam")    # search for a unique object
        assert res.Name == "NewOutputParameters"
        assert res.RelID == 3

        res = one_pri.NthObjectByPath( 0, "@NewOutputParameters|kind=OutputParam|relpos=888")    # should be OK, even with invalid relpos
        assert res.Name == "NewOutputParameters"
        assert res.RelID == 3 # the same as above
        
        f1 = rf.ObjectByPath( "#1/#1/#3")
        f2 = rf.NthObjectByPath( 0, "/@NewFolder|kind=Folder|relpos=0/@NewPrimitive|kind=Primitive|relpos=0/@NewOutputParameters|kind=OutputParam|relpos=0")
        assert f1 == f2
        
        f1 = rf.ObjectByPath( "#1/#2/#1")
        f2 = rf.NthObjectByPath( 0, "/@NewFolder|kind=Folder|relpos=0/@NewPrimitive|kind=Primitive|relpos=1/@NewOutputParameters|kind=OutputParam|relpos=0")
        assert f1 == f2
        
        f1 = rf.ObjectByPath( "#2/#1/#1")
        f2 = rf.NthObjectByPath( 0, "/@NewFolder|kind=Folder|relpos=1/@NewPrimitive|kind=Primitive|relpos=0/@NewOutputParameters|kind=OutputParam|relpos=0")
        assert f1 == f2
        
        # commit trans
        project.CommitTransaction()
        
        project.Save()
        project.Close(0)

"""
    Let's produce the test suites
"""
# 1st way
def suites():
    suite = unittest.makeSuite(TestCase6,'test')
    return suite

# 2nd way
def suites2():
    tc = TestCase6()
    #all = tc.my_suite()
    #return all

# 3rd way
def suites3():
    suite = unittest.TestSuite()
    suite.addTest( TestCase6( "testA"))
    #suite.addTest( TestCase6( "testB"))
    #suite.addTest( TestCase6( "testC"))
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
    # unittest.main(defaultTest='TestCase6')
    
    ######## 3rd way
    # all = suites3()
    # runner = unittest.TextTestRunner()
    # runner.run( all )
