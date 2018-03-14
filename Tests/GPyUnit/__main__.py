from __future__ import with_statement


import GPyUnit
import unittest

# For debugging
#import sys
#sys.stdin.readline()

if __name__=='__main__':
    import optparse
    parser = optparse.OptionParser()
    parser.add_option("-x", "--xml", action="store_true", dest="xml")
    parser.add_option("-v", action="store_true", dest="verbose")
    parser.add_option('-a', '--arch', dest='arch')
    parser.add_option('-o', '--output', dest='output')

    (options, args) = parser.parse_args()
    import GPyUnit.util
    if options.arch and options.arch == 'x64':
        GPyUnit.util._opts.Dispatch_x64 = True
    if options.xml:
        GPyUnit.run_xmlrunner(options.output or 'tests.xml')
    else:
        runner = unittest.TextTestRunner(verbosity=2 if options.verbose else 1)
        runner.run(GPyUnit._tests())
