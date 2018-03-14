import unittest
import os
import GPyUnit.util
from GPyUnit.util import DispatchEx

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestCopy(unittest.TestCase):
    def test_CopyFCODupGUID(self):
        mga = GPyUnit.util.parse_xme(self.connstr, _adjacent_file(r'SFDemo.xme'))
        mga.Save()
        try:
            mga.BeginTransactionInNewTerr()
            try:
                folder1 = mga.RootFolder.CreateFolder(mga.RootMeta.RootFolder.GetDefinedFolderByNameDisp('Folder', True))
                folder1.Name = 'folder1'
                folder1.MoveFCODisp(mga.ObjectByPath('/@System'))
                folder2 = mga.RootFolder.CopyFolderDisp(folder1)
                self.assertNotEqual(folder1.ObjectByPath('/@System').GetGuidDisp(), folder2.ObjectByPath('/@System').GetGuidDisp())
                self.assertNotEqual(folder1.ObjectByPath('/@System/@DBSetup1').GetGuidDisp(), folder2.ObjectByPath('/@System/@DBSetup1').GetGuidDisp())
            finally:
                mga.CommitTransaction()
        finally:
            mga.Close(False)
    
    @property
    def connstr(self):
        return "MGA=" + _adjacent_file("copytest.mga")

GPyUnit.util.MUGenerator(globals(), TestCopy)

if __name__ == "__main__":
        unittest.main()
