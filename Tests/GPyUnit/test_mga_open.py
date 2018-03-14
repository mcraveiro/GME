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

class TestMgaOpen(unittest.TestCase):
    def __init__(self, name, **kwds):
        super(TestMgaOpen, self).__init__(name, **kwds)
        self.output_file = "test_mga_open-output.mga"
        self.project = None
        
    def tearDown(self):
        if not self.project is None:
            self.project.Close(True)

    def test(self):
        from GPyUnit import util
        util.register_xmp('SF')
        with util.disable_early_binding():
            self.project = DispatchEx("Mga.MgaProject")
            self.project.Open("MGA=" + _adjacent_file('SFDemo_dup_guids.mga'))

            self.project.BeginTransactionInNewTerr(0)
            mga = self.project
            self.assertTrue(mga.ObjectByPath('/@Folder1').GetGuidDisp() == '{8ce2ca06-2729-4e4c-955f-fc88194782cc}' 
                or mga.ObjectByPath('/@Folder2').GetGuidDisp() == '{8ce2ca06-2729-4e4c-955f-fc88194782cc}')
            self.assertTrue(mga.ObjectByPath('/@Folder1/@System').GetGuidDisp() == '{a57ca6b2-d95e-485c-a768-98c16fd30588}' 
                or mga.ObjectByPath('/@Folder2/@System').GetGuidDisp() == '{a57ca6b2-d95e-485c-a768-98c16fd30588}')
            self.assertNotEqual(mga.ObjectByPath('/@Folder1').GetGuidDisp(), mga.ObjectByPath('/@Folder2').GetGuidDisp())
            self.assertNotEqual(mga.ObjectByPath('/@Folder1/@System').GetGuidDisp(), mga.ObjectByPath('/@Folder2/@System').GetGuidDisp())
            self.assertNotEqual(mga.ObjectByPath('/@Folder1/@System/@DBSetup1').GetGuidDisp(), mga.ObjectByPath('/@Folder2/@System/@DBSetup1').GetGuidDisp())
            self.project.AbortTransaction()
            self.project.Save("MGA=" + _adjacent_file(self.output_file), False)
            self.project.Close(True)


if __name__ == "__main__":
    unittest.main()
