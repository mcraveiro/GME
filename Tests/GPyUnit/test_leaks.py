import unittest
import os
import re

def _adjacent_file(file):
    import os.path
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), file)

class TestLeaks(unittest.TestCase):
    def test_Leaks(self):
        import subprocess
        def check_output_if_failed(*args, **kwargs):
            kwargs['stdout'] = subprocess.PIPE
            kwargs['stderr'] = subprocess.PIPE
            sub = subprocess.Popen(*args, **kwargs)
            (stdout, stderr) = sub.communicate()
            if sub.returncode != 0:
                print stdout
                print stderr
                raise subprocess.CalledProcessError(sub.returncode, args[0])
        check_output_if_failed([r'c:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe',
            _adjacent_file(r'Leaks\TestGMELeaks\TestGMELeaks.vcxproj'), '/p:Configuration=Release'])
        check_output_if_failed([r'\Program Files (x86)\Debugging Tools for Windows (x86)\gflags.exe'] +
            '-i TestGMELeaks.exe +ust'.split())
        umdh = r"c:\Program Files (x86)\Debugging Tools for Windows (x86)\umdh.exe"

        os.environ['OANOCACHE'] = '1'
        os.environ['_NT_SYMBOL_PATH'] = r'c:\Windows\symbols\dll;srv*{}\SymbolCache*https://msdl.microsoft.com/download/symbols'.format(os.environ['TEMP'])

        leak_sub = subprocess.Popen([_adjacent_file(r'Leaks\TestGMELeaks\Release\TestGMELeaks.exe')], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        print leak_sub.stdout.readline()
        check_output_if_failed([umdh] + '-pn:TestGMELeaks.exe -f:LeakDump1.txt'.split())
        leak_sub.stdin.write('\n')

        print leak_sub.stdout.readline()
        check_output_if_failed([umdh] + '-pn:TestGMELeaks.exe -f:LeakDump2.txt'.split())
        leak_sub.stdin.write('\n')

        check_output_if_failed([umdh] + 'LeakDump1.txt LeakDump2.txt -f:LeakDiff.txt'.split())

        leak_sub.wait()
        self.assertEqual(0, leak_sub.returncode)

        lines = open('LeakDiff.txt', 'rb').readlines()
        # Total decrease ==      0 requested +      0 overhead =      0
        self.assertTrue([line for line in lines[-3:] if line.find('Total decrease ==') != -1], lines[-3:])

        # self.assertEqual([], [line for line in lines if re.search('(MSVCR\\d+!malloc\\+|MSVCR\\d+!operator new)', line)])

if __name__ == "__main__":
        unittest.main()
