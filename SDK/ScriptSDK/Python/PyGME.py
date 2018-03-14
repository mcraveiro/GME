# GME Python Component Framework
# (c) 2002-2006 ISIS, Vanderbilt University

import os.path, sys

# detect registration options
register = False
for arg in sys.argv:
    if arg.startswith('-register'):
        register = True
        sarg = arg.split('=')
        registry = len(sarg) == 2 and sarg[1] == 'system' and 2 or 1
        sys.argv.remove(arg)
        break
    
# detect specification file
if len(sys.argv) > 1:
    specPath = len(sys.argv) > 1 and os.path.split(sys.argv[1])[1] or 'component.xml'
    spath = os.path.split(sys.argv[1])
    gpath = os.path.split(sys.argv[0])
    genPath = os.path.splitdrive(sys.argv[1])[0] and spath[0] or \
              spath[0] and os.path.join(gpath[0], spath[0]) or gpath[0]
else:
    specPath = 'component.xml'
    genPath = os.path.split(sys.argv[0])[0]

# initialize specification dictionary
import pythoncom
specDict = {'tooltip' : '', 'iconpath' : '', 'clsid' : str(pythoncom.CreateGuid())}

# parse specification
from xml.dom.minidom import parse, Element
specXML = parse(os.path.join(genPath, specPath))
de = specXML.documentElement
for attr in ('name', 'version', 'type', 'paradigm'):
    specDict[attr] = getattr(de.attributes.get(attr), 'value', None)
    if specDict[attr] is None:
        print "Attribute '%s' is missing from 'component' tag."
    if attr == 'type' and specDict[attr] and specDict[attr] not in ('Interpreter', 'Addon'):
        print "Unrecognized component type.  Must be either 'Interpreter' or 'Addon'."
        specDict[attr] = None
if None in specDict.values():
    print "Component generation terminated due to specification errors."
else:
    # optional specifications
    if specDict['type'] == 'Interpreter':
        for c in de.childNodes:
            if isinstance(c, Element):
                specDict[c.nodeName] = getattr(c.attributes.get('value'), 'value', None)
        
        if specDict.get('iconpath'):
            spath = os.path.split(specDict['iconpath'])
            iconBase = os.path.splitdrive(specDict['iconpath'])[0] and spath[0] or \
                       spath[0] and os.path.join(genPath, spath[0]) or genPath
            specDict['iconpath'] = spath[1]
        else:
            iconBase = ''
    
    # generate component
    from Generator import InterpreterTemplate, AddonTemplate
    f = file(os.path.join(genPath, "%s.py" % specDict['name']), "w")
    f.write((specDict['type'] == 'Interpreter' and InterpreterTemplate or AddonTemplate) % specDict)
    f.close()
    
    # optionally, register component with COM and GME
    if register:
        if genPath not in sys.path:
            sys.path.insert(0, genPath)
        mod = __import__(specDict['name'])
        comp = getattr(mod, specDict['name'])
        comp.RegisterSelf(registry, specDict['type'] == 'Interpreter' and iconBase or '')
