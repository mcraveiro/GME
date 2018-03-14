/*
 * Copyright (c) 2002, Vanderbilt University
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE VANDERBILT UNIVERSITY BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE VANDERBILT
 * UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE VANDERBILT UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE VANDERBILT UNIVERSITY HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Author: Gyorgy Balogh, Gabor Pap
 * Date last modified: 11/10/03
 */

package org.isis.gme.modelexplorer;

import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.Vector;

import org.eclipse.core.runtime.Path;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;
import org.isis.gme.attributeview.AttributeView;
import org.isis.gme.mga.MgaFCO;
import org.isis.gme.mga.MgaModel;
import org.isis.gme.mga.MgaObject;
import org.isis.gme.modeleditor.ModelEditorInput;
import org.isis.gme.plugin.GmePlugin;
import org.isis.gme.plugin.Project;
import org.isis.gme.plugin.Projects;

public class ModelExplorer extends ViewPart{
    private TreeViewer           viewer;      
    private IStructuredSelection lastSelection = null;   
    private Action               openProjectAction;
    private Action               closeProjectAction;
    private Action               deleteAction;
    private Action               importProjectAction;
    private Action				 removeProjectAction;
    
    private Image                atomImage;
    private Image                modelImage;
    private Image                connectionImage;
    private Image                referenceImage;
    private Image                aspectImage;
    
    private Item                 root;
            
    public class Item implements Comparable{
        private Object      connection = null;  // associated object
        private Item        parent     = null;
        private Vector      children   = new Vector();               
                        
        public Item( Object obj, Item parent )
        {
            connection  = obj;
            this.parent = parent;
            update();         
        }
       
        public void update()
        {
            children.clear();
            if( connection instanceof Projects )
            {
                Projects projects = (Projects)connection;
                Iterator it = projects.getProjects().iterator();
                while( it.hasNext() )
                    children.add( new Item((Project)it.next(),this) );                                
            }
            else if( connection instanceof Project )
            {
                Project project = (Project)connection; 
                if( project.isOpened() )
                {                
                    MgaObject mgaChildren[] = project.getMgaProject().getRootFolder().getChildObjects().getAll();
                    for( int i=0; i<mgaChildren.length; ++i )
                        children.add( new Item(mgaChildren[i],this) );                                                                          
                }                
            }
            else if( connection instanceof MgaObject )
            {
                MgaObject mgaObj = (MgaObject )connection; 
                int type = mgaObj.getObjType();
                if( type == MgaObject.OBJTYPE_MODEL || type == MgaObject.OBJTYPE_FOLDER )
                {                            
                    MgaObject mgaChildren[] = mgaObj.getChildObjects().getAll();
                    for( int i=0; i<mgaChildren.length; ++i )
                        children.add( new Item(mgaChildren[i],this) );
                }                                                                         
            }
        }
        
        public Vector getChildren()
        {
            return children;
        }
        
        public Object getParent(){
            return parent;
        }
        
        public Object getConnection()
        {
            return connection;
        }
                               
        public String toString()
        {
            if( connection instanceof Project )
                return ((Project)connection).toString();
            else if( connection instanceof MgaObject )
                return ((MgaObject)connection).getName();            
            else
                return "unknown object";
        }        
        
        private Image getSharedImage( String image )
        {
            return PlatformUI.getWorkbench().getSharedImages().getImage( image );
        }
        
        public Image getImage()
        {
            if( connection instanceof Project )
            {
                Project project = (Project)connection;
                if( project.isOpened() ) 
                    return getSharedImage( ISharedImages.IMG_OBJ_PROJECT );
                else
                    return getSharedImage( ISharedImages.IMG_OBJ_PROJECT_CLOSED);                                     
            }
            else if( connection instanceof MgaObject )
            {
                MgaObject mgaObj = (MgaObject)connection; 
                int type = mgaObj.getObjType();
                switch( type )
                {
                    case MgaObject.OBJTYPE_ATOM:
                        return atomImage;
                    case MgaObject.OBJTYPE_FOLDER:
                        return getSharedImage( ISharedImages.IMG_OBJ_FOLDER );                                           
                    case MgaObject.OBJTYPE_MODEL:
                        return modelImage;
                    case MgaObject.OBJTYPE_REFERENCE:
                        return referenceImage;
                    case MgaObject.OBJTYPE_CONNECTION:
                        return connectionImage;
                    case MgaObject.OBJTYPE_ASPECT:
                        return aspectImage;                    
                    default:
                        return getSharedImage( ISharedImages.IMG_OBJ_ELEMENT );                                          
                }               
            }
            else
            {
                return getSharedImage( ISharedImages.IMG_OBJ_ELEMENT );
            }
        }

		/* (non-Javadoc)
		 * @see java.lang.Comparable#compareTo(java.lang.Object)
		 */
		public int compareTo(Object arg0) {
			Item item0 = (Item)arg0;
			return this.toString().compareTo(item0.toString());
		}
    }

    class ViewContentProvider implements ITreeContentProvider{
        public void inputChanged(Viewer v, Object oldInput, Object newInput){
        }
        
        public void dispose(){
        }
        
        public Object[] getElements(Object parent){            
            return getChildren(parent);
        }
        
        public Object getParent(Object child){
            return ((Item)child).getParent();
        }
        
        public Object[] getChildren(Object parent){
			Object[] ret = ((Item)parent).getChildren().toArray();
			Arrays.sort(ret);
			return ret;
        }
        
        public boolean hasChildren(Object parent){
            return ((Item)parent).getChildren().size() > 0;            
        }
    }
    
    class ViewLabelProvider extends LabelProvider
    {
        public String getText(Object obj)
        {
            return obj.toString();
        }
        public Image getImage(Object obj)
        {
            return ((Item)obj).getImage();
        }
    }                   

    public ModelExplorer()
    {
        atomImage       = getImage( "atom.gif" );
        modelImage      = getImage( "model.gif" );
        connectionImage = getImage( "connection.gif" );
        referenceImage  = getImage( "reference.gif" );
        aspectImage     = getImage( "aspect.gif" );               
    }
    
    private Image getImage( String name )
    {
        Path path = new Path("icons/" + name);
        URL url = GmePlugin.getPlugin().getDescriptor().find(path);
        return ImageDescriptor.createFromURL(url).createImage();        
    }

    public void createPartControl(Composite parent){
        // create tree viewer
        viewer = new TreeViewer(parent, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL);
        
        // set content
        viewer.setContentProvider(new ViewContentProvider());
        viewer.setLabelProvider(new ViewLabelProvider());
        root = new Item( GmePlugin.getPlugin().getProjects(), null );
        viewer.setInput( root );

        // event handling
        makeActions();
        hookDoubleClickAction();
        hookSelectionChangeAction();                        
        hookContextMenu();
    }
    
    private void makeActions(){
        makeOpenAction();
        makeCloseAction();
        makeImportAction();
		makeRemoveAction();
    }
    
    private void makeOpenAction(){
        openProjectAction = new Action("Open Project"){
            public void run(){
                Iterator it = lastSelection.iterator();
                while( it.hasNext() ){
                    Item   item = (Item )it.next();
                    Object conn = item.getConnection();
                    if( conn instanceof Project ){
                        Project project = (Project)conn;
                        project.open();
                        item.update();
                        viewer.refresh(item);                        
                    }
                }                                                
            }
        };        
    }
    
    private void makeCloseAction()
    {
        closeProjectAction = new Action("Close Project")
        {
            public void run()
            {
                Iterator it = lastSelection.iterator();
                while( it.hasNext() )
                {
                    Item   item = (Item )it.next();
                    Object conn = item.getConnection();
                    if( conn instanceof Project )
                    {
                        Project project = (Project)conn;
                        project.close();
                        item.update();
                        viewer.refresh(item);                        
                    }
                }
            }
        };        
    }
    
    private void makeImportAction()
    {                                        
        importProjectAction = new Action("Import Project")
        {
            public void run()
            {
                FileDialog fileDlg = new FileDialog(viewer.getTree().getShell(), SWT.OPEN);
                fileDlg.setFilterExtensions( new String[]{"*.mga"} );
                String ret = fileDlg.open();
                if( ret != null )
                {
                    Projects projs = GmePlugin.getPlugin().getProjects();
                    Project  proj  = new Project( projs, "MGA=" + ret );                
                    projs.getProjects().add( proj );
                    root.children.add( new Item(proj,root) );                
                    viewer.refresh();
                    GmePlugin.getPlugin().saveProjects();
                }
            }
        };       
    }

	private void makeRemoveAction(){                                        
		removeProjectAction = new Action("Remove Project"){
			public void run(){
				Collection remove = new ArrayList();
				Iterator it = lastSelection.iterator();
				while( it.hasNext() ){
					Item   item = (Item )it.next();
					Object conn = item.getConnection();
					if( conn instanceof Project ){
						Project project = (Project)conn;
						remove.add(project);
					}
				}
				Projects projs = GmePlugin.getPlugin().getProjects();
				projs.getProjects().removeAll(remove);
				root.update();
				viewer.refresh();
			}
		};       
	}

    private void hookContextMenu()
    {
        MenuManager menuMgr = new MenuManager("#PopupMenu");
        menuMgr.setRemoveAllWhenShown(true);
        menuMgr.addMenuListener(new IMenuListener()
        {
            public void menuAboutToShow(IMenuManager manager)
            {
                boolean openProject   = true;
                boolean closeProject  = true;
				boolean removeProject = false;
                boolean allOpened     = true;
                boolean allClosed     = true;
                                
                if (lastSelection != null){
					Iterator it = lastSelection.iterator();
					while( it.hasNext() ){
						Object conn = ((Item )it.next()).getConnection();
						if( conn instanceof Project ){
							Project project = (Project)conn;
							removeProject = true;
							if( project.isOpened() )
								allClosed = false;
							else
								allOpened = false;                                                    
						}else{
							openProject  = false;
							closeProject = false;
						}
					}
                }
                
                if( openProject && !allOpened ) manager.add( openProjectAction );
                if( closeProject && !allClosed ) manager.add( closeProjectAction );
                
                manager.add( importProjectAction );
                if (removeProject) manager.add( removeProjectAction );
             }
        });
        Menu menu = menuMgr.createContextMenu(viewer.getControl());
        viewer.getControl().setMenu(menu);
        //getSite().registerContextMenu(menuMgr, viewer);      
    }   
   
    private void hookDoubleClickAction(){
        viewer.addDoubleClickListener(new IDoubleClickListener(){
            public void doubleClick(DoubleClickEvent event){                
                try{                                      
                    Iterator it   = lastSelection.iterator();
                    Item     item = (Item )it.next();
                    Object   conn = item.getConnection();
                    if( conn instanceof MgaObject ){
                        MgaObject obj = (MgaObject)conn; 
                        int type = obj.getObjType();
                        if( type == MgaObject.OBJTYPE_MODEL ){
                            IWorkbenchPage   page = GmePlugin.getPlugin().getWorkbench().getActiveWorkbenchWindow().getActivePage();
                            page.openEditor( new ModelEditorInput(new MgaModel( obj )), "org.isis.gme.modeleditor.ModelEditor" );                           
                        }                        
                    }
					if( conn instanceof Project){
						Project project = (Project)conn; 
						if( !project.isOpened()){
							project.open();
							item.update();
							viewer.setExpandedState(item, true);
							viewer.refresh(item);
						}                        
					}                                                           
                }catch( Exception e ){
                    e.printStackTrace();
                }                            
            }
        });
    }
    
    private void hookSelectionChangeAction()
    {
        viewer.addSelectionChangedListener( new ISelectionChangedListener()
        {
            public void selectionChanged(SelectionChangedEvent event)
            {
                lastSelection = (IStructuredSelection)event.getSelection();
                
                MgaFCO fco = null;
                if( lastSelection.size() == 1 )
                {
                    Iterator it = lastSelection.iterator();
                    Item   item = (Item )it.next();
                    Object   conn = item.getConnection();
                    if( conn instanceof MgaObject )
                    {
                        MgaObject obj = (MgaObject)conn;
                        int type = obj.getObjType();
                        if( type == MgaObject.OBJTYPE_MODEL 
                         || type == MgaObject.OBJTYPE_ATOM
                         || type == MgaObject.OBJTYPE_REFERENCE
                         || type == MgaObject.OBJTYPE_CONNECTION)
                        {                   
                            fco = new MgaFCO( obj );      
                        }                        
                    }                                        
                }
                
                AttributeView attribView = GmePlugin.getPlugin().getAttributeView();
                if( attribView != null )                 
                    attribView.setFCO(fco);
                
                // TODO: update attribute editor according to this selection
            }             
        });
    }
    
    private void showMessage(String message)
    {
        MessageDialog.openInformation( viewer.getControl().getShell(), "GME Model Explorer"
                                     , message);
    }
    
    public void setFocus()
    {
        viewer.getControl().setFocus();
    }
}