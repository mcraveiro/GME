# tests for GME-311 and GME-297
from __future__ import with_statement

import sys
import os.path
import unittest
from GPyUnit.util import DispatchEx

class TestRefportConnectionInvariantUnderMoves(unittest.TestCase):
    def __init__(self, input_file, fco_to_move, destination_model, name=None, use_disp=None, **kwds):
        super(TestRefportConnectionInvariantUnderMoves, self).__init__('test', **kwds)
        self.input_file = input_file
        self.fco_to_move = fco_to_move
        self.destination_model = destination_model
        name = name if name else os.path.splitext(self.input_file)[0]
        self._testMethodDoc = name
        self.output_file = name + "-output.mga"
        self.correct_file = name + "-correct.mga"
        if use_disp:
            self._move_fcos = self._move_fcos_disp


    def test(self):
        """
        Regression test: given self.input_file, move self.fco_to_move to self.destination_model. Then check self.output_file against self.correct_file
        """
        def _adjacent_file(file):
            import os.path
            return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)
        from GPyUnit import util
        util.register_xmp(_adjacent_file('GME297ModelRefportTest.xmp'))
        with util.disable_early_binding():
            self.project = DispatchEx("Mga.MgaProject")
            self.project.Open("MGA=" + _adjacent_file(self.input_file))
            self.territory = self.project.BeginTransactionInNewTerr()

            fco_to_move = self.project.ObjectByPath(self.fco_to_move)
            OBJTYPE_FOLDER = 6
            if fco_to_move.ObjType == OBJTYPE_FOLDER:
                tomove = DispatchEx("Mga.MgaFolders")
            else:
                tomove = DispatchEx("Mga.MgaFCOs")
            tomove.Append(fco_to_move)

            destination = self.project.ObjectByPath(self.destination_model)
            if destination.ObjType == OBJTYPE_FOLDER:
                destination.MoveFolderDisp(fco_to_move)
            else:
                self._move_fcos(destination, fco_to_move, tomove)
                #destination.MoveFCOs(tomove, None, None)

            self.project.CommitTransaction()
            self.project.Save("MGA=" + _adjacent_file(self.output_file))
            self.territory.Destroy()
            self.project.Close()

        import GPyUnit.util.mgadiff as mgadiff
        if not mgadiff.compare(_adjacent_file(self.correct_file), _adjacent_file(self.output_file)):
            self.fail("Reference file '%s' does not match output '%s'" % (self.correct_file, self.output_file))
        # print "Reference file '%s' matches output '%s'" % (self.correct_file, self.output_file)
    
    def _move_fcos(self, destination, fco_to_move, col_to_move):
        import platform
        if platform.system() == 'Java':
            import org.isis.jaut.Variant
            destination.MoveFCODisp(fco_to_move, org.isis.jaut.Variant.create(org.isis.jaut.Variant.VT_UNKNOWN))
        else:
            destination.MoveFCOs(col_to_move, None, None)

    def _move_fcos_disp(self, destination, fco_to_move, col_to_move):
        import platform
        if platform.system() == 'Java':
            import org.isis.jaut.Variant
            destination.MoveFCODisp(fco_to_move, org.isis.jaut.Variant.create(org.isis.jaut.Variant.VT_UNKNOWN))
        else:
            destination.MoveFCODisp(fco_to_move, None)

def suite():
    suite = unittest.TestSuite()
    suite.addTest(TestRefportConnectionInvariantUnderMoves(input_file="test1.mga", fco_to_move="/Test1/Folder1/A/B", destination_model="/Test1/Folder2/C"))
    suite.addTest(TestRefportConnectionInvariantUnderMoves(input_file="test2.mga", fco_to_move="/Test2/Subtypes/A/BSubtypeRef", destination_model="/Test2/Destination/Destination"))
    suite.addTest(TestRefportConnectionInvariantUnderMoves(input_file="test1.mga", fco_to_move="/Test1/Folder1/A/RefB", destination_model="/Test1/Folder2/C", name="test3"))
    suite.addTest(TestRefportConnectionInvariantUnderMoves(input_file="test4.mga", fco_to_move="/Test4/Folder1/A/RefRefB", destination_model="/Test4/Folder2/C"))
    suite.addTest(TestRefportConnectionInvariantUnderMoves(input_file="test5.mga", fco_to_move="/Test4/Folder2", destination_model="/Test4/Folder3"))
    suite.addTest(TestRefportConnectionInvariantUnderMoves(input_file="test4.mga", fco_to_move="/Test4/Folder1/A/RefRefB", destination_model="/Test4/Folder2/C", name="test6", use_disp=True))
    return suite

if __name__ == "__main__":
    runner = unittest.TextTestRunner()
    runner.run(suite())
