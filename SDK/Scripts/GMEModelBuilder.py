#################################################################################
#
#    This file contains Python helper methods for Model, Set, Reference objects.
#                   See bottom of the file for main()
#
#################################################################################
#
# Helper methods for different object types useable as follows
# ____________________________________________________________
#
# Models:
#        MetaRoleObject = metaRoleByName( Model, KindString)
#        MetaRoleObject = metaRole( Object)
#        MetaNameString = metaName( Object)
#        NewChild = newChild( Model, RoleString)
#        ClonedObject = clone( WhereModel, WhichObject)
#        delObj( Object)
#        ChildObject = childByName( Model, Name)
#        ListOfChildren = childrenByKind( Model, KindString)
#        ListOfChildren = children( Model)
#        Parent = parent( Object)
# Connections:
#        NewConn = connect( Model, Source, Destination, ConnectionMetaRole)
#        NewConn = connectThruRefChain( Model, Source, Destination, SourceReferenceChain, DestinationReferenceChain, ConnectionMetaRole)
#        ListOfConns = partOfConns( Object)
# References:
#        Tgt = getReferred( Ref)
#        setReferred( Ref, Tgt)
#        clearReference( Ref)
#        ListOfRefs = referredBy( Tgt)
# Sets:
#        T/F = isMemberIn( Set, Member)
#        ListOfMembers = members( Set)
#        ListOfSets = memberOfSets( Member)
#        addToSet( Set, Member)
#        remFromSet( Set, Member)
#
#################################################################################

import win32com.client

p = project                                                 # pre-existing variable defined, implements IMgaProject interface (see Interfaces/Mga.idl)
g = gme                                                     # pre-existing variable defined, implements IGMEOLEApp interface  (see Interfaces/GME.idl)



#############################################################
## OBJTYPE_REFERENCE (3)
#############################################################
def getReferred( r):
    if( r and r.ObjType == 3):
        return r.Referred
    return

def setReferred( r, o):
    if( r and r.ObjType == 3):
        r.Referred = o
    return

def clearReference( r):
    """ Clears a reference's Referred property.
        r.Referred = 0 does not work unfortunately
    """
    if r.ObjType == 3:
        try:
            r.ClearRef()                 # from GME8 on
        except:
            cout( "Exception while clearing reference: " + r.Name + "!", 3)
            raise
    return
    
def referredBy( o):
    return list( o.ReferencedBy)

#############################################################
## OBJTYPE_SET (5)
#############################################################

def isMemberIn( s, o):
    # GetIsMemberDisp is not giving back correct results:
    #return s.ObjType == 5 and s.GetIsMemberDisp( o)
    # that's why an alternative implementation is used:
    #s.GetIsMemberDisp( o)
    return o in members(s)

def members( s):
    if( s and s.ObjType == 5):
        return list( s.Members)
    return

def memberOfSets( o):
    return list(o.MemberOfSets)

def addToSet( s, o):
    if( s and s.ObjType == 5):
        s.AddMember( o)


def remFromSet( s, o):
    if( s and s.ObjType == 5):
        s.RemoveMember( o)

#############################################################
## OBJTYPE_CONNECTION
#############################################################

def connect( m, srco, dsto, conn_mrole):
    if( m.ObjType == 1):        # Model
    	c = m.CreateSimpleConn( conn_mrole, srco, dsto, None, None)
    	c.Name = conn_mrole.Name
    	return c
    return

def connectThruRefChain( m, srco, dsto, src_ref_chain, dst_ref_chain, conn_mrole):
    if( m.ObjType == 1):        # Model
    	c = m.CreateSimpleConn( conn_mrole, srco, dsto, src_ref_chain, dst_ref_chain)
    	c.Name = conn_mrole.Name
    	return c
    return

def partOfConns( o):
    conn_points = list( o.PartOfConns)
    conns = []
    for i in range(0, len(conn_points)):
    	conns.append( conn_points[i].Owner)
    return conns

#############################################################
## OBJTYPE_MODEL (1)
#############################################################

def metaRoleByName( m, kind_str):
    if m.ObjType == 1:             # Model
    	return m.Meta.GetRoleByNameDisp( kind_str)
    #return null

def metaRole( o):
    return o.MetaRole

def metaName( o):
    return o.Meta.Name

def newChild( m, role_str):
    if m.ObjType == 1:
    	mrole = metaRoleByName( m, role_str)
    	if mrole:
    	    nch = m.CreateChildObject( mrole)
    	    nch.Name = mrole.Name
    	    return nch
    return

def clone( m, orig):
    """ Clones orig object into m (model or folder).
        If orig is a folder, m needs to a folder too.
    """
    if m.ObjType not in (1, 6): return
    if not orig:                return
    
    if m.ObjType == 6:                                            # Target is a Folder
        if orig.ObjType == 6: cloned = m.CopyFolderDisp( orig)    # Orig is Folder too
        else:                 cloned = m.CopyFCODisp( orig)       # Orig is FCO
    elif m.ObjType == 1:
        cloned = m.CopyFCODisp( orig, metaRole( orig))            # Target is Model, Orig is FCO
    
    if cloned:
    	cloned.Name = "Cloned" + orig.Name
    return cloned
    
def delObj( o):
    """ Deletes an object o from its parent
    """
    if not o: return
    try:
        o.DestroyObject()
    except:
        cout( "Could not remove object '" + o.Name + "'!")
        raise                                                            # raise again the same exception
    return

def childByName( m, name_str):
    if m.ObjType == 1:
    	return m.GetChildFCODisp( name_str)
    return

def childrenByKind( m, kind_str):
    if m.ObjType == 1:
        return list(m.GetChildrenOfKind( kind_str))
    return

def children( m):
    if m.ObjType == 1:
    	return list(m.ChildFCOs)
    return

def parent( o):
    if   o.ParentFolder                   :    return o.ParentFolder     # by checking ParentFolder first, we can use the same 
    elif o.ObjType != 6 and o.ParentModel :    return o.ParentModel      # implementation for folders and fcos, since folders
                                                                         # have too a method named ParentFolder
    return

#######################################################################
# transaction handlers
#######################################################################

def begin( p_project ):
    # alt1: return project.BeginTransactionInNewTerr()   # only in GME8
    # alt2: project.BeginTransaction( project.CreateTerritory( None, None, None))
    #   or more verbosely:
    terr = p_project.CreateTerritory( None, None, None)
    p_project.BeginTransaction( terr)
    return terr

def commit( p_project, p_terr ):
    p_project.CommitTransaction()

    if p_terr:
       p_terr.Destroy()

def abort( p_project, p_terr ):
    p_project.AbortTransaction()

    if p_terr:
       p_terr.Destroy()


#######################################################################
# general helpers
#######################################################################

def cout3( _gme, _msg, _type = 1):
    _gme.ConsoleMessage( _msg, _type)

def cout( _msg, _type = 1):
    gme.ConsoleMessage( _msg, _type)

def fancy( _inStr):
    """ Encloses incoming string into a html table cell, so 
        that it will be shown with custom fgcolor and bgcolor
    """
    return '<head><style type="text/css">td.special{ background-color:aqua;font-size: 100%;margin-left: 20px;font-family: times, sans-serif, arial}</style></head><table><tr><td class="special">' + _inStr + '</td></tr></table>'

def fancy2( _inStr):
    return '<p style="background-color: yellow">' + _inStr + '</p>'

def makeLink( o):
    return '<a href=mga:' + o.ID + '">' + o.Name + '</a>' 

#######################################################################
# a sorter the traverser will use
#######################################################################

def IDSort( x, y):
    if( x.ID < y.ID):   r = -1
    elif( x.ID > y.ID): r = 1
    else:               r = 0
    return r


#######################################################################
# a generic handler for a visited object (fco or folder)
# implement/change it the way you like it
#######################################################################

def handle( _obj):
    try: 
        nc = 0
        if _obj.ObjType == 1:     # a model
            paradigm = p.MetaName

            if paradigm == "SF":
                """
                     in SF models we can create InputSignals in all containers
                """
                nc = newChild( _obj, "InputSignals")
                if nc: nc.Name = "this is a new creation"

            elif paradigm == "MetaGME":
                """
                     in MetaGME models we can create Atoms in all containers
                """
                nc = newChild( _obj, "Atom")
                if nc: nc.Name = "this_atom_is_a_new_creation"

            else:

                """
                    in unknown models we take the first child of the
                    current model and clone it based on its metarole
                """

                chldrn = children( _obj)
                if len( chldrn) > 0:
                    # select the first child to be cloned (except connections)
                    i = 0
                    while chldrn[i].ObjType == 4 and i < len( chldrn):
                        ++i
                    if i < len( chldrn):    # since a connection can't just exist by itself in a model there
                        Orig = chldrn[i]    # must exist at least one 'real' fco (the endpoint of the connection)

                    # we have an suitable element to be cloned in Orig
                    if Orig and metaRole( Orig):
                        nc = newChild( _obj, metaRole( Orig).Name)
                        if nc: nc.Name = "this is a new creation based on " + Orig.Name + "'s role"

        # nc holds a new child (if any)
        if nc: 
            cout("Created " + makeLink( nc))
    finally:
        pass

    return    

#######################################################################
# traverse
#######################################################################

def traverse_levelled( _rootfolder, _comparator):            # aka BFT, Breadth First Traversal with a fifo queue
    to_visit = []                                   # a list (!tuple)
    to_visit.append( _rootfolder)
    
    while len( to_visit) > 0:
        o = to_visit.pop(0)                         # pops the first element from the queue
        handle( o)                                  # handle current node
        
        if o.ObjType in (1, 6):                     # children for Models and Folders
            children = list( o.ChildObjects)        # only a list
            children.sort( cmp = _comparator)       # can be sorted
            to_visit.extend( children)              # append children to the fifo queue
    
    return

def traverse_postorder( _current, _comparator):
    to_visit = []                                   # a list (!tuple)

    if _current.ObjType in (1, 6):                  # children for Models and Folders
        to_visit.extend( _current.ChildObjects)     # remains a list
        to_visit.sort( cmp = _comparator)           # which can sort

    # 1st: traverse subtrees
    for o in to_visit:
        traverse_postorder( o, _comparator)

    # 2nd: handle current node
    handle( _current)

    return

##############################################################################################
#
#   Variables existing when the script is executed are
#   -----------------------------------------------------
#
#   project : the project variable (see Interfaces/Mga.idl)
#   gme     : the main application window (see Interfaces/GME.idl for IGMEOLEApp)
#   it      : the actively shown model (view) (see Interfaces/GME.idl for IGMEOLEIt)
#
###############################################################################################
# main() begins
#

cout( '--Hello World, this is Python here!--', 1)

terr = begin( p)                              # begin transaction (strictly needed to perform r/w operations on a IMgaProject)
try:

    # do a traversal of the active model (if valid), otherwise of the whole project
    if it.Valid: traverse_postorder( it.MgaModel,  IDSort)  
    else:        traverse_postorder( p.RootFolder, IDSort)

    #traverse_levelled( p.RootFolder, IDSort)  # do a traversal
    
    commit(p, terr)                           # commit transaction (strictly needed to return the project in a consistent mode)
except:
    cout( 'Need to abort', 2)                 # 2 means MSG_WARNING (yellow)
    abort(p, terr)                            # abort transaction if something went wrong

cout( '--End of Script--', 1)

p = None
g = None
