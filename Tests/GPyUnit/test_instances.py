import unittest
import os
import win32com.client
import GPyUnit.util
from GPyUnit.util import dec_disable_early_binding, DispatchEx

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestInstances(unittest.TestCase):
    project = None
    def __init__(self, name, **kwds):
        super(TestInstances, self).__init__(name, **kwds)

    def tearDown(self):
        if self.project:
            self.project.Close(True)

    @property
    def connstr(self):
        return "MGA=" + _adjacent_file("tmp.mga")
    
    @dec_disable_early_binding
    def test_CreateInstances(self):
        self.project = GPyUnit.util.parse_xme(self.connstr)
        self.project.BeginTransactionInNewTerr()
        for i in range(3): # exponential!
            for model in self.project.RootFolder.ChildFCOs:
                self.project.RootFolder.DeriveRootObject(model, True)
        self.project.CommitTransaction()

    @dec_disable_early_binding
    def test_RedirectBaseReference(self):
        self.project = GPyUnit.util.parse_xme(self.connstr)
        self.project.BeginTransactionInNewTerr()
        aspects = self.project.RootFolder.GetObjectByPathDisp("/@Aspects")
        allproxy = self.project.RootFolder.GetObjectByPathDisp("/@Aspects/@AllRef")
        aspects_instance = self.project.RootFolder.DeriveRootObject(aspects, True)
        aspects_subtype = self.project.RootFolder.DeriveRootObject(aspects, False)
        attributes = self.project.RootFolder.GetObjectByPathDisp("/@Stereotypes/@Attributes")
        allproxy.Referred = attributes
        for asp in (aspects_instance, aspects_subtype):
            self.assertEqual(asp.GetObjectByPathDisp("/@AllRef").Referred.Name, attributes.Name)
            self.assertEqual(asp.GetObjectByPathDisp("/@AllRef").Referred.ID, attributes.ID)
        self.project.CommitTransaction()

    @dec_disable_early_binding
    def test_RedirectBaseReference2(self):
        self.project = GPyUnit.util.parse_xme(self.connstr)
        self.project.BeginTransactionInNewTerr()
        aspects = self.project.RootFolder.GetObjectByPathDisp("/@Aspects")
        allproxy = self.project.RootFolder.GetObjectByPathDisp("/@Aspects/@AllRef")
        instances = []
        for inst_flag in (True, False, True, False):
            instances.append(aspects.DeriveChildObject(allproxy, allproxy.MetaRole, inst_flag))
        attributes = self.project.RootFolder.GetObjectByPathDisp("/@Stereotypes/@Attributes")
        allproxy.Referred = attributes
        try:
            instances[0].Referred = self.project.RootFolder.GetObjectByPathDisp("/@Constraints/@Constraints|kind=Aspect")
        except Exception, e:
            pass
        else:
            self.fail()
        for asp in instances:
            self.assertEqual(asp.Referred.Name, attributes.Name)
            self.assertEqual(asp.Referred.ID, attributes.ID)
        self.project.CommitTransaction()

    @dec_disable_early_binding
    def test_RedirectBaseReference3(self):
        self.project = GPyUnit.util.parse_xme(self.connstr)
        self.project.BeginTransactionInNewTerr()
        aspects = self.project.RootFolder.GetObjectByPathDisp("/@Aspects")
        allproxy = self.project.RootFolder.GetObjectByPathDisp("/@Aspects/@AllRef")
        instances = []
        for inst_flag in (True, False, True, False):
            instances.append(aspects.DeriveChildObject(allproxy, allproxy.MetaRole, inst_flag))
        self.assertFalse(instances[3].CompareToBase())
        instances[3].Referred = instances[3].Referred
        self.assertTrue(instances[3].CompareToBase())
        attributes = self.project.RootFolder.GetObjectByPathDisp("/@Stereotypes/@Attributes")
        try:
            allproxy.Referred = attributes
        except Exception, e:
            pass
        else:
            self.fail()
        instances[3].RevertToBase()
        self.assertFalse(instances[3].CompareToBase())
        allproxy.Referred = attributes
        for asp in instances:
            self.assertEqual(asp.Referred.Name, attributes.Name)
            self.assertEqual(asp.Referred.ID, attributes.ID)
        self.project.CommitTransaction()

    @dec_disable_early_binding
    def test_DetachFromArcheType_RelIDs(self):
        self.project = GPyUnit.util.parse_xme(self.connstr)
        self.project.BeginTransactionInNewTerr()
        aspects = self.project.RootFolder.GetObjectByPathDisp("/@Aspects")
        allproxy = self.project.RootFolder.GetObjectByPathDisp("/@Aspects/@AllRef")
        subtype = self.project.RootFolder.DeriveRootObject(aspects, False)
        allrefNew = subtype.CreateChildObject(allproxy.MetaRole)
        setMembership = subtype.GetObjectByPathDisp("@SetMembership")
        self.assertEquals(0x8000001, setMembership.RelID)
        
        subtype.DetachFromArcheType() # this changes RelIDs so there are no dups
        self.assertEquals(list(range(1,50+1)), sorted([fco.RelID for fco in subtype.ChildFCOs]))
        self.project.CommitTransaction()

    @dec_disable_early_binding
    def test_DetachFromArcheType_Attrs(self):
        self.project = GPyUnit.util.parse_xme(self.connstr)
        self.project.BeginTransactionInNewTerr()
        aspects = self.project.RootFolder.GetObjectByPathDisp("/@Aspects")
        aspect = self.project.RootFolder.GetObjectByPathDisp("/@Aspects/@Aspect")
        subtype = self.project.RootFolder.DeriveRootObject(aspects, False)
        instance = self.project.RootFolder.DeriveRootObject(subtype, True)

        aspect.SetStrAttrByNameDisp("GeneralPreferences", "test123")
        aspect_instance = instance.GetObjectByPathDisp("/@Aspect")
        self.assertEquals(aspect.GetStrAttrByNameDisp("GeneralPreferences"), aspect_instance.GetStrAttrByNameDisp("GeneralPreferences"))

        instance.DetachFromArcheType()
        self.assertEquals(aspect.GetStrAttrByNameDisp("GeneralPreferences"), aspect_instance.GetStrAttrByNameDisp("GeneralPreferences"))
        self.project.CommitTransaction()

    @dec_disable_early_binding
    def test_RemoveArchetypeConnpoint(self):
        self.project = GPyUnit.util.parse_xme(self.connstr)
        self.project.BeginTransactionInNewTerr()
        aspects = self.project.RootFolder.GetObjectByPathDisp("/@Aspects")
        allproxy = self.project.RootFolder.GetObjectByPathDisp("/@Aspects/@AllRef")
        subtype = self.project.RootFolder.DeriveRootObject(aspects, False)
        allrefNew = subtype.CreateChildObject(allproxy.MetaRole)
        stc = aspects.GetObjectByPathDisp("@SourceToConnector")
        model = aspects.GetObjectByPathDisp("@Model")
        stc.SetSrc(DispatchEx("Mga.MgaFCOs"), model)

        self.project.CommitTransaction()

    @dec_disable_early_binding
    def test_Connection_RevertToBase(self):
        self.project = GPyUnit.util.parse_xme(self.connstr)
        self.project.Preferences = self.project.Preferences & (0xffffffff - 4) # MGAPREF_IGNORECONNCHECKS
        self.project.BeginTransactionInNewTerr()
        aspects = self.project.RootFolder.GetObjectByPathDisp("/@Aspects")
        subtype = self.project.RootFolder.DeriveRootObject(aspects, False)
        subtype2 = self.project.RootFolder.DeriveRootObject(subtype, False)
        stc = subtype.GetObjectByPathDisp("@SourceToConnector")
        model = subtype.GetObjectByPathDisp("@Model")
        stc.SetSrc(DispatchEx("Mga.MgaFCOs"), model)
        self.assertEqual(stc.Src.ID, model.ID)
        
        self.assertEqual(1, stc.CompareToBase([cp for cp in stc.ConnPoints if cp.ConnRole == 'src'][0]))
        self.assertEqual(0, stc.CompareToBase([cp for cp in stc.ConnPoints if cp.ConnRole == 'dst'][0]))
        stc.RevertToBase(None)

        self.project.CommitTransaction()

#GPyUnit.util.MUGenerator(globals(), TestInstances)

class TestInstancesLib(unittest.TestCase):
    project = None
    def __init__(self, name, **kwds):
        super(TestInstancesLib, self).__init__(name, **kwds)

    def tearDown(self):
        if self.project:
            self.project.Close(True)

    @property
    def connstr(self):
        return "MGA=" + _adjacent_file("tmp.mga")
    
    @dec_disable_early_binding
    def test_DetachFromArcheType_InstanceIsLibObject(self):
        lib_path = 'MGA=' + _adjacent_file("instance_lib.mga")
        def createLibProject():
            lib_project = DispatchEx("Mga.MgaProject")
            lib_project.Create(lib_path, "MetaGME")
            paradigmSheet = lib_project.RootMeta.RootFolder.DefinedFCOByName('ParadigmSheet', True)
            lib_project.BeginTransactionInNewTerr()
            base = lib_project.RootFolder.CreateRootObject(paradigmSheet)
            base.Name = "PS"
            atom = base.CreateChildObject(paradigmSheet.RoleByName('Atom'))
            atom.Name = "Atom"
            lib_project.CommitTransaction()
            lib_project.Save()
            lib_project.Close()
        createLibProject()
        
        self.project = GPyUnit.util.parse_xme(self.connstr)
        self.project.BeginTransactionInNewTerr()
        lib_obj = self.project.RootFolder.AttachLibrary(lib_path)
        self.project.CommitTransaction()
        self.project.BeginTransactionInNewTerr()
        
        ps_instance = self.project.RootFolder.DeriveRootObject(lib_obj.GetObjectByPathDisp("@PS"), True)
        atom = ps_instance.GetObjectByPathDisp("@Atom")
        self.assertFalse(ps_instance.IsLibObject)
        self.assertFalse(atom.IsLibObject)
        ps_instance.DetachFromArcheType()
        self.assertFalse(atom.IsLibObject)
        self.assertFalse(ps_instance.IsLibObject)
        
        self.project.CommitTransaction()
        self.project.Save()

if __name__ == "__main__":
        unittest.main()
