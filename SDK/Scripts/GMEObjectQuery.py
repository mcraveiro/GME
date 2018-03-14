#################################################################################
#
#      This file contains Python methods for project-wide or local query.
#                   See bottom of the file for main()
#
#################################################################################
#
#  Query for objects based on different criteria
#  _____________________________________________
#
#
# filter() method: 
#        Usable to do a project wide search based on Name, Kind, Role, ObjectType, Level.
#        ObjectType is an enum as follows:
#               Model = 1, 
#               Atom = 2,
#               Reference = 3,
#               Connection = 4,
#               Set = 5,
#               Folder = 6
#               [ see objtype_enum GME/Interfaces/Meta.idl for more ]
#
#        Level (depth) can be a space-separated list of numbers or dash-separated number pairs: e.g: 1-2 5 7
#
# filterScoped() method:
#        Usable to do a scoped search based on Name, Kind, Role, ObjectType, Level.
#        Starts from a container, performs search down in its containment tree.
#
#################################################################################

import win32com.client

p = project                                                 # alias for the project pre-existing variable, implements IMgaProject interface (see Interfaces/Mga.idl)
g = gme                                                     # alias for the gme pre-existing variable, implements IGMEOLEApp interface  (see Interfaces/GME.idl)



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
# helper methods for filter
#######################################################################
def print_details_what_we_search_for( object, name, kind, role, otype, level):
    """ show some output to the console regarding what 
        kind of search the parameters are implying
    """

    prefix = ""
    msg    = ""

    if object: prefix = "Local query in " + makeLink( object) + " "
    else:      prefix = "Global query "
    
    if name != "":     msg += "[Name = '" + name + "'] "
    if kind != "":     msg += "[Kind = '" + kind + "'] "
    if role != "":     msg += "[Role = '" + role + "'] "
    if otype != "":    msg += "[ObjectType = '" + otype + "'] "
    if level != "":    msg += "[Level = '" + level + "'] "
    
    if msg == "":      # no criteria has been specified, will match all objects in scope
        cout( prefix + " with no specified criteria, will match all objects in scope!")
    else:
        cout( prefix + " for objects with " + msg)

def show_results_in_my_way( res):
    """ user preferred way to show results
    """

    if len( res) > 0:
        #cout( fancy("Result list:"))                               # fancy wraps the string into a colored HTML table cell
        cout( "Result list:")                                       # but it does not scroll down for some reason
    else:
        cout( "No object found!", 2)                                # 2: MSG_WARNING
    
    for k in range( 0, len( res)):                                  # show results
        cout( makeLink( res[k]))                                    # with a link so that users can click on them

#######################################################################
# global filter on project
#######################################################################

def filter( name = "", kind = "", role = "", otype = "", level = ""):
    """ Global search on project 
    """

    # display some information about the criteria
    print_details_what_we_search_for( 0, name, kind, role, otype, level)

    flt = p.CreateFilter()
    flt.Name = name
    flt.Kind = kind
    flt.Role = role
    flt.ObjType = otype
    flt.Level = level
    res = p.AllFCOs( flt)                                                # filter used project-wide

    return list(res)

#######################################################################
# local filter on the object (container typically)
#######################################################################

def filterScoped( object, name = "", kind = "", role = "", otype = "", level = ""):
    """ Search the hierarchy, starting from 'object' down
    """

    # display some information about the criteria
    print_details_what_we_search_for( object, name, kind, role, otype, level)

    flt = p.CreateFilter()
    flt.Name = name
    flt.Kind = kind
    flt.Role = role
    flt.ObjType = otype
    flt.Level = level
    res = object.GetDescendantFCOs( flt)                                 # filter used only down below container object in the hierarchy tree

    return list(res)

#######################################################################
# demo methods for the usage of filter and filterScoped
#######################################################################

def query_demo_MetaGME():
    """ demo for the MetaGME paradigm:
    """

    # 'it.Valid' bool indicates whether a model window is currently
    # shown (opened and active). In this case 'it.MgaModel' is a
    # valid pointer to this container, so a scoped/local search 
    # can be invoked using it. Otherwise do a global search.

    if it.Valid:
        objects = filterScoped( it.MgaModel, "", "ModelProxy", "", "", "")         # full param list used here but
    else:
        objects = filter( kind = "ModelProxy")                                     # named parameters used, or we could have used again
        #objects = filter( "", "ModelProxy", "", "", "")                           # this full parameter form

    show_results_in_my_way( objects)

#######################################################################
def query_demo_SF():
    """ demo for the SF paradigm:
        Will demonstrate the use of Level criteria, which is relative to the container.
        Level 1: child-parent relationship between the object and the container parameter.
        Level 2: grandchild-grandparent relationship, etc.
        Syntax: "1 2", "3-5", "1 2 5-7", "3-".
    """

    # we will search among grandchildren, great-grandchildren etc. of it.MgaModel 
    if it.Valid:
        objects = filterScoped( it.MgaModel, role = "InputSignals", level = "2-")
    else:
        objects = filter( role = "InputSignals", level = "2-")

    show_results_in_my_way( objects)

#######################################################################
def query_demo_generic():
    """ demo, which might make sense for any paradigm: will search for atoms (2) and references(3)
    """

    if it.Valid:
        objects = filterScoped( it.MgaModel, "", "", "", "2 3", "")              # looking for Atoms (2) and References (3)?
    else:
        objects = filter( "", "", "", "2 3", "")                                 # same on global scale

    show_results_in_my_way( objects)

#######################################################################
def query_demo():
    paradigm = p.MetaName                  # what is the paradigm name of the opened project?

    if paradigm == "MetaGME":
        query_demo_MetaGME()               # execute MetaGME specific demo in case a metamodel is opened
    elif paradigm == "SF":
        query_demo_SF()                    # ... SF specific demo
    else:
        query_demo_generic()               # ... generic demo, queries for atoms and references (probably there are such elements in any paradigm)
        
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
    query_demo()                              # do a query
    
    commit(p, terr)                           # commit transaction (strictly needed to return the project in a consistent mode)
except:
    cout( 'Transaction aborted', 2)           # 2 = MSG_WARNING (yellow)
    abort(p, terr)                            # abort transaction if something went wrong

cout( '--End of Script--', 1)

p = None
g = None
