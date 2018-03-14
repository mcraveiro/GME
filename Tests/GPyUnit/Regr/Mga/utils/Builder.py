import unittest
import win32com.client
import os
from GPyUnit.util import DispatchEx

def findInProj( project, obj_name = "", obj_kind = ""):
    """ Returns an object in project, satisfying the obj_name and obj_kind criteria, if speficied 
    """
    
    # create a filter
    filter = project.CreateFilter()
    
    # use Name filter
    filter.Name = obj_name
    filter.Kind = obj_kind

    try:
        some_fcos = project.AllFCOs( filter )
        if some_fcos.Count > 0:
            return some_fcos.Item( 1 )
        else:
            print "findInProj >> Object not found : name = '" + obj_name + "' kind = '" + obj_kind + "'"
            assert 0
    except:    
        print "findInProj >> Exception : name = '" + obj_name + "' kind = '" + obj_kind + "'"
        raise

    pass

def folder( project, kind):
    """ Returns an IMgaMetaFolder pointer, based on the folder kind given """
    metaproj = project.RootMeta        # an IMgaMetaProject
    metaroot = metaproj.RootFolder        # an IMgaMetaFolder
    
    #assert project.RootFolder.MetaFolder == metaroot
    
    mfol = metaroot.LegalChildFolderByName(kind)
    return mfol

def kind(project, kind):
    """ Returns an IMgaMetaFCO pointer, based on the kind given """
    metaproj = project.RootMeta        # an IMgaMetaProject
    metaroot = metaproj.RootFolder        # an IMgaMetaFolder
    
    #assert project.RootFolder.MetaFolder == metaroot
    
    mfco = metaroot.DefinedFCOByName(kind, 1)
    return mfco

def role(project, model, role_str):
    """ Returns an IMgaMetaRole pointer, based on role_str and the container model (IMgaFCO) given """
    try:
        metaproj = project.RootMeta        # an IMgaMetaProject
        metaroot = metaproj.RootFolder        # an IMgaMetaFolder

        metacont = kind(project, model.MetaBase.Name)
        metarole = metacont.RoleByName(role_str)

    except:
        print 'No such kind:', role_str, 'in', model
        raise
    return metarole


def new(p, cont, role_str):
    """ Creates an fco, in role_str role in container model specified """
    return cont.CreateChildObject( role(p, cont, role_str))

def subtype(p, cont, base):
    """ Creates a derived fco from base, in the same role as base in container model specified """
    return cont.DeriveChildObject( base, base.MetaRole, False)

def instantiate(p, cont, type):
    """ Instantiates an fco from type, in the same role as type in container model specified """
    return cont.DeriveChildObject( type, type.MetaRole, True)

def newObjInFold(p, fold, kind_str):
    """ Creates an object in a folder/rootfolder, based on kind_str"""
    return fold.CreateRootObject( kind( p, kind_str))

def subtypeInFold(p, cont, base):
    """ Creates a derived fco from base, in the folder specified """
    return cont.DeriveRootObject( base, False)

def instantiateInFold(p, cont, type):
    """ Instantiates an fco from type, in the folder specified """
    return cont.DeriveRootObject( type, True)

def newFolder(p, parent, folder_kind_str):
    """ Creates a folder_kind_str folder in parent"""
    return parent.CreateFolder( folder( p, folder_kind_str))


def connect(p, cont, s, d, role_str):
    """ Helper method connecting plain fcos/ports, when no references are involved
    """
    z0 = DispatchEx("Mga.MgaFCOs")
    return cont.CreateSimpleConn( role(p, cont, role_str), s, d, z0, z0)

def connectRefP(p, cont, s, d, r1, r2, role_str):
    """ Helper method connecting ports.
    s: source fco/port
    d: destination fco/port
    r1: modelreference, 'containing' s
    r2: modelreference, 'containing' d
    r1 or r2 might be 0, when that end of the connection is an fco or modelport (no reference involved)
    """
    z1 = DispatchEx("Mga.MgaFCOs")
    z2 = DispatchEx("Mga.MgaFCOs")
    if r1:
        z1.Append( r1)
    if r2:
        z2.Append( r2)
    return cont.CreateSimpleConn( role(p, cont, role_str), s, d, z1, z2)


def creaP(mganame, parad):
    from GPyUnit import util
    util.register_xmp(parad)
    project = DispatchEx("Mga.MgaProject")

    # may delete old file if exists
    # if os.path.isfile( mganame):
    #    os.remove( mganame)

    project.Create( "MGA=" + mganame, parad)
    project.BeginTransactionInNewTerr(0)
    return project

def saveP(project):
    try:
        project.CommitTransaction()
    except:
        project.AbortTransaction()
    project.Save()
    project.Close(0)

def populate(p):
    """Sample: The way the tester could build up a model needed for a test
    """
    folder1 = newFolder( p, p.RootFolder, 'SheetFolder')
    folder1.Name = 'SheetFolder1'
    
    parsh1 = newObjInFold( p, folder1, 'ParadigmSheet')
    parsh1.Name = 'ParadigmSheet1'
    
    at1 = new( p, parsh1, 'Atom')
    at1.Name = 'Atom1'

    at2 = new( p, parsh1, 'Atom')
    at2.Name = 'Atom2'
    
    ap1 = new( p, parsh1, 'AtomProxy')
    ap1.Name = 'AtProx1'

    ap2 = new( p, parsh1, 'AtomProxy')
    ap2.Name = 'AtProx2'
    
    ap1.Referred = at1
    ap2.Referred = at2
