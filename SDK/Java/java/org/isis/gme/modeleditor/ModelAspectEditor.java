/*
 * Created on Oct 8, 2003
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package org.isis.gme.modeleditor;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.gef.DefaultEditDomain;
import org.eclipse.gef.editparts.ScalableFreeformRootEditPart;
import org.eclipse.gef.ui.actions.ZoomInAction;
import org.eclipse.gef.ui.actions.ZoomOutAction;
import org.eclipse.gef.ui.parts.GraphicalEditor;
import org.eclipse.gef.ui.parts.ScrollingGraphicalViewer;
import org.eclipse.jface.action.IAction;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.isis.gme.modeleditor.control.GmeEditPartFactory;
import org.isis.gme.modeleditor.model.GmeDiagram;

/**
 * ModelEditor.java
 * 
 * @author Papszi
 */
public class ModelAspectEditor extends GraphicalEditor 
{
    private GmeDiagram modelDiagram = null;

    public ModelAspectEditor() 
    {       
        setEditDomain(new DefaultEditDomain(this));
    }
    
    public void init(IEditorSite site, IEditorInput input) 
    {
        try
        {        
            super.init( site, input );
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        modelDiagram = new GmeDiagram((ModelAspectEditorInput)getEditorInput());        
    }

    public void doSave(IProgressMonitor progressMonitor) 
    {        
    }
    
    public void gotoMarker(IMarker marker) 
    {
    }
    
    public void doSaveAs() 
    {
    }

    public boolean isDirty() 
    {
        return false;
    }
    
    public boolean isSaveAsAllowed() 
    {
        return true;
    }

    protected void initializeGraphicalViewer() 
    {
        getGraphicalViewer().setContents(modelDiagram);
        /*getGraphicalViewer().addDropTargetListener(
            new ModelItemTransferDropTargetListener(getGraphicalViewer()));
        */
    }

    protected void configureGraphicalViewer() 
    {
        super.configureGraphicalViewer();
        ScrollingGraphicalViewer viewer = (ScrollingGraphicalViewer)getGraphicalViewer();

        ScalableFreeformRootEditPart root = new ScalableFreeformRootEditPart();//GmeRootEditPart();//
        viewer.setRootEditPart(root);

        IAction zoomIn = new ZoomInAction(root.getZoomManager());
        IAction zoomOut = new ZoomOutAction(root.getZoomManager());
        getActionRegistry().registerAction(zoomIn);
        getActionRegistry().registerAction(zoomOut);

        viewer.setEditPartFactory(new GmeEditPartFactory());

        /*getSite().getKeyBindingService().registerAction(zoomIn);
        getSite().getKeyBindingService().registerAction(zoomOut);*/


/*      ContextMenuProvider provider = new LogicContextMenuProvider(viewer, getActionRegistry());
        viewer.setContextMenu(provider);
        getSite().registerContextMenu("org.eclipse.gef.examples.logic.editor.contextmenu", //$NON-NLS-1$
            provider, viewer);
        viewer.setKeyHandler(new GraphicalViewerKeyHandler(viewer)
            .setParent(getCommonKeyHandler()));*/
    }

    protected GmeDiagram getModelDiagram() 
    {
        return modelDiagram;
    }

    public void setModelDiagram(GmeDiagram diagram) 
    {
        modelDiagram = diagram;
    }

    /*protected void closeEditor(boolean save) 
    {
        getSite().getPage().closeEditor(ModelEditor.this, save);
    }*/
    
    /*public Object getAdapter(Class type) 
    {
        if (type == ZoomManager.class)
            return (
                (ScalableFreeformRootEditPart) getGraphicalViewer().getRootEditPart()).getZoomManager();

        return super.getAdapter(type);
    }*/

/*  protected void createActions() {
        super.createActions();
        ActionRegistry registry = getActionRegistry();
        IAction action;
    
        action = new CopyTemplateAction(this);
        registry.registerAction(action);

        action = new GmePasteTemplateAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new IncrementDecrementAction(this, true);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new IncrementDecrementAction(this, false);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new DirectEditAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction(this, PositionConstants.LEFT);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction(this, PositionConstants.RIGHT);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction(this, PositionConstants.TOP);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction(this, PositionConstants.BOTTOM);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction(this, PositionConstants.CENTER);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction(this, PositionConstants.MIDDLE);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

    }

        // It begins here:
    private KeyHandler keyHandler;

    protected void createOutputStream(OutputStream os) throws IOException {
        ObjectOutputStream out = new ObjectOutputStream(os);
        out.writeObject(getEditorRootPart());
        out.close();
    }

    protected KeyHandler getCommonKeyHandler() {
        if (keyHandler == null) {
            keyHandler = new KeyHandler();
            keyHandler.put(
                KeyStroke.getPressed(SWT.DEL, 127, 0),
                getActionRegistry().getAction(GEFActionConstants.DELETE));
            keyHandler.put(
                KeyStroke.getPressed(SWT.F2, 0),
                getActionRegistry().getAction(GEFActionConstants.DIRECT_EDIT));
        }
        return keyHandler;
    }

    protected boolean performSaveAs() {
/*      SaveAsDialog dialog =
            new SaveAsDialog(getSite().getWorkbenchWindow().getShell());
        dialog.setOriginalFile(((IFileEditorInput) getEditorInput()).getFile());
        dialog.open();
        IPath path = dialog.getResult();

        if (path == null)
            return false;

        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        final IFile file = workspace.getRoot().getFile(path);

        WorkspaceModifyOperation op = new WorkspaceModifyOperation() {
            public void execute(final IProgressMonitor monitor)
                throws CoreException {
                try {
                    ByteArrayOutputStream out = new ByteArrayOutputStream();
                    createOutputStream(out);
                    file.create(
                        new ByteArrayInputStream(out.toByteArray()),
                        true,
                        monitor);
                    out.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        };

        try {
            new ProgressMonitorDialog(
                getSite().getWorkbenchWindow().getShell()).run(
                false,
                true,
                op);
            setInput(new FileEditorInput((IFile) file));
            getCommandStack().markSaveLocation();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }*/
}

