# GME Python Component Framework
# (c) 2002-2006 ISIS, Vanderbilt University

import os.path, time

import win32com
from win32com.client import gencache
import winerror

gme_connection_string_prefix='MGA='

class GMEComComponent(object):

    # The following properties should be set for every GME interpreter
    # ----------------------------------------------------------------

    # Component specific parts for COM registration (you must change these values)
    _comname_ = "MyComponent"
    _comp_version_ = "1.0"
    _reg_clsid_ = "{2F841BDF-5EB3-4041-A66A-58A3B9EC3EA1}"
    
    # Boilerplate code for COM registration (same for all component, just copy & paste)
    _reg_desc_ = _comname_
    _reg_progid_ = "MGA.PythonInterpreter.%s" % _comname_
    _reg_verprogid_ = "MGA.PythonInterpreter.%s.%s" % (_comname_, _comp_version_)
    _reg_class_spec_ = "%s.%s" % (__module__, _comname_)

    try:
        mga  = gencache.EnsureModule("{270B4F86-B17C-11D3-9AD1-00AA00B6FE26}", 0, 1, 0)
        meta = gencache.EnsureModule("{0ADEEC71-D83A-11D3-B36B-005004D38590}", 0, 1, 0)
    except Exception,e:
        raise Exception("An error occurred during GME Python component initialization.\n%s" % e)
     
    # Component specific parts for GME registration (uncomment and/or change)
    _component_type_ = mga.constants.COMPONENTTYPE_INTERPRETER
    #_reg_iconfile_ = "iconfile.ico"
    #_tooltip_ = "Python GME Component"
    _paradigm_ = 'MyParadigm'

    # ----------------------------------------------------------------
    # Boilerplate code for the invokes (same for all component, get them by inheritance)
    # ----------------------------------------------------------------
    componentParameters = {}
    InteractiveMode = 1
    _public_methods_ = ['Enable', 'Initialize', 'Invoke', 'GetComponentParameter', 'SetComponentParameter', 'ObjectsInvokeEx', 'InvokeEx']
    _public_attrs_ = ['InteractiveMode','ComponentName', 'ComponentType', 'Paradigm', 'ComponentProgID', 'Version']
    # ----------------------------------------------------------------

    def __init__(self):
        try:
            self.ComponentName = self._reg_desc_ 
            self.ComponentProgID = self._reg_progid_
            self.ComponentType = self._component_type_
            self.Paradigm = self._paradigm_
            mgautil = gencache.EnsureModule("{461F30AE-3BF0-11D4-B3F0-005004D38590}", 0, 1, 0)
            self.Version = mgautil.constants.MgaInterfaceVersion_Current

        except Exception,e:
            raise Exception("An error occurred during GME Python component initialization.\n%s" % e)
        return None

    def Enable(self, b):
        pass

    def Initialize(self, project):
        pass
        
    def Invoke(self, project, sel, param):
        pass

    def GetComponentParameter(self, paramname):
        if paramname in self.componentParameters:
            return self.componentParameters[paramname]
        else:
            return winerror.S_OK

    def Logger(self, message, msgtype=1):
        # see GME.idl 'msgtype_enum' for definition of msgtype codes
        # message is (oddly enough) HTML
        if message:
            try:
                message = "%s %s" % (time.asctime(), message)
                client = self.project.GetClientByName("GME.Application")
                gme = client.OLEServer
                gme.ConsoleMessage(message, msgtype)
                import win32gui
                win32gui.PumpWaitingMessages()
            except:
                pass

    def SetComponentParameter(self, paramname, newval):
        self.componentParameters[paramname] = newval

    def ObjectsInvokeEx(self, project, currentobj, selectedobjs, param):
        pass

    def InvokeEx(self, project, currentobj=None, selectedobjs=None, param=None):
        try:
            self.project = self.mga.IMgaProject(project)
            self.project_path = os.path.dirname(str(self.project.ProjectConnStr[len(gme_connection_string_prefix):]))
        except Exception, e:
            raise Exception("An error occurred during component execution.\n%s" % e)
        return winerror.S_OK

    def RegisterSelf(self, systemwide, icon_path=""):
        import SelfRegistration
        print '\n%s\n%s' % (self._reg_progid_, '-'*32)
        SelfRegistration.registerAsCOMComponent(self, icon_path)
        SelfRegistration.registerAsGMEComponent(self, systemwide, icon_path, self._component_type_)

    RegisterSelf = classmethod(RegisterSelf)
    
    def UnregisterSelf(self, systemwide):
        import SelfRegistration
        print '\n%s\n%s' % (self._reg_progid_, '-'*32)
        SelfRegistration.unregisterAsCOMComponent(self)
        SelfRegistration.unregisterAsGMEComponent(self, systemwide)

    UnregisterSelf=classmethod(UnregisterSelf)
