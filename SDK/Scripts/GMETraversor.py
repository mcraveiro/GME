#################################################################################
#
#        This file contains Python methods for project-wide traversal.
#     In addition it has helper methods for Model, Set, Reference objects.
#                   See bottom of the file for main()
#
#################################################################################
#
#  Visitation according to the following graph traversals algorithms
#  _________________________________________________________________
#
#  Breath First method: Levelled
#  Depth First methods: PreOrder, InOrder, PostOrder
#  method names: traverse_levelled, traverse_preorder, traverse_inorder, traverse_postorder
#
#  Children can be sorted during each traversal by:
#        NameSort, IDSort, GUIDSort, AspectHorizSort, AspectVerticSort, AspectAbsSort
#
#
#################################################################################

import win32com.client

p = project                                                 # pre-existing variable defined, implements IMgaProject interface (see Interfaces/Mga.idl)
g = gme                                                     # pre-existing variable defined, implements IGMEOLEApp interface  (see Interfaces/GME.idl)



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
# sorters
#######################################################################

def IDSort( x, y):
    if( x.ID < y.ID):   r = -1
    elif( x.ID > y.ID): r = 1
    else:               r = 0
    return r

def GUIDSort( x, y):
    if( x.GetGUIDDisp() < y.GetGUIDDisp()):   r = -1
    elif( x.GetGUIDDisp() > y.GetGUIDDisp()): r = 1
    else:                                     r = 0
    return r

def NameSort( x, y):
    if( x.Name < y.Name):   r = -1
    elif( x.Name > y.Name): r = 1
    else:                   r = 0
    return r

#######################################################################
# aspect sorter helper
#######################################################################

def getAspectPos( object, aspectname):
    str_ax,str_ay = object.GetRegistryValueDisp( 'PartRegs/' + aspectname + '/Position').split(',')
    return int(str_ax), int(str_ay)

#######################################################################
# aspect sorters
#######################################################################

def AspectAbsSort( a, b):
    """ Since objects can be placed in different aspects
    It is required for these comparators to define which
    aspect information are they going to consider.
    And since not all objects reside in a model (some sit in 
    a folder), those will throw exceptions when are asked
    about their position in a certain aspect.
    """
    aspectname = 'SignalFlowAspect'
    try:
        ax, ay = getAspectPos( a, aspectname)
        bx, by = getAspectPos( b, aspectname)
    
        absa = ax * ax + ay * ay;
        absb = bx * bx + by * by;
    
        r  =  0
        if( absa < absb)   :  r  = -1
        elif( absa > absb) :  r  =  1
        else               :  r  = IDSort( a, b)
    
        return r
    except:
        cout( 'Exc during comparing ' + a.Name + ' with ' + b.Name + ' in AspectAbsSort', 1)
        return IDSort( a, b)
    
def AspectHorizSort( a, b):
    aspectname = 'SignalFlowAspect'
    try:
        # get pairs of coordinates like "86, 86"
        ax, ay = getAspectPos( a, aspectname)
        bx, by = getAspectPos( b, aspectname)

        r  =  0
        if( ax < bx)   : r = -1
        elif(ax > bx)  : r = 1
        elif(ay < by)  : r = -1
        elif(ay > by)  : r = 1
        else           : r = 0

        return r
    except:
        cout( 'Exc during comparing ' + a.Name + ' with ' + b.Name + ' in AspectHorizSort', 1)
        return IDSort( a, b)

def AspectVerticSort( a, b, aspectname = 'SignalFlowAspect'):
    try:
        # get pairs of coordinates like "86, 86"
        ax, ay = getAspectPos( a, aspectname)
        bx, by = getAspectPos( b, aspectname)

        r  =  0
        if( ay < by)   : r = -1
        elif(ay > by)  : r = 1
        elif(ax < bx)  : r = -1
        elif(ax > bx)  : r = 1
        else           : r = 0

        return r
    except:
        cout( 'Exc during comparing ' + a.Name + ' with ' + b.Name + ' in AspectVerticSort', 1)
        return IDSort( a, b)

        
#######################################################################
# a generic handler for a visited object (fco or folder)
# implement/change it the way you like it
#######################################################################

def handle( _obj):
    try: 
        # dump info to the console
        cout( _obj.ID + ' ~ ' + _obj.GetGUIDDisp() + ' ~ ' + _obj.Name)
    finally:
        pass

    #
    # 1 alternative: dump data to a file:
    #f = open( 'c:\\visitation.txt', 'a')
    #try: 
    #    f.write( _obj.ID + '~' + _obj.GetGUIDDisp() + '~' + _obj.Name + '\n')
    #finally:
    #    f.close()

    return    

#######################################################################
# traversal algorithms
#######################################################################

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

def traverse_preorder( _current, _comparator):
    to_visit = []                                   # a list (!tuple)

    if _current.ObjType in (1, 6):                  # children for Models and Folders
        to_visit.extend( _current.ChildObjects)     # remains a list
        to_visit.sort( cmp = _comparator)           # which can sort

    # 1st: handle current node
    handle( _current)

    # 2nd: traverse subtrees
    for o in to_visit:
        traverse_preorder( o, _comparator)

    return

def traverse_inorder( _current, _comparator):
    to_visit = []                                   # a list (!tuple)

    if _current.ObjType in (1, 6):                  # children for Models and Folders
        to_visit.extend( _current.ChildObjects)     # remains a list
        to_visit.sort( cmp = _comparator)           # which can sort
    
    separate_at = (1 + len(to_visit))/2             # element at separate_at will be in right children
                                                    # leftchildren need visitation before the current node
                                                    # rightchildren need visitation after the current node
                                                    # we figure out an index (separate_at) to split the list in 2

    # 1st: traverse left children
    for i in range( 0, separate_at):
        traverse_inorder( to_visit[i], _comparator)

    # 2nd: handle current node
    handle( _current)
    
    # 3rd: traverse right children
    for i in range( separate_at, len(to_visit)):
        traverse_inorder( to_visit[i], _comparator)

    return
    
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

def traverse_invoker( _methodFunc, _rootFolder, _comparatorFunc):
    _methodFunc( _rootFolder, _comparatorFunc)               # simple call the _methodFunc with the provided parameters
    return

#######################################################################
#
#  Visitation according to the following graph traversals algorithms
#
#
#  Depth First methods: PreOrder, InOrder, PostOrder
#  Breath First method: Levelled
#
#  Children can be sorted during each traversal by:
#  Name                                               : NameSort
#  ObjectID (as strings)                              : IDSort
#  GUID (as strings, as in the displayed format)      : GUIDSort
#  Horizontal Position (in a certain aspect)          : AspectHorizSort
#  Vertical   Position (in a certain aspect)          : AspectVerticSort
#  Absolute Distance from (0,0) (in a certain aspect) : AspectAbsSort
#

def traverse():
    algo_text={ traverse_preorder  : 'Preorder traversal'          # dictionary, will help showing to the user which traversal is used
              , traverse_postorder : 'Postorder traversal'
              , traverse_inorder   : 'Inorder traversal'
              , traverse_levelled  : 'Levelled traversal (BFS)'}

    sort_text={ AspectAbsSort      : 'Absolute Position'           # dictionary, will help showing to the user which sort is used
              , AspectHorizSort    : 'Horizontal Position'
              , AspectVerticSort   : 'Vertical Position'
              , NameSort           : 'Name'
              , GUIDSort           : 'GUID'
              , IDSort             : 'ID'}

    #-----------------------------------
    # select the sort criteria here
    #-----------------------------------
    used_sort = NameSort
    
    #-----------------------------------
    # select traversal algorithm here
    #-----------------------------------
    used_algo = traverse_levelled

    cout( fancy('Examining project \'' + p.RootFolder.Name + '\' with ' + algo_text[used_algo] + ' method, sorting children by their ' + sort_text[ used_sort] + '.'), 1)

    #-----------------------------------
    traverse_invoker( used_algo, p.RootFolder, used_sort)       # use this generic invoker or ...

    #-----------------------------------
    #traverse_preorder ( p.RootFolder, used_sort)               # or any of these direct invokations
    #traverse_postorder( p.RootFolder, used_sort)
    #traverse_inorder  ( p.RootFolder, used_sort)
    #traverse_levelled ( p.RootFolder, used_sort)


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
    traverse()                                # do a traversal
    
    commit(p, terr)                           # commit transaction (strictly needed to return the project in a consistent mode)
except:
    cout( 'Need to abort', 2)                 # 2 means MSG_WARNING (yellow)
    abort(p, terr)                            # abort transaction if something went wrong

cout( '--End of Script--', 1)

p = None
g = None
