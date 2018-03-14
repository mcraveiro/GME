import unittest
import os
import os.path
import GPyUnit.util
from GPyUnit.util import DispatchEx

_filedir = os.path.dirname(os.path.abspath(__file__))
def _adjacent_file(file):
    return os.path.join(_filedir, file)

class TestDecorators(unittest.TestCase):
    def test_MetaGME(self):
        self.runDecorators(os.environ['GME_ROOT'] + r"\Paradigms\MetaGME\MetaGME-model.xme")

    def test_UML(self):
        self.runDecorators(os.environ['GME_ROOT'] + r"\Paradigms\UML\UMLMeta.xme")

    def runDecorators(self, xme_file):
        mga = GPyUnit.util.parse_xme(self.connstr, xme_file)
        mga.Save()
        mga.Close()

        self.gme = DispatchEx("GME.Application")
        self.gme.OpenProject(self.connstr)
        dumpwmf = DispatchEx("MGA.DumpWMF")
        dumpwmf.DumpWMFs(self.outdir, self.gme)

    outdir = None
    gme = None
    def tearDown(self):
        if self.gme:
            self.gme.Exit()
            del(self.gme)
        import glob
        if self.outdir:
            for file in glob.glob(self.outdir + '/*'):
                os.unlink(file)
    
    def setUp(self):
        outdir = _adjacent_file("TestDecoratorsOutput")
        if not os.path.isdir(outdir):
            os.mkdir(outdir)
        self.outdir = outdir

    @property
    def connstr(self):
        return "MGA=" + _adjacent_file("TestDecorators.mga")
        
if __name__ == "__main__":
    unittest.main()
