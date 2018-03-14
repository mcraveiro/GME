from __future__ import with_statement

import unittest

import platform
if platform.system() == 'Java':
	import os
	import os.path
	import sys
	sys.path.append(os.path.join(os.environ['GME_ROOT'], 'SDK', 'Java', 'Jython'))
	sys.path[0:0] = (os.path.join(os.environ['GME_ROOT'], 'SDK', 'Java', 'gme.jar'),)
	sys.add_package("org.isis.jaut")
	import org.isis.jaut
	if not org.isis.jaut.Apartment.currentApartment():
		org.isis.jaut.Apartment.enter(0)

import GPyUnit.test_PIAs

# it is also possible to run tests like this on Python 2.7:
# python -m unittest GPyUnit.Regr.Mga.tc7

def _test_names():
    _test_names = [
 'test_PIAs',
 'test_registrar',
 'test_gmeoleapp',
 'test_parser',
 'test_copy',
 'test_mga_open',
 'test_registry',
 'test_instances',
 'GME_297.suite',
 'GME_310.suite',
 'GME_371',
 'GME_391',
 'test_MetaInterpreter',
 'Regr.Mga.tc1',
 'test_decorators',
 'test_DsmlGenerator',
 'test_coreTransaction',
]
    if platform.system() != 'Java':
        _test_names += [
        'Regr.Mga.tc2',
        'Regr.Mga.tc3',
        'Regr.Mga.tc5',
        'Regr.Mga.tc6',
        ]
    return _test_names

def _tests():
    # print _test_names()
    # can't find tc2 with \python27\python -m GPyUnit if we don't do this
    if platform.system() != 'Java':
        import GPyUnit.Regr.Mga.tc2
    return unittest.defaultTestLoader.loadTestsFromNames(['GPyUnit.' + test for test in _test_names()])

def run_xmlrunner(output_filename):
    import os.path
    import xmlrunner
    results = []
    with open(output_filename, "w") as output:
        output.write("<testsuites>")
        for test in GPyUnit._tests():
            runner = xmlrunner.XMLTestRunner(output)
            runner.run(test)
        output.write("</testsuites>")

