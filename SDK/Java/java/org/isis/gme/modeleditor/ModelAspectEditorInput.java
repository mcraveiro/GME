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
 * Created on Sep 18, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;
import org.isis.gme.meta.MgaMetaAspect;
import org.isis.gme.meta.MgaMetaPart;
import org.isis.gme.mga.MgaFCO;
import org.isis.gme.mga.MgaModel;
import org.isis.gme.mga.MgaObject;
import org.isis.gme.mga.MgaPart;
import org.isis.gme.mga.MgaProject;
import org.isis.gme.mga.MgaSimpleConnection;
import org.isis.gme.mgadecorator.MgaDecorator;
import org.isis.gme.mgadecorator.MgaDecorator.Location;
import org.isis.gme.modeleditor.model.Edge;
import org.isis.gme.modeleditor.model.GmeSubpart;
import org.isis.gme.modeleditor.model.Node;
import org.isis.gme.modeleditor.model.Port;

/**
 * GmeEditorInput.java
 * 
 * @author Papszi
 */
public class ModelAspectEditorInput implements IEditorInput
{
    // mga fileds
    private MgaModel       model      = null;
    private MgaMetaAspect  aspect     = null;      
    private MgaProject     project    = null;
    private MgaPart[]      parts      = null;    
    
    // model fileds
    private List           nodes      = new ArrayList();
    private List           edges      = new ArrayList();
    private String         aspectName = null;
    
    // TODO: mga object-> model object map
     
    
    public ModelAspectEditorInput()
    {
    }
    
    public ModelAspectEditorInput( MgaModel model, MgaMetaAspect aspect )
    {
        this.model  = model;
        this.aspect = aspect;        
        aspectName  = aspect.getName();         
        fillModelParts();
    }
    
    public String getAspectName()
    {
        return aspectName; 
    }
    
    public List getNodes()
    {
        return nodes;
    }
    
    public List getEdges()
    {
        return edges;
    }
       
    private void fillModelParts()
    {
        project = model.getProject();                             
        parts   = model.getAspectParts(aspect).getAll();
    
        fillNodes();
        fillEdges();
        setRoutingPrefs();
    }
    
    private void fillNodes()
    {
        nodes.clear();
        for( int i=0; i<parts.length; ++i )
        {        
            MgaFCO           fco      = parts[i].getFCO();
            MgaPart.GmeAttrs attr     = parts[i].getGmeAttrs();
            MgaMetaPart      metaPart = new MgaMetaPart( parts[i].getMeta() );
            int              type     = fco.getObjType();
            
            if( type == MgaObject.OBJTYPE_ATOM || type == MgaObject.OBJTYPE_MODEL || type == MgaObject.OBJTYPE_REFERENCE )
            {
                MgaDecorator decorator = new MgaDecorator( fco.getRegistryValue( "decorator" ) );
                decorator.initialize( project, metaPart, fco );
                MgaDecorator.Size s = decorator.getPreferredSize();
                decorator.setLocation( decorator.new Location( attr.x,attr.y,attr.x+s.sizex,attr.y+s.sizey ) );
                //decorator.setActvie( false );

                // create node                
                Node n = new Node( decorator, fco );

                // create ports
                MgaFCO[] fcos = decorator.getPorts().getAll();
                for( int j=0; j<fcos.length; ++j )
                {                                        
                    Location loc = decorator.getPortLocation( fcos[j] );                                                    
                    Port p = new Port( n, fcos[j] );
                    p.setLocation( new Point(loc.sx,loc.sy) );
                    p.setSize( new Dimension(loc.ex-loc.sx,loc.ey-loc.sy) );
                    n.getPorts().add( p );
                }
        
                // add node
                nodes.add( n );
            }
        }
    }
    
    private void fillEdges()
    {
        edges.clear();
        for( int i=0; i<parts.length; ++i )
        {        
            MgaFCO   fco   = parts[i].getFCO();
            int      type  = fco.getObjType();

            if( type == MgaObject.OBJTYPE_CONNECTION )            
            {
                MgaSimpleConnection conn = new MgaSimpleConnection( fco );                                                
                
                Edge e = new Edge( conn );
                GmeSubpart src = findFCO( conn.getSrc() );
                GmeSubpart dst = findFCO( conn.getDst() );
                if( src != null && dst != null )
                {
                    e.setSource(src);
                    e.setTarget(dst);
                    edges.add( e );                      
                }
            }
        }                
    }
    
    private GmeSubpart findFCO( MgaFCO fco )
    {
        // TODO: make it faster with hasmaps!!!
        Iterator it = nodes.iterator();
        while( it.hasNext() )
        {
            Node   n = (Node)it.next();
            if( fco.equals(n.getFCO()) )
                return n;
            Iterator it2 = n.getPorts().iterator();
            while( it2.hasNext() )
            {
                Port p = (Port)it2.next();
                if( fco.equals(p.getFCO()) )
                    return p;                                               
            }            
        }       
        return null;
    }
    
    public void setRoutingPrefs()
    {
        // ports
        Iterator it = nodes.iterator();
        while( it.hasNext() )
        {
            Node n = (Node)it.next();
            Iterator it2 = n.getPorts().iterator();
            while( it2.hasNext() )
            {
                Port port = (Port)it2.next();
                port.calcRoutingPrefs();                
            }            
        }                
        
        // TODO: connections                
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
        return "empty";
    } 
 
    public IPersistableElement getPersistable()
    {
        return null; 
    }
            
    public String getToolTipText()
    {
        return "GME Model Editor, no toolTipText yet";
    } 
    
    public Object getAdapter(Class type) 
    {        
        return null;
    }
}

