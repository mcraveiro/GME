//#################################################################################
//#
//#    This file contains JScript methods for project-wide traversal or query.
//#                  See bottom of the file for main()
//#
//#################################################################################
//#
//#  Query for objects based on different criteria
//#  _____________________________________________
//#
//# filter() method: 
//#        Usable to do a project wide search based on Name, Kind, Role, ObjectType, Level.
//#        ObjectType is an enum as follows:
//#               Model = 1, 
//#               Atom = 2,
//#               Reference = 3,
//#               Connection = 4,
//#               Set = 5,
//#               Folder = 6
//#        Level (depth) can be a space-separated list of numbers or dash-separated number pairs: e.g: 1-2 5 7
//#
//# filterScoped() method:
//#        Usable to do a scoped search based on Name, Kind, Role, ObjectType, Level.
//#        Starts from a container, performs search down in its containment tree.
//#
//#################################################################################

var g, p;
g = gme
p = project

//#######################################################################
//# general helpers
//#######################################################################

function cout3( gme, mStr, mType)
{
    gme.ConsoleMessage( mStr, mType);
}

function cout( mStr, mType)
{
    g.ConsoleMessage( mStr, mType);
}

function fancy( mInStr)
{
    var fancy = "<head><style type=\"text/css\">" +
    "td.special{ background-color:aqua;font-size: 100%;margin-left: 20px;font-family: times, sans-serif, arial}" +
    "</style>" +
    "</head>" +
    "<table><tr><td class=\"special\">" +
    mInStr +
    "</td></tr></table>";
    return fancy;
}

function makeLink( o)
{
    return '<a href=mga:' + o.ID + '">' + o.Name + '</a>';
}

function coll2array( in_collection)
{
    var a = new Array();
    for( var j = 1; j <= in_collection.Count; ++j) {     // indices from 1 ... count
        a.push( in_collection.Item(j));
    }
    return a;
}


//#######################################################################
//# transaction handlers
//#######################################################################

function begin( the_project )
{
    /// in GME7 and under must use: g.oleit.BeginTransaction();
    /// in GME8.5 and above use: project.BeginTransactionInNewTerr();

    try {
        var terr = the_project.BeginTransactionInNewTerr(); // will work on GME8
        return terr;
    } catch(err) {
        g.oleit.BeginTransaction();                         // will work on GME7
    }
}

function commit( the_project, terr )
{
    /// can be used also: (in all GME versions available)
    //g.oleit.CommitTransaction();

    the_project.CommitTransaction();

    if( terr != null) {
        terr.Destroy();
    }
}

function abort( the_project, terr )
{
    /// in GME8 you can use also:
    //g.oleit.AbortTransaction();

    the_project.AbortTransaction();

    if( terr != null) {
        terr.Destroy();
    }
}

///#######################################################################
///# helper methods for filter
///#######################################################################

function print_details_what_we_search_for( object, name, kind, role, otype, level)
{
    /// show some output to the console regarding what 
    /// kind of search the parameters are implying
    
    var prefix = "";
    var msg    = "";

    if( object != null) prefix = "Local query in " + makeLink( object) + " ";
    else                prefix = "Global query ";
    
    if( name != "")     msg += "[Name = '" + name + "'] ";
    if( kind != "")     msg += "[Kind = '" + kind + "'] ";
    if( role != "")     msg += "[Role = '" + role + "'] ";
    if( otype != "")    msg += "[ObjectType = '" + otype + "'] ";
    if( level != "")    msg += "[Level = '" + level + "'] ";
    
    if( msg == "")      // no criteria has been specified, will match all objects in scope
        cout( prefix + " with no specified criteria, will match all objects in scope!");
    else
        cout( prefix + " for objects with " + msg);
}

function show_results_in_my_way( res)
{
    /// user preferred way to show results
    if( res.length > 0)
        cout( "Results list:", 1);
    else
        cout( "No object found!", 2);
    for( var k = 0; k < res.length; ++k)
        cout( makeLink( res[k]), 1);
}

///#######################################################################
///# global filter on project
///#######################################################################

function filter( oname, okind, orole, otype, level)
{
    /// display some information about the criteria
    print_details_what_we_search_for( null, oname, okind, orole, otype, level);

    var flt = p.CreateFilter();
    flt.Name = oname;
    flt.Kind = okind;
    flt.Role = orole;
    flt.ObjType = otype;
    flt.Level = level;
    var res = p.AllFCOs( flt);                                                // filter used project-wide
    return coll2array( res);
}

///#######################################################################
///# local filter on the object (container typically)
///#######################################################################

function filterScoped( scopeObject, oname, okind, orole, otype, level)
{
    /// display some information about the criteria
    print_details_what_we_search_for( scopeObject, oname, okind, orole, otype, level);

    var flt = p.CreateFilter();
    flt.Name = oname;
    flt.Kind = okind;
    flt.Role = orole;
    flt.ObjType = otype;
    flt.Level = level;
    var res = scopeObject.GetDescendantFCOs( flt);                            // filter used only down under container object in the hierarchy tree
    return coll2array( res);
}

///#######################################################################
///# demo methods for the usage of filter and filterScoped
///#######################################################################

function query_demo_MetaGME()
{
    /// demo for the MetaGME paradigm:

    /// 'it.Valid' bool indicates whether a model window is currently
    /// shown (opened and active). In this case 'it.MgaModel' is a
    /// valid pointer to this container, so a scoped/local search 
    /// can be invoked using it. Otherwise do a global search.

    var objects;

    if( it.Valid)
        objects = filterScoped( it.MgaModel, "", "ModelProxy", "", "", "");         // kind filtering
    else
        objects = filter( "", "ModelProxy", "", "", "");                            // same, but globally

    show_results_in_my_way( objects);
}

///#######################################################################
function query_demo_SF()
{
    /// demo for the SF paradigm:
    /// Will demonstrate the use of Level criteria, which is relative to the container.
    /// Level 1: child-parent relationship between the object and the container parameter.
    /// Level 2: grandchild-grandparent relationship, etc.
    /// Syntax: "1 2", "3-5", "1 2 5-7", "3-".

    var objects;

    /// we will search among grandchildren, great-grandchildren etc. of it.MgaModel
    if( it.Valid)
        objects = filterScoped( it.MgaModel, "", "", "InputSignals", "", "2-");     // role and level filtering
    else
        objects = filter( "", "", "InputSignals", "", "2-");

    show_results_in_my_way( objects);
}

///#######################################################################
function query_demo_generic()
{
    /// demo, which might make sense for any paradigm: will search for atoms (2) and references(3)

    var objects;

    if( it.Valid)
        objects = filterScoped( it.MgaModel, "", "", "", "2 3", "");              /// looking for Atoms (2) and References (3)?
    else
        objects = filter( "", "", "", "2 3", "");                                 /// same on global scale

    show_results_in_my_way( objects);
}

///#######################################################################
function query_demo()
{
    var paradigm = p.MetaName;              /// what is the paradigm name of the opened project?

    if( paradigm == "MetaGME")
        query_demo_MetaGME();               /// execute MetaGME specific demo in case a metamodel is opened
    else if( paradigm == "SF")
        query_demo_SF();                    /// ... SF specific demo
    else
        query_demo_generic();               /// ... generic demo, queries for atoms and references (probably there are such elements in any paradigm)
}
  


///#######################################################################
///# main
                     
cout( "--Hello World, this is JScript here!--", 1);

var terr = begin(p);         /// begin transaction (strictly needed to perform r/w operations on a IMgaProject)

try {
    query_demo( p);

    commit( p, terr);
}
catch(err) {
    cout( "Exception [" + err.message + "]. Transaction will abort.", 3);
    abort( p, terr);
}

cout( "--End of script--", 1);

p = null
g = null
