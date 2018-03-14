import unittest
import win32com.client
import win32ui
import pythoncom
import os
import pywintypes
import utils.Builder
bd = utils.Builder

from GPyUnit.util import DispatchEx

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestCase7( unittest.TestCase ):
    """
    avoid if possible the overriding of __init__, yet if you override use the following form:
    def __init__(self, methodName='runTest'):
            unittest.TestCase.__init__(self, methodName)
    """

    project = None
    
    def setUp( self ):              ## hook method
        pass

    def tearDown( self ):           ## hook method
        if self.project:
            self.project.Close( 0 )
        pass

    def findInProj( self, project, obj_name = "", obj_kind = ""):
        # create a filter
        filter = project.CreateFilter()
        
        # use Name filter
        filter.Name = obj_name
        filter.Kind = obj_kind

        try:
            some_fcos = project.AllFCOs( filter )
            if some_fcos.Count > 0:
                    return some_fcos.Item( 1 )
            else:
                print "findInProj >> Object not found : name = '" + obj_name + "' kind = '" + obj_kind + "'"
                assert 0
        except:
            print "findInProj >> Exception : name = '" + obj_name + "' kind = '" + obj_kind + "'"
            assert 0

    def populate7A(self, p):
        pri = bd.newObjInFold( p, p.RootFolder, 'Primitive')
        pri.Name = 'NewPrimitive'
        
        i1 = bd.new( p, pri, 'InputParameters')
        i1.Name = 'NewInputParameters'
        
        i2 = bd.new( p, pri, 'InputSignals')
        i2.Name = 'NewInputSignals'
        
        i3 = bd.new( p, pri, 'OutputParameters')
        i3.Name = 'NewOutputParameters'
        
        i4 = bd.new( p, pri, 'OutputParameters')
        i4.Name = 'NewOutputParameters'
        
        i5 = bd.new( p, pri, 'OutputSignals')
        i5.Name = 'NewOutputSignals'
        
        i6 = bd.new( p, pri, 'Parameters')
        i6.Name = 'NewParameters'
        

    def testA( self ):
        """    testA
                test properties of IMgaFCO such as
                        Meta
                        MetaRole
                        ParentModel
                        ParentFolder
        """
                
        mganame = _adjacent_file("_tc7_A_sf.mga")

        # create the project with the needed kinds
        project = bd.creaP( mganame, "SF")
        if not project:
            self.fail("Create failed")
        try:
            self.populate7A( project)
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

        to_find = "id-0066-00000002"
        fco1 = project.GetFCOByID( to_find )

        to_find = "id-0066-00000003"
        fco3 = project.GetFCOByID( to_find )
        to_find = "id-0066-00000004"
        fco4 = project.GetFCOByID( to_find )
        
        assert fco3.Meta == fco4.Meta
        metafco3 = fco3.Meta            # this line causes win32 exception
        assert metafco3.Name == "OutputParam"
        
        assert fco3.MetaRole == fco4.MetaRole
        metarole3 = fco3.MetaRole       # this line causes win32 exception
        assert metarole3.Name == "OutputParameters"
        
        mod = fco3.ParentModel
        par = mod.ParentFolder
        
        assert par == project.RootFolder

        # commit trans
        project.CommitTransaction()
        

        project.Save()
        project.Close(0)

    def populate7B(self, p):
        folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
        folder1.Name = 'NewFolder'
        
        folder2 = bd.newFolder( p, folder1, 'Folder')
        folder2.Name = 'NewFolder'

        folder3 = bd.newFolder( p, folder2, 'Folder')
        folder3.Name = 'NewFolder'

        c1 = bd.newObjInFold( p, folder3, 'Compound')
        c1.Name = 'NewCompound'
        
        for i in range(0,7):
            ci = bd.newObjInFold( p, folder3, 'Compound')
            ci.Name = 'NewCmp'
        
        c2 = bd.new( p, c1, 'CompoundParts')
        c2.Name = 'NewCompoundParts'
        
        c3 = bd.new( p, c2, 'CompoundParts')
        c3.Name = 'NewCompoundParts'
        
        c4 = bd.new( p, c3, 'CompoundParts')
        c4.Name = 'NewCompoundParts'
        
        c5 = bd.new( p, c4, 'CompoundParts')
        c5.Name = 'NewCompoundParts'
        
        c6 = bd.new( p, c5, 'CompoundParts')
        c6.Name = 'NewCompoundParts'
        
        c7 = bd.new( p, c5, 'PrimitiveParts')
        c7.Name = 'NewPrimitiveParts'
        
        
    def testB( self ):
        """    testB
                test properties of IMgaFCO such as
                        ParentModel
                        ParentFolder
        """

        mganame = _adjacent_file("_tc7_B_sf.mga")

        # create the project with the needed kinds
        project = bd.creaP( mganame, "SF")
        if not project:
            self.fail("Create failed")
        try:
            self.populate7B( project)
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

        to_find = "id-0065-0000000d"
        fco3 = project.GetFCOByID( to_find )
        to_find = "id-0065-0000000e"
        fco4 = project.GetFCOByID( to_find )
        
        last_parent = fco3;
        mod = fco3.ParentModel
        count_model_parents = 0
        while mod != None:
            count_model_parents = 1 + count_model_parents
            last_parent = mod
            mod = mod.ParentModel
        
        assert count_model_parents == 5

        fol = last_parent.ParentFolder
        count_folder_parents = 0
        while fol != None:
            count_folder_parents = 1 + count_folder_parents
            last_parent = fol
            fol = fol.ParentFolder
        
        assert count_folder_parents == 4

        assert last_parent == project.RootFolder

        # commit trans
        project.CommitTransaction()
        
        project.Save()
        project.Close(0)

    def populate7C(self, p):
        folder1 = bd.newFolder( p, p.RootFolder, 'Folder')
        folder1.Name = 'Samples'
        
        folder2 = bd.newFolder( p, p.RootFolder, 'Folder')
        folder2.Name = 'Universe'

        c1 = bd.newObjInFold( p, folder1, 'Compound')
        c1.Name = 'NewCompound'
        
        c2 = bd.newObjInFold( p, folder1, 'Primitive')
        c2.Name = 'NewPrimitive'
        
        c3 = bd.new( p, c1, 'CompoundParts')
        c3.Name = 'NewCompoundParts'
        
        i1 = bd.new( p, c3, 'InputParameters')
        i1.Name = 'NewInputParameters'
        
        i2 = bd.new( p, c3, 'InputSignals')
        i2.Name = 'NewInputSignals'
        
        c4 = bd.newObjInFold( p, folder2, 'Compound')
        c4.Name = 'NewCompound'
        
        c5 = bd.newObjInFold( p, folder2, 'Primitive')
        c5.Name = 'NewPrimitive'
        
        c6 = bd.new( p, c4, 'CompoundParts')
        c6.Name = 'NewCompoundParts'
        
        c7 = bd.new( p, c4, 'PrimitiveParts')
        c7.Name = 'NewPrimitiveParts'
        
        i3 = bd.new( p, c6, 'InputParameters')
        i3.Name = 'NewInputParameters'
        
        i4 = bd.new( p, c6, 'InputParameters')
        i4.Name = 'NewInputParameters'
        
        i5 = bd.new( p, c6, 'InputSignals')
        i5.Name = 'NewInputSignals'
        
        i6 = bd.new( p, c6, 'InputSignals')
        i6.Name = 'NewInputSignals'
        
        
        i7 = bd.new( p, c7, 'InputParameters')
        i7.Name = 'NewInputParameters'
        
        i8 = bd.new( p, c7, 'InputParameters')
        i8.Name = 'NewInputParameters'
        
        i9 = bd.new( p, c7, 'InputSignals')
        i9.Name = 'NewInputSignals'
        
        ia = bd.new( p, c7, 'InputSignals')
        ia.Name = 'NewInputSignals'        

        ib = bd.new( p, c5, 'InputParameters')
        ib.Name = 'NewInputParameters'
        
        ic = bd.new( p, c5, 'InputSignals')
        ic.Name = 'NewInputSignals'
        
        id = bd.new( p, c5, 'OutputParameters')
        id.Name = 'NewOutputParameters'
        
                
    def testC( self ):
        """    testC
                test properties of IMgaFCO such as
                        Part
                        Parts
                        PartByMetaPart
        """
        
        mganame = _adjacent_file("_tc7_C_sf.mga")

        # create the project with the needed kinds
        project = bd.creaP( mganame, "SF")
        if not project:
            self.fail("Create failed")
        try:
            self.populate7C( project)
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

        to_find = "id-0066-00000001"
        inpar = project.GetFCOByID( to_find )   # inputparameter

        insig = project.ObjectByPath("/@Samples/@NewCompound/@NewCompoundParts/@NewInputSignals")
        assert insig
        
        to_find = "id-0065-00000003"
        cp = project.GetFCOByID( to_find )      # newcompoundparts
        
        to_find = "id-0065-00000004"
        pp = project.GetFCOByID( to_find )      # newprimitiveparts
        
        aspect_1 = cp.Meta.AspectByName("ParameterAspect")
        aspect_2 = cp.Meta.AspectByName("SignalFlowAspect")
        
        #
        # tests for insig "InputSignal"
        #
        with self.assertRaises(pywintypes.com_error):
            insig.Part(aspect_1)

        part = insig.Part(aspect_2)
        assert part != None
        assert part in insig.Parts
        metapart = part.Meta
        another_part = insig.PartByMetaPart( metapart)
        assert another_part in insig.Parts
        assert another_part == part
        assert another_part.FCO.Name == part.FCO.Name
        
        #
        # tests for inpar "InputParameter"
        #
        with self.assertRaises(pywintypes.com_error):
            inpar.Part(aspect_2)
        
        part = inpar.Part( aspect_1)
        assert part != None

        assert part in inpar.Parts

        metapart = part.Meta
        another_part = inpar.PartByMetaPart( metapart)
        assert another_part in inpar.Parts
        assert another_part == part
        assert another_part.FCO.Name == part.FCO.Name
        
        # commit trans
        project.CommitTransaction()
        

        project.Save()
        project.Close(0)

    def populate7D(self, p):
        c1 = bd.newObjInFold( p, p.RootFolder, 'Compound')
        c1.Name = 'NewCompound'
        
        p1 = bd.new( p, c1, 'PrimitiveParts')
        p1.Name = 'NewPrimitiveParts'
        
        ip1 = bd.new( p, p1, 'InputParameters')
        ip1.Name = 'NewInputParameters'
        

    def testD( self ):
        """    testD
                test properties of IMgaFCO such as
                        AttributeByName -get/put
                        StrAttrByName   -get/put
                        IntAttrByName   -get/put
                        
                first inquire the original values, then modify them
                inquire once again, revert back to the original ones 
                then modify and inquire once again using the variant put/getter AttributeByName
        """

        mganame = _adjacent_file("_tc7_D_sf.mga")

        # create the project with the needed kinds
        project = bd.creaP( mganame, "SF")
        if not project:
            self.fail("Create failed")
        try:
            self.populate7D( project)
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

        to_find = "id-0066-00000001"
        ip = project.GetFCOByID( to_find )      # inputparameter
        
        to_find = "id-0065-00000002"
        pp = project.GetFCOByID( to_find )      # newprimitiveparts
        
        aspect_1 = pp.Meta.AspectByName("ParameterAspect")
        aspect_2 = pp.Meta.AspectByName("SignalFlowAspect")
        
        assert 1 == ip.GetIntAttrByNameDisp( "Size" )              # default value
        assert "Integer" == ip.GetStrAttrByNameDisp( "DataType" )  # default value
        
        # modify attributes
        ip.SetIntAttrByNameDisp( "Size", 2)
        ip.SetStrAttrByNameDisp( "DataType", "Double")
        
        assert 2 == ip.GetIntAttrByNameDisp( "Size" )
        assert "Double" == ip.GetStrAttrByNameDisp( "DataType" )
        
        # revert changes
        ip.SetIntAttrByNameDisp( "Size", 1)
        ip.SetStrAttrByNameDisp( "DataType", "Integer")

        assert 10 == pp.GetIntAttrByNameDisp( "Priority" )
        assert "" == pp.GetStrAttrByNameDisp( "Script" )
        assert "IFALL" == pp.GetStrAttrByNameDisp( "Firing" )
        
        # modify
        pp.SetIntAttrByNameDisp( "Priority", 5 )
        d_s = "int dummy_script()\n{ return 1; }"
        pp.SetStrAttrByNameDisp( "Script", d_s)
        pp.SetStrAttrByNameDisp( "Firing", "IFANY" )
        
        assert 5 == pp.GetIntAttrByNameDisp( "Priority" )
        assert d_s == pp.GetStrAttrByNameDisp( "Script" )
        assert "IFANY" == pp.GetStrAttrByNameDisp( "Firing" )

        # test whether VARIANTS are returning back the values
        pr = 0 # integer
        pr = pp.GetAttributeByNameDisp( "Priority")
        assert pr == 5
        
        scr = ""
        scr = pp.GetAttributeByNameDisp( "Script" )
        assert scr == d_s

        fir = "IFALL"
        fir = pp.GetAttributeByNameDisp( "Firing" )
        assert "IFANY" == fir

        # revert changes using AttributeByName (test whether VARIANTS are interpreted all right)
        old_pr_val = 10
        pp.SetAttributeByNameDisp( "Priority", old_pr_val )

        old_scr_val = ""
        pp.SetAttributeByNameDisp( "Script", old_scr_val )
        
        old_fir_val = "IFALL"
        pp.SetAttributeByNameDisp( "Firing", old_fir_val )                     
        
        # the values must be the original ones
        assert 10 == pp.GetIntAttrByNameDisp( "Priority")
        assert "" == pp.GetStrAttrByNameDisp( "Script")
        assert "IFALL" == pp.GetStrAttrByNameDisp( "Firing")

        # commit trans
        project.CommitTransaction()
        
        project.Save()
        project.Close(0)

    def populate7F(self, p):
        f = bd.newObjInFold( p, p.RootFolder, 'NewModel')
        f.Name = 'NewNewModel'
        
        a = bd.new( p, f, 'NewAtom')
        a.Name = 'NewNewAtom'

    def _notworking_testF( self ):
        """    testF
                test properties of IMgaFCO such as
                        AttributeByName -get/put
                        FloatAttrByName -get/put
                        ClearAttrByName
                        
                test put/get methods on double type FieldAttributes on a custom (FloatAttr) paradigm
        """

        mganame = _adjacent_file("_tc7_F_fl.mga")

        # create the project with the needed kinds
        project = bd.creaP( mganame, "FloatAttr")
        if not project:
            self.fail("Create failed")
        try:
            self.populate7F( project)
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

        to_find = "NewNewAtom"
        at = self.findInProj( project, to_find)

        try:
            nnnname = at.Name
        except:
            print "Caught 1"
            assert 0
    
        
        assert 0.000 == at.GetFloatAttrByNameDisp( "Float1" )          # default value
        assert 1.001 == at.GetFloatAttrByNameDisp( "Float2" )          # default value
        
        at.SetFloatAttrByNameDisp( "Float1", 2.002 )
        at.SetFloatAttrByNameDisp( "Float2", 3.003 )
        
        assert 2.002 == at.GetFloatAttrByNameDisp( "Float1" )
        assert 3.003 == at.GetFloatAttrByNameDisp( "Float2" )

        at.SetAttributeByNameDisp( "Float1", -4.004 )
        at.SetAttributeByNameDisp( "Float2", -5.005 )
        
        # test whether VARIANTS are returning back good values
        assert -4.004 == at.GetAttributeByNameDisp( "Float1" )
        assert -5.005 == at.GetAttributeByNameDisp( "Float2" )

        # reset the attribute to its default value and to its unset state
        at.ClearAttrByName( "Float1" )
        at.ClearAttrByName( "Float2" )

        assert 0.000 == at.GetFloatAttrByNameDisp( "Float1" )          # default value
        assert 1.001 == at.GetFloatAttrByNameDisp( "Float2" )          # default value

        # commit trans
        project.CommitTransaction()
        
        project.Save()
        project.Close(0)

        pass

"""
        Let's produce the test suites
"""
# 1st way
def suites():
        suite = unittest.makeSuite(TestCase7,'test')
        return suite

# 2nd way
def suites2():
        tc = TestCase7()
        #all = tc.my_suite()
        #return all

# 3rd way
def suites3():
        suite = unittest.TestSuite()
        #suite.addTest( TestCase7( "testA"))
        #suite.addTest( TestCase7( "testB"))
        suite.addTest( TestCase7( "testE"))
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
    # unittest.main(defaultTest='TestCase7')
    
    ######## 3rd way
    # all = suites3()
    # runner = unittest.TextTestRunner()
    # runner.run( all )
