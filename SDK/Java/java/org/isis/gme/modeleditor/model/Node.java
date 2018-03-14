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
 * Last modified on 10/23/03
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
/*
 * Node.java
 *
 * Created on September 8, 2003, 4:36 PM
 */

package org.isis.gme.modeleditor.model;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.swt.graphics.Image;
import org.isis.gme.mga.MgaFCO;
import org.isis.gme.mga.MgaObject;
import org.isis.gme.mgadecorator.MgaDecorator;
import org.isis.gme.modeleditor.autorouter.Box;
/**
 *
 * @author  bogyom
 */
public class Node extends GmeSubpart
{   
	private static int count;
    
    private MgaFCO fco = null;
	private MgaDecorator decorator;
	private Box box;
        
    // ports
    ArrayList ports = new ArrayList();

	public Node(MgaDecorator decorator, MgaFCO fco) {
		this.decorator = decorator;
		this.fco = fco;
		size.width = decorator.getPreferredSize().sizex;
		size.height= decorator.getPreferredSize().sizey;
		location.x = decorator.getLocation().sx;
		location.y = decorator.getLocation().sy;
		port = new org.isis.gme.modeleditor.autorouter.Port(this.location.x, this.location.y, this.location.x+this.size.width, this.location.y+this.size.height);
		setRoutingPrefs();
	}

	public Node(GmeSubpart parent, MgaDecorator decorator, MgaFCO fco) {
		super(parent);
		this.decorator = decorator;
		this.fco = fco;
		size.width = decorator.getPreferredSize().sizex;
		size.height= decorator.getPreferredSize().sizey;
		location.x = decorator.getLocation().sx;
		location.y = decorator.getLocation().sy;
		port = new org.isis.gme.modeleditor.autorouter.Port(this.location.x, this.location.y, this.location.x+this.size.width, this.location.y+this.size.height);
		setRoutingPrefs();
	}
    
    protected boolean isOverlapped()
    {
        GmeDiagram diagram = (GmeDiagram)getParent();

        Iterator it = diagram.getChildren().iterator();
        while( it.hasNext() )
        {
            Node node2  = (Node)it.next();
            if( node2 == this )
                continue;

            Point     loc2  = node2.getLocation();
            Dimension size2 = node2.getSize();                      
        
            boolean left   = loc2.x>=location.x && loc2.x<=location.x+size.width;
            boolean top    = loc2.y>=location.y && loc2.y<=location.y+size.height;
            boolean right  = loc2.x+size2.width>=location.x && loc2.x+size2.width<=location.x+size.width;
            boolean bottom = loc2.y+size2.height>=location.y && loc2.y+size2.height<=location.y+size.height;
        
            if( (left && top) || (left && bottom) || (right && top) || (right && bottom) )
                return true;
            
            left   = location.x>=loc2.x && location.x<=loc2.x+size2.width;
            top    = location.y>=loc2.y && location.y<=loc2.y+size2.height;
            right  = location.x+size.width>=loc2.x && location.x+size.width<=loc2.x+size2.width;
            bottom = location.y+size.height>=loc2.y && location.y+size.height<=loc2.y+size2.height;
        
            if( (left && top) || (left && bottom) || (right && top) || (right && bottom) )
                return true;                
        }
        return false;        
    }
    
    protected boolean hasBadPosition()
    {
        if( location.x < 0 || location.y < 0 )
            return true;
        return isOverlapped();
    }
    
    public void autoPlace()
    {
        if( !hasBadPosition() )
            return;
        
        // try random locations near (increesing radius)
        Random rand = new Random();            
        int    x    = location.x;
        int    y    = location.y;            
        for( int i=0; i<100; ++i )
        {
            location.x = (int)(x + (i * rand.nextGaussian()));
            location.y = (int)(y + (i * rand.nextGaussian()));
            if( !hasBadPosition() )           
                return;                           
        }   
    
        // find max loc
        GmeDiagram diagram = (GmeDiagram)getParent();
        Iterator it = diagram.getChildren().iterator();
        int ymax = y;
        while( it.hasNext() )
        {
            Node n  = (Node)it.next();
            int y1  = n.getLocation().y + n.getSize().height;            
            if( y1 > ymax )
                ymax = y1;
        }        
        location.y = ymax + 5;
        location.x = 0;
    }
    
    
    public void setRoutingPrefs()
    {
        String routingprefs = fco.getRegistryValue( MgaFCO.AUTOROUTER_PREF );
        if( routingprefs.length() == 0 )
        {
            if( fco.getObjType() == MgaObject.OBJTYPE_ATOM )
            {
                port.setAttributes( org.isis.gme.modeleditor.autorouter.Port.START_ON_ALL
                                  | org.isis.gme.modeleditor.autorouter.Port.END_ON_ALL
                                  );                
            }
            else
            {
                port.setAttributes( org.isis.gme.modeleditor.autorouter.Port.START_ON_TOP
                                  | org.isis.gme.modeleditor.autorouter.Port.START_ON_BOTTOM
                                  | org.isis.gme.modeleditor.autorouter.Port.END_ON_TOP
                                  | org.isis.gme.modeleditor.autorouter.Port.END_ON_BOTTOM
                                  );
            }
        }
        else
        {
            int prefs = 0;
            
            if( routingprefs.indexOf( "N" ) != -1 ) prefs |= org.isis.gme.modeleditor.autorouter.Port.START_ON_TOP;
            if( routingprefs.indexOf( "E" ) != -1 ) prefs |= org.isis.gme.modeleditor.autorouter.Port.START_ON_RIGHT;
            if( routingprefs.indexOf( "S" ) != -1 ) prefs |= org.isis.gme.modeleditor.autorouter.Port.START_ON_BOTTOM;
            if( routingprefs.indexOf( "W" ) != -1 ) prefs |= org.isis.gme.modeleditor.autorouter.Port.START_ON_LEFT;
            if( routingprefs.indexOf( "n" ) != -1 ) prefs |= org.isis.gme.modeleditor.autorouter.Port.END_ON_TOP;
            if( routingprefs.indexOf( "e" ) != -1 ) prefs |= org.isis.gme.modeleditor.autorouter.Port.END_ON_RIGHT;
            if( routingprefs.indexOf( "s" ) != -1 ) prefs |= org.isis.gme.modeleditor.autorouter.Port.END_ON_BOTTOM;
            if( routingprefs.indexOf( "w" ) != -1 ) prefs |= org.isis.gme.modeleditor.autorouter.Port.END_ON_LEFT;
            
            port.setAttributes( prefs );
        }       
    }

	public MgaDecorator getDecorator(){
		return decorator;
	}
    
    public MgaFCO getFCO(){
        return fco;
    }
    
    public void setBox(Box b){
    	box = b;
    }
    
	public Box getBox(){
		return box;
	}

	/* (non-Javadoc)
	 * @see org.isis.gme.modeleditor.model.GmeSubpart#getIconImage()
	 */
	public Image getIconImage() {
		// TODO Auto-generated method stub
		return null;
	}
	/* (non-Javadoc)
	 * @see org.isis.gme.modeleditor.model.GmeSubpart#getNewID()
	 */
	protected String getNewID() {
		return Integer.toString(count++);
	}
	
	public List getPorts(){
		return (List)ports;
	}      
}
