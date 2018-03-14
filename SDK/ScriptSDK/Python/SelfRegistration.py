# GME Python Component Framework
# (c) 2002-2006 ISIS, Vanderbilt University

import win32com.server.register
import win32com.client
import pythoncom
import os.path

def registerAsCOMComponent(component, icon_path="", debugging=None):
    try:
        win32com.client.Dispatch(component._reg_progid_)
    except pythoncom.com_error, e:
        pass
    else:
        print "Already registered. Cleanup first."
        unregisterAsCOMComponent(component)

    print "Registering as a COM component...",

    if debugging == 1:
        dispatcherSpec = "win32com.server.dispatcher.DispatcherWin32trace"
        options = {'Debugging':'1'}
    else:
        dispatcherSpec = None
        options = {'Debugging':'0'}
    try:
        win32com.server.register.RegisterServer (clsid = component._reg_clsid_
                                                ,pythonInstString = component._reg_class_spec_
                                                ,desc = component._reg_desc_
                                                ,progID = component._reg_progid_
                                                ,verProgID = component._reg_verprogid_
                                                ,defIcon = hasattr(component, '_reg_iconfile_') and os.path.join(icon_path, component._reg_iconfile_) or None
                                                ,other = options
                                                ,dispatcher = dispatcherSpec
                                                )
    except Exception, e:
        raise Exception("Error while registering component:\n%s" % e)
    print "Done"

def unregisterAsCOMComponent(component):
    print "Unregistering the COM component...",
    try:
        win32com.server.register.UnregisterServer(clsid=component._reg_clsid_
                                                  ,progID=component._reg_progid_
                                                  ,verProgID=component._reg_verprogid_
                                                  )
        print "Done"                    
    except Exception, e:
        raise Exception("Error while unregistering component:\n" % e)
  
def registerAsGMEComponent(component, systemwide, icon_path="", type=None):
    registrar = win32com.client.Dispatch("Mga.MgaRegistrar")

    try:
        registrar.QueryComponent(component._reg_progid_, None, None, systemwide)
    except pythoncom.com_error, (hr, desc, exc, argErr) :
        if exc[1] == 'GME':
            pass
        else:
            raise
    else:
        print "Already registered. Cleanup first."
        unregisterAsGMEComponent(component, systemwide)

    print "Registering as a GME Component...",
    if type is None:
        type = component.mga.constants.COMPONENTTYPE_INTERPRETER
    registrar.RegisterComponent(component._reg_progid_,
                                type,
                                component._reg_desc_,
                                systemwide)
    registrar.Associate(progid = component._reg_progid_,
                        paradigm = component._paradigm_,
                        mode = systemwide)
    if hasattr(component,'_reg_iconfile_'):
        registrar.SetComponentExtraInfo(mode = systemwide,
                                        progid = component._reg_progid_,
                                        name = 'Icon',
                                        arg3 = os.path.join(icon_path, component._reg_iconfile_))
    if hasattr(component,'_tooltip_'):
        registrar.SetComponentExtraInfo(mode = systemwide,
                                        progid = component._reg_progid_,
                                        name = 'Tooltip',
                                        arg3 = component._tooltip_)
    print "Done"

def unregisterAsGMEComponent(component, systemwide):
    print "Unregistering the GME Interpreter...",
    registrar = win32com.client.Dispatch("Mga.MgaRegistrar")
    registrar.Disassociate(progid = component._reg_progid_,
                           paradigm = component._paradigm_,
                           mode = systemwide)

    registrar.UnregisterComponent(progid = component._reg_progid_,
                                  mode = systemwide)
    print "Done"

