from __future__ import with_statement

import sys
import os.path
import unittest
import GPyUnit.util
import GPyUnit.util.gme
from GPyUnit.util import DispatchEx

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestRegistry(unittest.TestCase):
    def __init__(self, name, **kwds):
        super(TestRegistry, self).__init__(name, **kwds)
        self.output_file = "TestRegistry-output.mga"
        self.project = None
        
    def tearDown(self):
        if not self.project is None:
            self.project.Close(True)

    @property
    def connstr(self):
        return "MGA=" + _adjacent_file(self.output_file)

    def test(self):
        from GPyUnit import util
        util.register_xmp('MetaGME')
        with util.disable_early_binding():
            self.project = DispatchEx("Mga.MgaProject")
            self.project.Create(self.connstr, "MetaGME")
            self.project.BeginTransactionInNewTerr(0)
            
            rootregs = self.project.RootFolder.GetRegistryDisp(True)
            self.assertEqual(rootregs.Count, 0)
            self.project.RootFolder.GetRegistryNodeDisp('test123').Value = 'xxx'
            self.project.RootFolder.GetRegistryNodeDisp('test345').Value = 'yyy'
            
            for i in range(1, self.project.RootMeta.RootFolder.DefinedFCOs.Count+1):
                if self.project.RootMeta.RootFolder.DefinedFCOs.Item(i).Name == 'ParadigmSheet':
                    sheet_meta = self.project.RootMeta.RootFolder.DefinedFCOs.Item(i)
            sheet = self.project.RootFolder.CreateRootObject(sheet_meta)
            sheetregs = sheet.GetRegistryDisp(True)
            self.assertEqual(set([reg.Name for reg in sheetregs]), set(['namePosition', 'isTypeInfoShown'])) # this assert will fail with an old MetaGME registered
            
            namePosition = sheet.GetRegistryNodeDisp('namePosition')
            self.assertEqual(namePosition.Name, 'namePosition')
            self.assertEqual(namePosition.Value, '0')
            self.assertEqual(namePosition.GetSubNodesDisp(True).Count, 0)
            self.assertEqual(namePosition.GetSubNodesDisp(False).Count, 0)
            
            def keytest():
                newkey = sheet.GetRegistryNodeDisp('newkey')
                newkey.Value = 'newvalue'
                self.assertEqual(newkey.Value, 'newvalue')
            keytest()
            newkey = sheet.GetRegistryNodeDisp('newkey')
            self.assertEqual(newkey.GetSubNodesDisp(True).Count, 0)
            
            def subkeytest():
                newsubkey = sheet.GetRegistryNodeDisp('newkey/subkey')
                newsubkey.Value = 'subvalue'
                self.assertEqual(newsubkey.Value, 'subvalue')
                self.assertEqual(newsubkey.GetSubNodesDisp(True).Count, 0)
                newkey = sheet.GetRegistryNodeDisp('newkey')
                self.assertEqual(newkey.GetSubNodesDisp(False).Count, 1)
            subkeytest()
            

            #self.project.Save("MGA=" + _adjacent_file(self.output_file))
            self.project.CommitTransaction()
            terr = self.project.BeginTransactionInNewTerr()
            sheet = terr.OpenObj(sheet)
            keytest()
            subkeytest()
            self.project.CommitTransaction()
            
            self.project.Undo()
            self.project.Undo()
            terr = self.project.BeginTransactionInNewTerr()
            self.assertEqual(self.project.RootFolder.GetRegistryDisp(False).Count, 0)
            self.project.CommitTransaction()
            
            terr = self.project.BeginTransactionInNewTerr()
            self.project.RootFolder.GetRegistryNodeDisp('xtest123').Value = 'xxx&<'
            self.project.RootFolder.GetRegistryNodeDisp('ytest123').Value = 'yyy'
            self.project.RootFolder.GetRegistryNodeDisp('xtest123/ztest').Value = 'zzz'
            self.project.RootFolder.GetRegistryNodeDisp('xtest123/ztest/blank').Value = ''
            self.project.CommitTransaction()
            
            self.project.Save()
            self.project.Close(True)
            
            self.project.Open(self.connstr)
            terr = self.project.BeginTransactionInNewTerr()
            def testxtest():
                self.assertEqual(self.project.RootFolder.GetRegistryNodeDisp('xtest123').Value, 'xxx&<')
                self.assertEqual(self.project.RootFolder.GetRegistryNodeDisp('ytest123').Value, 'yyy')
                self.assertEqual(self.project.RootFolder.GetRegistryNodeDisp('xtest123/ztest').Value, 'zzz')
                self.assertEqual(self.project.RootFolder.GetRegistryNodeDisp('xtest123/ztest/blank').Value, '')
                ATTSTATUS_HERE = 0
                self.assertEqual(self.project.RootFolder.GetRegistryNodeDisp('xtest123/ztest/blank').Status(), ATTSTATUS_HERE)
            testxtest()
            
            self.project.RootFolder.GetRegistryNodeDisp('xtest123/zt').Value = 'xxxx'
            self.assertEqual(self.project.RootFolder.GetRegistryNodeDisp('xtest123/zt').GetSubNodesDisp().Count, 0)
            self.assertEqual(self.project.RootFolder.GetRegistryDisp(False).Count, 2)
            self.project.CommitTransaction()
            self.project.Close(True)
            if self.connstr.find("MGA=") == 0:
                util.gme.mga2xme(_adjacent_file(self.output_file))
                util.gme.xme2mga(os.path.splitext(_adjacent_file(self.output_file))[0] + ".xme")
                self.project.Open(self.connstr)
                terr = self.project.BeginTransactionInNewTerr()
                testxtest()
                self.project.CommitTransaction()
                self.project.Close(True)

    def test_derived(self):
        from GPyUnit import util
        util.register_xmp('MetaGME')
        with util.disable_early_binding():
            self.project = DispatchEx("Mga.MgaProject")
            self.project.Create(self.connstr, "MetaGME")
            self.project.BeginTransactionInNewTerr()
            
            for i in range(1, self.project.RootMeta.RootFolder.DefinedFCOs.Count+1):
                if self.project.RootMeta.RootFolder.DefinedFCOs.Item(i).Name == 'ParadigmSheet':
                    sheet_meta = self.project.RootMeta.RootFolder.DefinedFCOs.Item(i)
            sheet = self.project.RootFolder.CreateRootObject(sheet_meta)
            sheet.SetRegistryValueDisp('testcreation', 'testcreate')
            sheet2 = self.project.RootFolder.DeriveRootObject(sheet, False)
            sheet3 = self.project.RootFolder.DeriveRootObject(sheet2, False)
            sheet4 = self.project.RootFolder.DeriveRootObject(sheet3, False)
            
            sheet.SetRegistryValueDisp('test123', 'test')
            self.assertEqual(sheet4.GetRegistryValueDisp('test123'), 'test')
            sheet3.DetachFromArcheType()
            self.assertEqual(sheet4.GetRegistryValueDisp('test123'), 'test')
            self.assertEqual(sheet.GetRegistryDisp(False).Count, 2)
            self.assertEqual(sheet4.GetRegistryDisp(False).Count, 0)
            self.assertEqual(sheet3.GetRegistryDisp(False).Count, 3) # detaching adds a regnode "_detachedFrom"
            self.assertEqual(sheet4.GetRegistryDisp(False).Count, 0)
            self.assertEqual(sheet4.GetRegistryDisp(False).Count, 0)
            self.project.CommitTransaction()
            self.project.Save()
            self.project.Close()


    def test_copy(self):
        from GPyUnit import util
        util.register_xmp('MetaGME')
        with util.disable_early_binding():
            self.project = DispatchEx("Mga.MgaProject")
            self.project.Create(self.connstr, "MetaGME")
            self.project.BeginTransactionInNewTerr()
            
            for i in range(1, self.project.RootMeta.RootFolder.DefinedFCOs.Count+1):
                if self.project.RootMeta.RootFolder.DefinedFCOs.Item(i).Name == 'ParadigmSheet':
                    sheet_meta = self.project.RootMeta.RootFolder.DefinedFCOs.Item(i)
            sheet = self.project.RootFolder.CreateRootObject(sheet_meta)
            sheet.SetRegistryValueDisp('test123', 'test')
            sheet2 = self.project.RootFolder.CopyFCODisp(sheet)
            self.assertEqual(sheet2.GetRegistryValueDisp('test123'), 'test')
            self.project.CommitTransaction()
            self.project.Save()
            self.project.Close()

    def test_subtree_removal(self):
        from GPyUnit import util
        util.register_xmp('MetaGME')
        with util.disable_early_binding():
            self.project = DispatchEx("Mga.MgaProject")
            self.project.Create(self.connstr, "MetaGME")
            self.project.BeginTransactionInNewTerr()
            
            for i in range(1, self.project.RootMeta.RootFolder.DefinedFCOs.Count+1):
                if self.project.RootMeta.RootFolder.DefinedFCOs.Item(i).Name == 'ParadigmSheet':
                    sheet_meta = self.project.RootMeta.RootFolder.DefinedFCOs.Item(i)
            sheet = self.project.RootFolder.CreateRootObject(sheet_meta)
            #sys.stdin.readline()
            sheet.SetRegistryValueDisp('test123', 'test')
            sheet.SetRegistryValueDisp('test123/123', 'test')
            sheet.SetRegistryValueDisp('test123456', 'test')
            sheet.SetRegistryValueDisp('test123456/123', 'test')
            sheet.GetRegistryNodeDisp('test123').RemoveTree()
            ATTSTATUS_UNDEFINED = -2
            self.assertEqual(sheet.GetRegistryNodeDisp('test123').Status(), ATTSTATUS_UNDEFINED)
            self.assertEqual(sheet.GetRegistryNodeDisp('test123/123').Status(), ATTSTATUS_UNDEFINED)
            self.assertEqual(sheet.GetRegistryValueDisp('test123456'), 'test')
            self.assertEqual(sheet.GetRegistryValueDisp('test123456/123'), 'test')
            self.project.CommitTransaction()
            self.project.Save()
            self.project.Close()

    def xxxtestupgrade(self):
        from GPyUnit import util
        util.register_xmp('MetaGME')
        with util.disable_early_binding():
            self.project = DispatchEx("Mga.MgaProject")
            self.project.Open("MGA=" + r"C:\Users\ksmyth\Documents\META\meta\CyPhyML\CyPhyML.mga")
            self.project.BeginTransactionInNewTerr()
            self.project.RootFolder.ChildFolders

import GPyUnit.util
GPyUnit.util.MUGenerator(globals(), TestRegistry)

if __name__ == "__main__":
    unittest.main()
