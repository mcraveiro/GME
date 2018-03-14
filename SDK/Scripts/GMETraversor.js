//#################################################################################
//#
//#        This file contains JScript methods for project-wide traversal.
//#                  See bottom of the file for main()
//#
//#################################################################################
//#
//#  Visitation according to the following graph traversals algorithms
//#  _________________________________________________________________
//# 
//#  Breadth First method: Levelled
//#  Depth First methods: PreOrder, InOrder, PostOrder
//#  method names: traverse_levelled, traverse_preorder, traverse_inorder, traverse_postorder
//#  
//#  Children can be sorted during traversal by:
//#        NameSort, IDSort, GUIDSort, AspectHorizSort, AspectVerticSort, AspectAbsSort
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


//#############################################################
// OBJTYPE_REFERENCE (3)
//#############################################################
function refersTo( r)
{
    var tgt = null;
    if( r.ObjType == 3)
        tgt = r.Referred;
       
    return tgt;
}

function setReferTo( r, o)
{
    if( r.ObjType == 3)
        r.Referred = o;
}

function referredBy( o)
{
    return coll2array( o.ReferencedBy);
}

//#############################################################
// OBJTYPE_SET (5)
//#############################################################

function isMemberIn( s, o)
{
    return s.ObjType == 5 && s.GetIsMemberDisp( o);
}

function members( s)
{
    var mems = null;
    if( s.ObjType == 5)
        mems = coll2array( s.Members);
        
    return mems;
}

function memberOfSets( o)
{
    return coll2array( o.MemberOfSets)
}

function addToSet( s, o)
{
    if( s.ObjType == 5)
        s.AddMember( o);
}

function remFromSet( s, o)
{
    if( s.ObjType == 5)
        s.RemoveMember( o);
}

//#############################################################
// OBJTYPE_CONNECTION
//#############################################################
function connect( m, srco, dsto, conn_mrole)
{
    if( m.ObjType == 1) // Model
    {
    	var c = m.CreateSimpleConn( conn_mrole, srco, dsto, null, null);
    	c.Name = conn_mrole.Name;
    }
}

function connectThruRefChain( m, srco, dsto, src_ref_chain, dst_ref_chain, conn_mrole)
{
    if( m.ObjType == 1) // Model
    {
    	var c = m.CreateSimpleConn( conn_mrole, srco, dsto, src_ref_chain, dst_ref_chain, conn_mrole);
    	c.Name = conn_mrole.Name;
    }
}

function partOfConns( o)
{
    var conn_points = coll2array( o.PartOfConns);

    var conns = new Array();
    for( i = 0; i < conn_points.length; ++i)
    {
    	conns.push( conn_points[i].Owner);
    }
    return conns;
}

//#############################################################
// OBJTYPE_MODEL (1)
//#############################################################

function metaRoleByName( m, kind_str)
{
    if( m.ObjType == 1) // Model
    {
    	return m.Meta.GetRoleByNameDisp( kind_str);
    }
    return null;
}

function metaRole( o)
{
    // it seems we don't have access to meta info JScript
    // because it does return undefined
    return o.MetaRole;
}

function metaName( o)
{
    // it seems we don't have access to meta info from JScript
    // because it does return undefined
    return o.Meta.Name;
}

function newChild( m, kind_str)
{
    var nch = null;
    if( m.ObjType == 1)
    {
    	var mrole = metaRoleByName( m, kind_str);
    	if( null != mrole)
    	{
    	    nch = m.CreateChildObject( mrole);
    	    nch.Name = mrole.Name;
    	}
    }
    return nch;
}

function childByName( m, name_str)
{
    var ch = null;
    if( m.ObjType == 1)
    {
    	ch = m.GetChildFCODisp( name_str);
    }
    return ch;
}

function children( m)
{
    if( m.ObjType == 1)
    {
    	return coll2array( m.ChildFCOs);
    }
    return null;
}

function parent( o)
{
    if( o.ParentFolder)     return o.ParentFolder; // by checking ParentFolder first, we can use the same 
    else if( o.ParentModel) return o.ParentModel;  // implementation for folders and fcos, since folders
    else                    return null;           // have too a method named ParentFolder
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


//#######################################################################
//# sorters
//#######################################################################
function sortNumber(a, b)
{
    return a - b;
}

function IDSort( x, y)
{
    if( x.ID < y.ID)       r = -1;
    else if( x.ID > y.ID)  r = 1;
    else                   r = 0;
    return r;
}

function GUIDSort( x, y)
{
    if( x.GetGUIDDisp() < y.GetGUIDDisp())       r = -1;
    else if( x.GetGUIDDisp() > y.GetGUIDDisp())  r = 1;
    else                                         r = 0;
    return r;
}

function NameSort( x, y)
{
    if( x.Name < y.Name)       r = -1;
    else if( x.Name > y.Name)  r = 1;
    else                       r = 0;
    return r;
}

//#######################################################################
//# aspect sorter helper
//#######################################################################

function getAspectPos( object, aspectname)
{
    var pos_array = object.GetRegistryValueDisp( 'PartRegs/' + aspectname + '/Position').split(',');
    var int_array = new Array( parseInt( pos_array[0]), parseInt( pos_array[1]));
    return int_array;
}

//#######################################################################
//# aspect sorters
//#######################################################################

function AspectAbsSort( a, b)
{
    /*Since objects can be placed in different aspects
    It is required for these comparators to define which
    aspect information are they going to consider.
    And since not all objects reside in a model (some sit in 
    a folder), those will throw exceptions when are asked
    about their position in a certain aspect.
    */
    var aspectname = 'SignalFlowAspect';
    try {
        var a_pos = getAspectPos( a, aspectname);
        var b_pos = getAspectPos( b, aspectname);
        var ax = a_pos[0], ay = a_pos[1];
        var bx = b_pos[0], by = b_pos[1];
    
        absa = ax * ax + ay * ay;
        absb = bx * bx + by * by;
    
        r  =  0
        if( absa < absb)         r  = -1;
        else if( absa > absb)    r  =  1;
        else                     r  = IDSort( a, b);
    
        return r;
    }
    catch(err) {
        cout( 'Exc during comparing ' + a.Name + ' with ' + b.Name + ' in AspectAbsSort, Message = ' + err.message, 1)
        return IDSort( a, b)
    }
}
    
function AspectHorizSort( a, b)
{
    var aspectname = 'SignalFlowAspect';
    try {
        // get pairs of coordinates like "86, 86"
        var a_pos = getAspectPos( a, aspectname);
        var b_pos = getAspectPos( b, aspectname);
        var ax = a_pos[0], ay = a_pos[1];
        var bx = b_pos[0], by = b_pos[1];

        r  =  0;
        if( ax < bx)        r = -1;
        else if(ax > bx)    r = 1;
        else if(ay < by)    r = -1;
        else if(ay > by)    r = 1;
        else                r = 0;

        return r;
    }
    catch(err) {
        cout( 'Exc during comparing ' + a.Name + ' with ' + b.Name + ' in AspectHorizSort, Message = ' + err.message, 1);
        return IDSort( a, b)
    }
}

function AspectVerticSort( a, b)
{
    var aspectname = 'SignalFlowAspect';
    try {
        // get pairs of coordinates like "86, 86"
        var a_pos = getAspectPos( a, aspectname);
        var b_pos = getAspectPos( b, aspectname);
        var ax = a_pos[0], ay = a_pos[1];
        var bx = b_pos[0], by = b_pos[1];

        r  =  0;
        if( ay < by)        r = -1;
        else if(ay > by)    r = 1;
        else if(ax < bx)    r = -1;
        else if(ax > bx)    r = 1;
        else                r = 0;

        return r;
    }
    catch(err) {
        cout( 'Exc during comparing ' + a.Name + ' with ' + b.Name + ' in AspectVerticSort, Message = ' + err.message, 1);
        return IDSort( a, b);
    }
}

function handle( o)
{
	cout( o.GetGUIDDisp() + " ~ " + o.ID + " ~ " + o.Name, 1);
}


//#######################################################################
//# traversal algorithms
//#######################################################################

function traverse_postorder( currento, comparator)
{
    // 1st: traverse subtrees
    if( currento.ObjType == 1 || currento.ObjType == 6)                  // children for Models and Folders
    {
        var to_visit = coll2array( currento.ChildObjects);
        to_visit.sort( comparator);                                      // sorted with comparator

        for( var oi = 0; oi < to_visit.length; ++oi) 
        {
            traverse_postorder( to_visit[oi], comparator);
        }
    }
    
    // 2nd: handle current node
    handle( currento);
}

function traverse_preorder( currento, comparator)
{
    // 1st: handle current node
    handle( currento);

    // 2nd: traverse subtrees
    if( currento.ObjType == 1 || currento.ObjType == 6)                  // children for Models and Folders
    {
        var to_visit = coll2array( currento.ChildObjects);
        //to_visit.sort( comparator);                                    // sorted with comparator

        for( var oi = 0; oi < to_visit.length; ++oi)
        {
           traverse_preorder( to_visit[oi], comparator);
        }
    }
}    

function traverse_inorder( currento, comparator)
{
    var to_visit = new Array();
    var separate_at = 0;
    var i = 0;
    
    if( currento.ObjType == 1 || currento.ObjType == 6)                // children for Models and Folders
    {
        to_visit = coll2array( currento.ChildObjects);
        to_visit.sort( comparator);                                    // sorted with comparator

        separate_at = Math.floor( (1 + to_visit.length)/2);            // element at separate_at will be in right children
                                                                       // leftchildren need visitation before the current node
                                                                       // rightchildren need visitation after the current node
                                                                       // we figure out an index (separate_at) to split the list in 2
    }

    // 1st: traverse left children
    //cout( "Left: from " + i + " to " + separate_at, 2);
    for( i = 0; i < separate_at; ++i) {
        traverse_inorder( to_visit[i], comparator);
    }

    // 2nd: handle current node
    handle( currento);
    
    // 3rd: traverse right children
    //cout( "Right: from " + separate_at + " to " + to_visit.length, 2);
    for( i = separate_at; i < to_visit.length; ++i) {
        traverse_inorder( to_visit[i], comparator);
    }
}    


function traverse_levelled( rootfolder, cmp)      //  ' aka BFT, Breadth First Traversal
{                                                        //  ' non-recursive implementation with a fifo queue
    var to_visit = new Array( rootfolder);
    var i = 0;
    while( i < to_visit.length)
    {
        var o = to_visit[i];
        handle( o);
        if( o.ObjType == 1 || o.ObjType == 6)
        {
            // convert childobjects into an array
            var to_append = coll2array( o.ChildObjects);
            
            //
            // sort children
            to_append.sort( cmp);
            
            //
            // append to the queue
            to_visit = to_visit.concat( to_append);
        }
        ++i;
    }
}

function traverse()
{
    var algo_text = new Array();
    algo_text[traverse_preorder  ] = 'Preorder traversal';         // dictionary, will help showing to the user which traversal is used
    algo_text[traverse_postorder ] = 'Postorder traversal';
    algo_text[ traverse_inorder  ] = 'Inorder traversal';
    algo_text[ traverse_levelled ] = 'Levelled traversal (BFS)';

    var sort_text = new Array();
    sort_text[ AspectAbsSort    ] = 'Absolute Position';           // dictionary, will help showing to the user which sort is used
    sort_text[ AspectHorizSort  ] = 'Horizontal Position';
    sort_text[ AspectVerticSort ] = 'Vertical Position';
    sort_text[ NameSort         ] = 'Name';
    sort_text[ GUIDSort         ] = 'GUID';
    sort_text[ IDSort           ] = 'ID';


    //#-----------------------------------
    //# select the sort criteria here
    //#-----------------------------------
    var used_sort = AspectHorizSort;

    //#-----------------------------------
    //# select traversal algorithm here
    //#-----------------------------------
    var used_algo = traverse_levelled;

    cout( 'Examining project \'' + p.RootFolder.Name + '\' with ' + algo_text[used_algo] + ' method, sorting children by their ' + sort_text[ used_sort] + '.', 1)


    cout( "Traversing " + p.RootFolder.Name, 1);
    used_algo( p.RootFolder, used_sort);
    // or simply:
    //traverse_levelled( p.RootFolder, IDSort);
    //traverse_preorder( p.RootFolder, IDSort);
    //traverse_postorder(p.RootFolder, IDSort);
    //traverse_inorder(  p.RootFolder, IDSort);
}


//#######################################################################
//# main
                     
cout( "--Hello World, this is JScript here!--", 1);

var terr = begin(p);

try {
    traverse();

    commit( p, terr);
}
catch(err) {
    cout( "Exception [" + err.message + "]. Transaction will abort.", 3);
    abort( p, terr);
}

cout( "--End of script!--", 1);

p = null
g = null