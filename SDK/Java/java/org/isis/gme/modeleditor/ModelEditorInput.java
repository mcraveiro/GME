/*
 * Created on Oct 8, 2003
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package org.isis.gme.modeleditor;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;
import org.isis.gme.meta.MgaMetaAspect;
import org.isis.gme.mga.MgaModel;

/**
 * @author bogyom
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class ModelEditorInput implements IEditorInput
{
    private MgaModel model                   = null;  
    private List     modelAspectEditorInputs = new ArrayList();
    
    public ModelEditorInput( MgaModel model )
    {
        this.model = model;

        // fill modelAspectEditorInputs
        MgaMetaAspect aspects[] = model.getMetaModel().getAspects().getAll();
        for( int i=0; i<aspects.length; ++i )
            modelAspectEditorInputs.add( new ModelAspectEditorInput( model, aspects[i]) );       
    }   
    
    public List getModelAspectEditorInputs()
    {
        return modelAspectEditorInputs;        
    }
    
    public boolean exists()
    {
        return true;
    } 

    public ImageDescriptor getImageDescriptor()
    {
        return ImageDescriptor.createFromFile(this.getClass(), "empty.gif");
    }
 
    public String getName()
    {
        if( model != null)
            return model.getName();
        else
            return "error";
    }
 
    public IPersistableElement getPersistable()
    {
        return null; 
    }
            
    public String getToolTipText()
    {
        return null;
    }
    
    public Object getAdapter(Class type) 
    {        
        return null;
    }
    

}
