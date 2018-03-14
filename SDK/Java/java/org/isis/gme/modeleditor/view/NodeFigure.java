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
 
package org.isis.gme.modeleditor.view;

import org.eclipse.swt.widgets.Display;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Vector;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.isis.gme.mgadecorator.MgaDecorator;

/**
 * NodeView.java
 * 
 * @author Papszi
 */
public class NodeFigure extends Figure{

	private MgaDecorator decorator = new MgaDecorator();

	protected HashMap connectionAnchors = new HashMap(7);
	protected Vector inputConnectionAnchors = new Vector(2,2);
	protected Vector outputConnectionAnchors = new Vector(2,2);
    
	public NodeFigure(MgaDecorator decorator) 
    {
        //if( Math.random() > 0.5 ) 
		this.decorator = decorator;
	}

    protected void paintFigure(Graphics graphics)
    {       
        SWTGraphics swtGraphics = (SWTGraphics)graphics;        
        Rectangle   bounds      = getBounds();
        
        //graphics.setForegroundColor( color );
        //graphics.drawRectangle( bounds.x, bounds.y, bounds.width-5, bounds.height-5 );
        //graphics.setBackgroundColor( new Color( Display.getDefault(), 255, 0, 0 ) );
        //graphics.fillRectangle(getBounds());
        
        //swtGraphics.pushState();                                  
        int x = swtGraphics.translateX + bounds.x;
        int y = swtGraphics.translateY + bounds.y;
        
        //swtGraphics.setClip( new Rectangle());
               
        
        /*org.eclipse.swt.graphics.Rectangle r = swtGraphics.gc.getClipping();
        r.x -= 50;
        r.height += 50;
        r.width += 100;
        swtGraphics.gc.setClipping( r.x-50, r.y, r.width + 100, r.height + 50 );
        swtGraphics.clipRect()        
        System.out.println( swtGraphics.gc.getClipping() );*/
        
        //swtGraphics.checkPaint();
        
        Color cl = new Color(Display.getDefault(),255,255,255);
        swtGraphics.gc.setBackground(cl);
        
        decorator.setLocation(decorator.new Location( x, y, x + bounds.width, y + bounds.height ));         
        decorator.draw( swtGraphics.gc.handle );                       
        //swtGraphics.popState();
        
        
        //System.out.println( "alma:" + toString() + " " + bounds.toString() + " " + swtGraphics.translateX );       
    }
	

	public ConnectionAnchor connectionAnchorAt(Point p) {
		ConnectionAnchor closest = null;
		long min = Long.MAX_VALUE;

		Enumeration e = getSourceConnectionAnchors().elements();
		while (e.hasMoreElements()) {
			ConnectionAnchor c = (ConnectionAnchor) e.nextElement();
			Point p2 = c.getLocation(null);
			long d = p.getDistance2(p2);
			if (d < min) {
				min = d;
				closest = c;
			}
		}
		e = getTargetConnectionAnchors().elements();
		while (e.hasMoreElements()) {
			ConnectionAnchor c = (ConnectionAnchor) e.nextElement();
			Point p2 = c.getLocation(null);
			long d = p.getDistance2(p2);
			if (d < min) {
				min = d;
				closest = c;
			}
		}
		return closest;
	}

	public ConnectionAnchor getConnectionAnchor(String terminal) {
		return (ConnectionAnchor)connectionAnchors.get(terminal);
	}

	public String getConnectionAnchorName(ConnectionAnchor c){
		Iterator iter = connectionAnchors.keySet().iterator();
		String key;
		while (iter.hasNext()){
			key = (String)iter.next();
			if (connectionAnchors.get(key).equals(c))
				return key;
		}
		return null;
	}

	public ConnectionAnchor getSourceConnectionAnchorAt(Point p) {
		ConnectionAnchor closest = null;
		long min = Long.MAX_VALUE;

		Enumeration e = getSourceConnectionAnchors().elements();
		while (e.hasMoreElements()) {
			ConnectionAnchor c = (ConnectionAnchor) e.nextElement();
			Point p2 = c.getLocation(null);
			long d = p.getDistance2(p2);
			if (d < min) {
				min = d;
				closest = c;
			}
		}
		return closest;
	}

	public Vector getSourceConnectionAnchors() {
		return outputConnectionAnchors;
	}

	public ConnectionAnchor getTargetConnectionAnchorAt(Point p) {
		ConnectionAnchor closest = null;
		long min = Long.MAX_VALUE;

		Enumeration e = getTargetConnectionAnchors().elements();
		while (e.hasMoreElements()) {
			ConnectionAnchor c = (ConnectionAnchor) e.nextElement();
			Point p2 = c.getLocation(null);
			long d = p.getDistance2(p2);
			if (d < min) {
				min = d;
				closest = c;
			}
		}
		return closest;
	}

	public Vector getTargetConnectionAnchors() {
		return inputConnectionAnchors;
	}
}
