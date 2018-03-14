import unittest
import os
import GPyUnit.util
from GPyUnit.util import DispatchEx

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestParser(unittest.TestCase):
    def test_ParseMetaGME(self):
        mga = GPyUnit.util.parse_xme(self.connstr)
        mga.Save()
        try:
            mga.BeginTransactionInNewTerr()
            try:
                #self.assertEqual('{5297b5a3-1e47-403c-bf85-40d5c5ecce3f}', mga.RootFolder.GetGuidDisp())
                self.assertEqual('{819c860f-177a-4382-a325-4c73f616734b}', mga.ObjectByPath('/@Aspects').GetGuidDisp())
            finally:
                mga.AbortTransaction()
        finally:
            mga.Close()
        if type(self) == TestParser:
            self.assertTrue(os.path.isfile(_adjacent_file("parsertest.mga")))
    
    def test_ParseDupGuids(self):
        mga = GPyUnit.util.parse_xme(self.connstr, _adjacent_file('SFDemo_dup_guids.xme'))
        mga.Save()
        try:
            mga.BeginTransactionInNewTerr()
            try:
                self.assertEqual(mga.ObjectByPath('/@Folder1').GetGuidDisp(), '{8ce2ca06-2729-4e4c-955f-fc88194782cc}')
                self.assertEqual(mga.ObjectByPath('/@Folder1/@System').GetGuidDisp(), '{a57ca6b2-d95e-485c-a768-98c16fd30588}')
                self.assertEqual(mga.ObjectByPath('/@Folder1/@System/@DBSetup1').GetGuidDisp(), '{009ef956-cfe9-4b2a-9bed-3d486dfc71ce}')
                self.assertNotEqual(mga.ObjectByPath('/@Folder1').GetGuidDisp(), mga.ObjectByPath('/@Folder2').GetGuidDisp())
                self.assertNotEqual(mga.ObjectByPath('/@Folder1/@System').GetGuidDisp(), mga.ObjectByPath('/@Folder2/@System').GetGuidDisp())
                self.assertNotEqual(mga.ObjectByPath('/@Folder1/@System/@DBSetup1').GetGuidDisp(), mga.ObjectByPath('/@Folder2/@System/@DBSetup1').GetGuidDisp())
            finally:
                mga.AbortTransaction()
        finally:
            mga.Close()
        if type(self) == TestParser:
            self.assertTrue(os.path.isfile(_adjacent_file("parsertest.mga")))
    
    def test_ParseInTx(self):
        project = GPyUnit.util.DispatchEx("Mga.MgaProject")
        project.Create(self.connstr, "MetaGME")
        project.BeginTransactionInNewTerr()
        try:
            mga = GPyUnit.util.parse_xme(self.connstr, project=project)
        except Exception, e:
            self.assertTrue(str(e).find("already in transaction") != -1)
        project.Close(True)
    
    @property
    def connstr(self):
        return "MGA=" + _adjacent_file("parsertest.mga")

    def test_GetXMLParadigm(self):
        parser = DispatchEx("Mga.MgaParser")
        paradigm = parser.GetXMLParadigm(os.environ['GME_ROOT'] + r"\Paradigms\MetaGME\MetaGME-model.xme")
        assert paradigm == "MetaGME"
       
    def test_GetXMLInfo(self):
        parser = DispatchEx("Mga.MgaParser")
        xme = os.environ['GME_ROOT'] + r"\Paradigms\MetaGME\MetaGME-model.xme"
        (paradigm, parversion, parguid, basename, ver) = parser.GetXMLInfo(xme)
        assert paradigm == "MetaGME"
    
GPyUnit.util.MUGenerator(globals(), TestParser)

if __name__ == "__main__":
        unittest.main()
