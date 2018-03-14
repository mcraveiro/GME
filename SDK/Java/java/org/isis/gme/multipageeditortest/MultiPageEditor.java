/*
 * Created on Oct 7, 2003
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package org.isis.gme.multipageeditortest;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.isis.gme.modeleditor.ModelEditor;

/**
 * @author bogyom
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class MultiPageEditor extends EditorPart
{
    private CTabFolder  tabFolder = null;
    private CTabItem    tabItem1  = null;
    private CTabItem    tabItem2  = null;
    
    private ModelEditor page1     = null;      
    //private Label       page1     = null;
    private Label       page2     = null;
    
    public MultiPageEditor()
    {
        super();
        
        page1 = new ModelEditor();
    }
    
    public void createPartControl(Composite parent)
    {   
        //Layout l = parent.getLayout();
        //System.out.println( "basszuskulcs34:" + l );    
        
        tabFolder = new CTabFolder(parent, SWT.BOTTOM);               
               
        /*tabFolder.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e)
            {
                CTabItem item = (CTabItem)e.item;
                if( item.getText().equals("alma") )
                {
                    //page1.setVisible( true );
                    //page2.setVisible( false );                    
                }
                else
                {
                    //page1.setVisible( false );
                    //page2.setVisible( true );
                }
            }            
        });*/
        
        tabItem1 = new CTabItem(tabFolder, SWT.NULL);
        tabItem1.setText("alma");
        
        tabItem2 = new CTabItem(tabFolder, SWT.NULL);
        tabItem2.setText("korte");
                       
        Composite tabPage1 = new Composite (tabFolder, SWT.NONE);
        tabPage1.setLayout( new FillLayout() );
        tabPage1.setVisible( true );
        
        Composite tabPage2 = new Composite (tabFolder, SWT.NONE);
        tabPage2.setLayout( new FillLayout() );
        tabPage2.setVisible( true );
        
        page1.createPartControl( tabPage1 );        
        //page1     = new Label( tabPage1, 0 );
//      page1.setText("ez az elso");
        page2     = new Label( tabPage2, 0 );
        page2.setText("ez a masodik");
        
        tabItem1.setControl( tabPage1 );
        tabItem2.setControl( tabPage2 );                
    }
    
    public void doSave(IProgressMonitor monitor)
    {
    }
    
    public void doSaveAs()
    {
    }
    
    public void gotoMarker(IMarker marker)
    {
    }
    
    public void init(IEditorSite site, IEditorInput input) throws PartInitException
    {
        setSite(site);
        setInput(input);
        
        page1.init( site, input );        
    }

    public boolean isDirty()
    {
        return false;
    }

    public boolean isSaveAsAllowed()
    {
        return false;
    }
        
    public void setFocus()
    {
    }
}
