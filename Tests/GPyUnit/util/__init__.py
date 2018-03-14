from __future__ import with_statement

import os
import sys
import unittest
import platform

_opts = type("Options", (object,), {
    'Dispatch_x64': False,
    'Run_SVN': False,
    })

def DispatchEx(progid):
    import win32com.client
    import platform

    CLSCTX_ALL = 23
    CLSCTX_INPROC_SERVER = 1
    CLSCTX_LOCAL_SERVER = 4
    CLSCTX_ACTIVATE_32_BIT_SERVER = 0x40000
    CLSCTX_ACTIVATE_64_BIT_SERVER = 0x80000
    if _opts.Dispatch_x64:
        if '64bit' not in platform.architecture() or progid == "GME.Application":
            return win32com.client.DispatchEx(progid, clsctx=CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_64_BIT_SERVER)
        else:
            return win32com.client.DispatchEx(progid, clsctx=CLSCTX_INPROC_SERVER)
    else:
        # FIXME: does this work with 64bit Jython?
        if platform.system() != 'Java' and '64bit' in platform.architecture():
            return win32com.client.DispatchEx(progid, clsctx=CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER)
        return win32com.client.DispatchEx(progid)


class disable_early_binding(object):
    def __enter__(self):
        import win32com.client.gencache

        self._savedGetClassForCLSID = win32com.client.gencache.GetClassForCLSID
        win32com.client.gencache.GetClassForCLSID = lambda x: None
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        import win32com.client.gencache

        win32com.client.gencache.GetClassForCLSID = self._savedGetClassForCLSID


class NoopUsing(object):
    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        pass

import platform

if platform.system() == 'Java':
    disable_early_binding = NoopUsing

def dec_disable_early_binding(f):
    def ret(*args, **kwargs):
        with disable_early_binding():
            f(*args, **kwargs)

        # n.b. unittest reads __name__
    ret.__name__ = f.__name__
    return ret


def register_xmp(xmpfile):
    import os.path

    predef = {'SF': os.path.join(os.environ['GME_ROOT'], "Paradigms", "SF", "SF.xmp"),
              'MetaGME': os.path.join(os.environ['GME_ROOT'], "Paradigms", "MetaGME", "MetaGME.xmp")}
    if not os.path.isfile(xmpfile):
        xmpfile = predef[xmpfile]
    import gme

    gme.register_if_not_registered(xmpfile)


def parse_xme(connstr, xme=None, project=None):
    testdir = os.path.dirname(os.path.abspath(__file__))
    if xme is None:
        xme = os.environ['GME_ROOT'] + r"\Paradigms\MetaGME\MetaGME-model.xme"
    parser = DispatchEx("Mga.MgaParser")

    if project is None:
        (paradigm, parversion, parguid, basename, ver) = parser.GetXMLInfo(xme)
        import sys
        #sys.stderr.write("xxx'" + paradigm + "'")
        project = DispatchEx("Mga.MgaProject")
        project.Create(connstr, paradigm)
    parser.ParseProject(project, xme)
    return project

# From pathutils by Michael Foord: http://www.voidspace.org.uk/python/pathutils.html
def onerror(func, path, exc_info):
    """
    Error handler for ``shutil.rmtree``.

    If the error is due to an access error (read only file)
    it attempts to add write permission and then retries.

    If the error is for another reason it re-raises the error.

    Usage : ``shutil.rmtree(path, onerror=onerror)``
    """
    import stat

    if not os.access(path, os.W_OK):
        # Is the error an access error ?
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise


class MUTestMixin(
    unittest.TestCase): # need to inherit from TestCase so __mro__ works, since TestCase.__init__ doesn't call super().__init__
    def __init__(self, name, **kwds):
        super(MUTestMixin, self).__init__(name, **kwds)
        import os.path

        self.mgxdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "MUTest")

    def setUp(self):
        super(MUTestMixin, self).setUp()
        import os.path

        if os.path.isdir(self.mgxdir):
            import shutil

            assert len(self.mgxdir) > 10 # sanity check
            shutil.rmtree(self.mgxdir, onerror=onerror)

    @property
    def connstr(self):
        return "MGX=\"" + self.mgxdir + "\""


class MUSVNTestMixin(MUTestMixin):
    def setUp(self):
        super(MUSVNTestMixin, self).setUp()
        svn_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), "MUTestRepo")
        self.svn_url = "file:///" + svn_file
        if os.path.isdir(svn_file):
            import shutil

            assert len(svn_file) > 10 # sanity check
            shutil.rmtree(svn_file, onerror=onerror)
        import subprocess

        subprocess.check_call(['svnadmin', 'create', svn_file])
        with open(os.environ['USERPROFILE'] + '\\GME_MU_config.opt', 'w') as file:
            file.write('''
AutomaticLogin=true
UseAccountInfo=true
AutoCommit=true
account=''' + os.environ['USERNAME'])

    @property
    def connstr(self):
        print 'opts: ' + self.opts()
        return "MGX=\"" + self.mgxdir + "\" svn=\"" + self.svn_url + "\"" + self.opts()


def MUGenerator(module, test):
    # XMLBackend on x64 is unsupported
    if _opts.Dispatch_x64: return
    module[test.__name__ + "MU"] = type(test.__name__ + "MU", (MUTestMixin, test), {})
    if not _opts.Run_SVN: return

    def opts_f(opts):
        return lambda self: opts

    for name, opts in (('MUSVN', ''), ('MUSVNHashed', ' hash=\"true\" hval=\"256\"')):
        module[test.__name__ + name] = type(test.__name__ + name, (MUSVNTestMixin, test), {'opts': opts_f(opts)})


def get_MetaGME_user_reg_data():
    registrar = DispatchEx("Mga.MgaRegistrar")
    try:
        meta_gme_guid = registrar.ParadigmGUIDString(1, "MetaGME")
    except:
        # not registered in user
        return None
    else:
        # get MetaGME ConnStr (can't get it out of registrar with Jython)
        project = DispatchEx("Mga.MgaProject")
        project.Create("MGA=" + "get_MetaGME_user_reg_data.mga", "MetaGME")
        meta_gme_connstr = project.ParadigmConnStr
        project.Close(True)
        return ("MetaGME", meta_gme_connstr, None, meta_gme_guid, 1)

if platform.system() != 'Java':
    from pythoncom import com_error
else:
    import org.isis.jaut.InvokeException
    com_error = org.isis.jaut.InvokeException
