import unittest

class TestPIAs(unittest.TestCase):
    def test_existance(self):
        import os
        import os.path
        self.assertTrue(os.path.isfile(os.path.join(os.environ['SystemRoot'], r'assembly\GAC_MSIL\GME.MGA\1.0.0.0__f240a760fe751c2e\GME.MGA.dll')))
        self.assertTrue(os.path.isfile(os.path.join(os.environ['SystemRoot'], r'assembly\GAC_MSIL\GME.MGA\1.0.1.0__f240a760fe751c2e\GME.MGA.dll')))
        self.assertTrue(os.path.isfile(os.path.join(os.environ['SystemRoot'], r'assembly\GAC_MSIL\GME.MGA.Parser\1.0.0.0__f240a760fe751c2e\GME.MGA.Parser.dll')))
        self.assertTrue(os.path.isfile(os.path.join(os.environ['SystemRoot'], r'assembly\GAC_MSIL\GME.MGA.Parser\1.0.1.0__f240a760fe751c2e\GME.MGA.Parser.dll')))
        self.assertTrue(os.path.isfile(os.path.join(os.environ['SystemRoot'], r'assembly\GAC_MSIL\GME.MGA.Parser\1.1.0.0__f240a760fe751c2e\GME.MGA.Parser.dll')))


if __name__ == "__main__":
        unittest.main()
