/*
 * Copyright (c) 2003, Vanderbilt University
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
 * Created on Sep 10, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */

package org.isis.gme.modeleditor;

import java.util.Iterator;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;

public class ModelEditor extends EditorPart
{
    private CTabFolder  tabFolder = null;
          
    public void createPartControl(Composite parent)
    {  
        tabFolder = new CTabFolder(parent, SWT.BOTTOM);
                             
        ModelEditorInput input = (ModelEditorInput)getEditorInput();
                
        Iterator it = input.getModelAspectEditorInputs().iterator();
        while( it.hasNext() )
        {
            ModelAspectEditorInput aspInput = (ModelAspectEditorInput)it.next();
    
            // create and initialize visual tab page
            CTabItem tabItem = new CTabItem( tabFolder, SWT.NULL );
            tabItem.setText( aspInput.getAspectName() );            
            Composite tabPage = new Composite( tabFolder, SWT.NONE );
            tabPage.setLayout( new FillLayout() );
            tabPage.setVisible( true );            
            tabItem.setControl( tabPage );
    
            // create and initialize aspect editor
            ModelAspectEditor aspectEditor = new ModelAspectEditor();
            aspectEditor.init( getEditorSite(), aspInput );
            aspectEditor.createPartControl( tabPage );
        }
               
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
    
    public void init(IEditorSite site, IEditorInput in) throws PartInitException
    {        
        setSite(site);
        setInput(in);
        setTitle(in.getName());                     
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
        tabFolder.setFocus();
    }
}
