import unittest
import os
import os.path
import GPyUnit.util
from GPyUnit.util import DispatchEx

_filedir = os.path.dirname(os.path.abspath(__file__))
def _adjacent_file(file):
    return os.path.join(_filedir, file)

class TestCSharpDSMLGenerator(unittest.TestCase):
    def test_run(self):
        mga = GPyUnit.util.parse_xme("MGA=" + _adjacent_file("BidirConnection.mga"), _adjacent_file("BidirConnection.xme"))
        print self.connstr
        try:
            mga.Save()
            selectedobj = DispatchEx("Mga.MgaFCOs")
            launcher = DispatchEx("Mga.MgaLauncher")
            #launcher.RunComponent("Mga.Interpreter.MetaInterpreter", mga, None, selectedobj, 128)
            launcher.RunComponent("Mga.Interpreter.CSharpDSMLGenerator", mga, None, selectedobj, 128)
        finally:
            mga.Close()
        self.assertTrue(os.path.isfile(os.path.join(self.outdir(), "ISIS.GME.Dsml.BidirConnection.Classes.cs")))
        self.assertTrue(os.path.isfile(os.path.join(self.outdir(), "ISIS.GME.Dsml.BidirConnection.dll")))

        mga = GPyUnit.util.parse_xme(self.connstr, _adjacent_file("BidirConnectionModel.xme"))
        mga.Save()
        mga.BeginTransactionInNewTerr()
        try:
            self.assertEqual(mga.ObjectByPath("/@NewModel/@Child1").PartOfConns.Count, 2)
            self.assertEqual(set([x.ConnRole for x in mga.ObjectByPath("/@NewModel/@Child1").PartOfConns]), set(['src', 'dst']))
            self.assertEqual(set([x.ConnRole for x in mga.ObjectByPath("/@NewModel/@Child2").PartOfConns]), set(['src', 'dst']))
        finally:
            mga.AbortTransaction()
            mga.Close()
        
        import subprocess
        subprocess.check_call([_adjacent_file(r"DsmlGeneratorTest\.nuget\NuGet.exe"), "restore"], cwd=_adjacent_file("DsmlGeneratorTest"))
        subprocess.check_call([r"c:\Windows\Microsoft.NET\Framework\v4.0.30319\msbuild.exe", _adjacent_file(r"DsmlGeneratorTest\DsmlGeneratorTest.sln")])
        subprocess.check_call([_adjacent_file(r"DsmlGeneratorTest\bin\Debug\DsmlGeneratorTest.exe"), self.connstr])

            
    def setUp(self):
        registrar = DispatchEx("Mga.MgaRegistrar")
        registrar.RegisterParadigmFromData("XML=" + _adjacent_file("BidirConnection.xmp"), "BidirConnection", 1)

    def tearDown(self):
        for file in ("AssemblySignature.snk", "ISIS.GME.Dsml.BidirConnection.Classes.cs", "ISIS.GME.Dsml.BidirConnection.dll", 
                "ISIS.GME.Dsml.BidirConnection.Interfaces.cs", "ISIS.GME.Dsml.BidirConnection.xml", "ISIS.GME.Dsml.pdb", "BidirConnection.xmp.log", 
                "BidirConnection.mta", r"DsmlGeneratorTest\bin\Debug\DsmlGeneratorTest.exe"):
            if os.path.isfile(os.path.join(self.outdir(), file)):
                os.unlink(os.path.join(self.outdir(), file))

    @property
    def connstr(self):
        return "MGA=" + _adjacent_file("BidirConnectionModel.mga")
    
    def outdir(self):
        return os.path.abspath(_adjacent_file('.'))

if GPyUnit.util._opts.Dispatch_x64:
    del TestCSharpDSMLGenerator
    #FIXME: TestCSharpDSMLGenerator on x64 make PGO fail
        
if __name__ == "__main__":
    unittest.main()
