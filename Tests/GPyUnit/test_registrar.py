import unittest
from GPyUnit.util import DispatchEx

class TestRegistrar(unittest.TestCase):
	def test_GetAssociatedParadigmsDisp(self):
		x = DispatchEx("MGA.MgaRegistrar")
		self.assertEqual(list(x.GetAssociatedParadigmsDisp("MGA.Interpreter.MetaInterpreter", 1)), [u'MetaGME'])

#suite = (TestRegistrar(),)

if __name__ == "__main__":
        unittest.main()
