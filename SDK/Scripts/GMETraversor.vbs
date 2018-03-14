'//#################################################################################
'//#
'//#    This file contains VBScript methods for project-wide traversal or query.
'//#                  See bottom of the file for main()
'//#
'//#################################################################################
'//#
'//#  Visitation according to the following graph traversals algorithms
'//#  _________________________________________________________________
'//# 
'//#  Breadth First method: Levelled
'//#  Depth First methods: PreOrder, InOrder, PostOrder
'//#  method names: traverse_levelled, traverse_preorder, traverse_inorder, traverse_postorder
'//#  
'//#  Children can be sorted during traversal by:
'//#        NameSort, IDSort, GUIDSort, AspectHorizSort, AspectVerticSort, AspectAbsSort
'//#  
'//#################################################################################
'//#
'//#  Query for objects based on different criteria
'//#  _____________________________________________
'//#
'//# filter() method: 
'//#        Usable to do a project wide search based on Name, Kind, Role, ObjectType, Level.
'//#        ObjectType is an enum as follows:
'//#               Model = 1, 
'//#               Atom = 2,
'//#               Reference = 3,
'//#               Connection = 4,
'//#               Set = 5,
'//#               Folder = 6
'//#        Level (depth) can be a space-separated list of numbers or dash-separated number pairs: e.g: 1-2 5 7
'//#
'//# filterScoped() method:
'//#        Usable to do a scoped search based on Name, Kind, Role, ObjectType, Level.
'//#        Starts from a container, performs search down in its containment tree.
'//#
'//#################################################################################

Set g = gme
Set p = project

Sub cout3( gme, mStr, mType)
    gme.ConsoleMessage mStr, mType
End Sub

Sub cout( mStr, mType)
    g.ConsoleMessage mStr, mType
End Sub

Function fancy( mInStr)
    fancy = "<head><style type=""text/css"">" &_
    "td.special{ background-color:aqua;font-size: 100%;margin-left: 20px;font-family: times, sans-serif, arial}" &_
    "</style>" &_
    "</head>" &_
    "<table><tr><td class=""special"">" &_
    mInStr &_
    "</td></tr></table>"
End Function

Function makeLink( o)
    makeLink = "<a href=mga:" & o.ID & """>" & o.Name & "</a>" 
End Function

'********************************************************************


Sub begin( the_project )
	the_project.BeginTransaction Nothing
End Sub

Sub commit( the_project )
	the_project.CommitTransaction
End Sub

Sub abort( the_project )
	the_project.AbortTransaction
End Sub

'********************************************************************

Sub handle( o)
	msg = o.ID & " ~ " & o.GetGUIDDisp() & " ~ " & o.Name
	cout msg, 1
End Sub

'********************************************************************

Sub traverse_preorder( currento)
    ' 1st: handle current node
    Call handle( currento)
    
    ' 2nd: traverse subtrees
    If  currento.ObjType = 1 or currento.ObjType = 6 Then              ' children for Models and Folders
	Set to_visit = currento.ChildObjects
	'Call sortMyArray( to_visit)                                      ' sorted with comparator

        For k = 1 to to_visit.Count
		traverse_preorder( to_visit(k))
	Next
    End If
End Sub

Sub traverse_postorder( currento)
    ' 1st: traverse subtrees
    If  currento.ObjType = 1 or currento.ObjType = 6 Then              ' children for Models and Folders
	Set to_visit = currento.ChildObjects
	'Call sortMyArray( to_visit)                                      ' sorted with comparator

        For k = 1 to to_visit.Count
		traverse_postorder( to_visit(k))
	Next
    End If
    
    ' 2nd: handle current node
    Call handle( currento)
End Sub

Sub traverse_inorder( currento)

    current_len = 0
    Dim to_visit()

    If  currento.ObjType = 1 or currento.ObjType = 6 Then              ' children for Models and Folders
	Set children = currento.ChildObjects
	ReDim Preserve to_visit( children.Count)
	
	' append the children to the to_visit list
	For k = 1 to children.Count
		Set to_visit(current_len) = children(k)
		current_len = current_len + 1
	Next

	'Call sortMyArray( to_visit)                                      ' sorted with comparator
    End If

    separate_at = current_len\2                                        ' trunc logic: [0..2], current len = 3, separate at = 1
                                                                       ' elem at index separate_at is in right children

    ' 1st: traverse left children
    k = 0
    While k < separate_at
	traverse_inorder( to_visit(k))
	k = k + 1
    Wend
    
    ' 2nd: handle current node
    Call handle( currento)

    ' 3rd: traverse right children
    While k < current_len
	traverse_inorder( to_visit(k))
	k = k + 1
    Wend
End Sub

Sub traverse_levelled( rootfolder)                ' aka BFT, Breadth First Traversal, with a FIFO queue logic
	Dim to_visit()

	current_len = 1
	ReDim to_visit( current_len)
	Set to_visit(0) = rootfolder

	i = LBound(to_visit)
	While i < current_len
		Set o = to_visit(i)
		Call handle( o)                   ' handle the currently selected object
		                                  '
		If o.ObjType = 1 or o.ObjType = 6 Then
			Set children = o.ChildObjects
			' cout "Combined size will be : " & current_len + children.Count, 1
			ReDim Preserve to_visit( current_len + children.Count)
			
			' append the children to the to_visit list
			For k = 1 to children.Count
				Set to_visit(current_len) = children(k)
				current_len = current_len + 1
			Next
		End If
		i = i + 1
	Wend
End Sub

'********************************************************************

Function filter( fname, fkind, frole, otype, level)
    Set flt = p.CreateFilter()
    cout flt.Project.RootFolder.Name, 3
    flt.Name = fname
    flt.Kind = fkind
    flt.Role = frole
    flt.ObjType = otype
    flt.Level = level
    Set res = p.AllFCOs( flt)                                                ' filter used project-wide
    cout "Results follow: ", 1
    For i = 1 to res.Count
       cout res.Item(i).Name & " -> " & makeLink( res.Item(i)), 1
    Next
    Set filter = res
End Function

Function filterScoped( object, fname, fkind, frole, otype, level)
    Set flt = p.CreateFilter()
    flt.Name = fname
    flt.Kind = fkind
    flt.Role = frole
    flt.ObjType = otype
    flt.Level = level
    Set res = object.GetDescendantFCOs( flt)                                 ' filter used only down below container object in the hierarchy tree
    cout "Results follow: ", 1
    For i = 1 to res.Count
       cout res.Item(i).Name & " -> " & makeLink( res.Item(i)), 1
    Next
    Set filterScoped = res
End Function

'********************************************************************

Sub test1( m)
     'Call filter( "", "ModelProxy", "", "", "")
     'Call filterScoped( it.MgaModel, "", "ModelProxy", "", "", "")
     Set fres1 = filter( "", "Model", "", "", "")
     Set fres2 = filterScoped( it.MgaModel, "", "Model", "", "", "")
     cout "Filtered results follow", 2
     For i = 1 to fres2.Count
        cout makeLink( fres2.Item(i)), 2
     Next
     
    
End Sub

'********************************************************************
'* main


On Error Resume Next ' turn off default error handler but only on this level (not in subs)
                     ' On Error GoTo 0 ' turn on default error handler
                     ' The saying is, that it works like SEH (Structured Exception Handling)
                     ' Errors are propagated up in the stack, until somebody handles it
                     ' in our case, somebody has a 'Resume Next' cmd and handles it
                     


cout fancy("--Greetings!--"), 1
cout "Hello World, this is VBScript here!", 1

Call begin( p)

cout p.RootFolder.Name, 1
'Call traverse_levelled( p.RootFolder)
'Call traverse_postorder( p.RootFolder)
'Call traverse_preorder( p.RootFolder)
'Call traverse_inorder( p.RootFolder)
test1 it.mgamodel
Call commit( p)

If Err.Number <> 0 Then   ' "Exception handling"
	Call abort( p)
End If
