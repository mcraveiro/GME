import unittest
import os
import os.path
import GPyUnit.util
from GPyUnit.util import DispatchEx

_filedir = os.path.dirname(os.path.abspath(__file__))
def _adjacent_file(file):
    return os.path.join(_filedir, file)

class TestMetaInterpreter(unittest.TestCase):
    def test_AttributeOrdering(self):
        mga = GPyUnit.util.parse_xme(self.connstr)
        try:
            mga.Save()
            selectedobj = DispatchEx("Mga.MgaFCOs")
            launcher = DispatchEx("Mga.MgaLauncher")
            launcher.RunComponent("Mga.Interpreter.MetaInterpreter", mga, None, selectedobj, 128)
        finally:
            mga.Close()

        with open(os.path.join(self.outdir(), "MetaGME.xmp")) as xmp:
            lines = xmp.readlines()
            atomattributes = "IsAbstract InRootFolder GeneralPreferences DisplayedName IsTypeShown Icon IsNameEnabled PortIcon SubTypeIcon NamePosition InstanceIcon Decorator NameWrapNum IsHotspotEnabled IsResizable AutoRouterPref HelpURL IsGradientFillEnabled GradientFillColor GradientFillDirection IsShadowCastEnabled ShadowColor ShadowThickness ShadowDirection IsRoundRectangleEnabled RoundRectangleRadius"
            self.assertTrue(atomattributes in "".join(lines), filter(lambda x: '<atom name = "Atom"' in x, lines))
        
        metaproj = DispatchEx("Mga.MgaMetaProject")
        metaproj.Open("MGA=" + os.path.join(self.outdir(), "MetaGME.mta"))
        try:
            atomattrs = [attr.Name for attr in metaproj.RootFolder.DefinedFCOByName("Atom", True).Attributes]
            self.assertEqual(atomattrs, list(atomattributes.split()))
        finally:
            metaproj.Close()

    def test_attrpool_clear(self):
        mga = GPyUnit.util.parse_xme(self.connstr)
        # mga = DispatchEx("Mga.Mgaproject")
        # mga.Open(self.connstr)
        try:
            mga.Save()

            mga.BeginTransactionInNewTerr()
            try:
                obj = mga.RootFolder.GetObjectByPathDisp("/@Stereotypes/@AtomProxy")
                attrs = list(obj.Attributes)
                # print [a.Meta.MetaRef for a in attrs]
                # add obj to CMgaProject.changedobjs
                obj.SetStrAttrByNameDisp("Decorator", "asdf")
            finally:
                # abort calls changedobjs.front()->apool.clear()
                # Before 6/26/2017: Under appverif with Heaps (full): "Win32 exception occurred releasing IUnknown at 0x503dcfc8"
                mga.AbortTransaction()
            del(attrs)
        finally:
            mga.Close()
    
    def _rm_old_files(self):
        for file in ("MetaGME.xmp", "MetaGME.mta", "MetaGME.xmp.log"):
            if os.path.isfile(os.path.join(self.outdir(), file)):
                os.unlink(os.path.join(self.outdir(), file))

    def tearDown(self):
        self._rm_old_files()
        registrar = DispatchEx("Mga.MgaRegistrar")
        if self.old_reg_data is None:
            registrar.UnregisterParadigm("MetaGME", 1)
        else:
            registrar.RegisterParadigm(*self.old_reg_data)


    def setUp(self):
        self._rm_old_files()
        self.old_reg_data = GPyUnit.util.get_MetaGME_user_reg_data()

    @property
    def connstr(self):
        return "MGA=" + _adjacent_file("MetaInterpretertest.mga")
    
    def outdir(self):
        return os.path.abspath(_adjacent_file('.'))

GPyUnit.util.MUGenerator(globals(), TestMetaInterpreter)
def _muoutdir(self):
    return os.path.abspath(os.getcwd())
try:
    TestMetaInterpreterMU.outdir = _muoutdir
    TestMetaInterpreterMUSVN.outdir = _muoutdir
    TestMetaInterpreterMUSVNHashed.outdir = _muoutdir
except NameError:
    pass # MU isn't tested under x64

class TestCSharpDSMLGenerator(unittest.TestCase):
    def test_run(self):
        mga = GPyUnit.util.parse_xme(self.connstr)
        try:
            mga.Save()
            selectedobj = DispatchEx("Mga.MgaFCOs")
            launcher = DispatchEx("Mga.MgaLauncher")
            launcher.RunComponent("Mga.Interpreter.MetaInterpreter", mga, None, selectedobj, 128)
            launcher.RunComponent("Mga.Interpreter.CSharpDSMLGenerator", mga, None, selectedobj, 128)
        finally:
            mga.Close()
        self.assertTrue(os.path.isfile(os.path.join(self.outdir(), "ISIS.GME.Dsml.MetaGME.Classes.cs")))
        self.assertTrue(os.path.isfile(os.path.join(self.outdir(), "ISIS.GME.Dsml.MetaGME.dll")))

            
    def _rm_old_files(self):
        for filename in ("MetaGME.xmp", "MetaGME.mta", "MetaGME.xmp.log"):
            if os.path.isfile(os.path.join(self.outdir(), filename)):
                os.unlink(os.path.join(self.outdir(), filename))
        for filename in ("AssemblySignature.snk", "ISIS.GME.Dsml.MetaGME.Classes.cs", "ISIS.GME.Dsml.MetaGME.dll", "ISIS.GME.Dsml.MetaGME.Interfaces.cs", "ISIS.GME.Dsml.MetaGME.xml", "ISIS.GME.Dsml.pdb"):
            path = os.path.join(self.outdir(), filename)
            if os.path.isfile(path):
                os.unlink(path)

    def tearDown(self):
        registrar = DispatchEx("Mga.MgaRegistrar")
        if self.old_reg_data is None:
            registrar.UnregisterParadigm("MetaGME", 1)
        else:
            registrar.RegisterParadigm(*self.old_reg_data)
        self._rm_old_files()


    def setUp(self):
        self.old_reg_data = GPyUnit.util.get_MetaGME_user_reg_data()
        self._rm_old_files()

    @property
    def connstr(self):
        return "MGA=" + _adjacent_file("MetaInterpretertest.mga")
    
    def outdir(self):
        return os.path.abspath(_adjacent_file('.'))

if GPyUnit.util._opts.Dispatch_x64:
    del TestCSharpDSMLGenerator
    #FIXME: TestCSharpDSMLGenerator on x64 make PGO fail
        
if __name__ == "__main__":
    unittest.main()
