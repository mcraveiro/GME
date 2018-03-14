"""
Helper methods for developing Python testcases for GME
"""
from GPyUnit.util import DispatchEx
import os

regNodePositionStartTag = '<regnode name="PartRegs"'
regNodeEndTag = '</regnode>'
regNodeStartTag = '<regnode'

def xmeFileComp( fn1, fn2, p_skipPositionRegnodes = True, p_skipLines = range(4,5) ):
        """Compares two xme (xml) files line by line. It may disregard position information (x, y coordinates)
        of fcos in models if speficied by the 3rd parameter. It may skip the line range specified in the 4th
        parameter->default value skips the line containing the GUIDs
        
        Returns the number of different lines.
        """
        
        lineDiff = lambda o,t: (o == t and [0] or [1])[0]
        try:
                f1 = file( fn1)
        except IOError:
                print 'gmeUtils::xmeFileComp >> Could not open', fn1
                return -1

        try:
                f2 = file( fn2)
        except IOError:
                print 'gmeUtils::xmeFileComp >> Could not open', fn2
                return -1

        #if not f1 or not f2:
        #    raise 'Could not open one of the files'

        go_on = True
        nDiff = 0
        nLine = 0
        while go_on:
                s1 = f1.readline()
                s2 = f2.readline()
                nLine += 1
                if nLine not in p_skipLines:
                        if p_skipPositionRegnodes:
                            if regNodePositionStartTag in s1:
                               s1 = nextImportantLine( s1, f1 )
                            
                            if regNodePositionStartTag in s2:
                               s2 = nextImportantLine( s2, f2 )

                        nDiff += lineDiff( s1, s2)
                        
                # eof test
                go_on = len(s1) > 0 and len(s2) > 0

        f1.close()
        f2.close()
        return nDiff

def nextImportantLine( s1, f1 ):
    """Helper method of xmlFileComp. Inside <regnode> elements another <regnode>s may exist. 
    That is why if a position storing <regnode> is met, we skip all the lines till the
    enclosing tag is found (skipping over several other regnodes).
    """

    n1 = f1.readline()
    nInnerRegs = 0
    go_on_till_endtag = True
    while f1 and go_on_till_endtag:
        if regNodeEndTag in n1 and nInnerRegs <= 0:
            go_on_till_endtag = False
            break
        elif regNodeEndTag in n1:
            nInnerRegs -= 1
        elif regNodeStartTag in n1:
            nInnerRegs += 1
        else:
            pass
        n1 = f1.readline()
    s1 = f1.readline()
    return s1



def mga2xme( mganame, xmename = ''):
        """Given an mga file, it exports to xml format using the GME.Application COM object
        When the method exits the GME.Application object remains alive, so upon the next
        invokation the same object will be used. Beware if you make testcases involving
        the GME.Application object, leave the object in a state regarding that this method 
        tries to open a project.
        """
        if xmename == '':
            xmename = mganame + '.xme'

        try:
                gme = DispatchEx("GME.Application")
        except:
                raise 'Could not create GME.Application'

        try:
                gme.OpenProject( "MGA=" + mganame )
                
                gme.ExportProject( xmename )
                gme.CloseProject(0)
                return xmename
        except:
                gme.CloseProject(0)


def findInProj( project, obj_name = "", obj_kind = ""):
        """Helper method, to find an object in the hierarchy. You may find it useful it in your testcases.
        """

        # create a filter
        filter = project.CreateFilter()
        
        # use Name filter
        filter.Name = obj_name
        filter.Kind = obj_kind

        try:
                some_fcos = list( project.AllFCOs( filter ))
                if len( some_fcos) > 0:
                        return some_fcos[0]
                else:
                        print "findInProj >> Object not found : name = '" + obj_name + "' kind = '" + obj_kind + "'"
                        assert 0
        except:
                print "findInProj >> Exception : name = '" + obj_name + "' kind = '" + obj_kind + "'"
                assert 0

        pass

