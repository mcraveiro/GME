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
 * Created on Oct 3, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor.view;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.RotatableDecoration;
import org.eclipse.draw2d.geometry.Rectangle;
import org.isis.gme.modeleditor.tools.GmeConnectionEndpointLocator;

/**
 * EdgeFigure.java
 * 
 * @author Papszi
 */
public class EdgeFigure extends PolylineConnection{

	private int lineStyle = Graphics.LINE_SOLID;
	
	private GmeLabel destLabel1 = null;
	private GmeLabel destLabel2 = null;
	private GmeLabel srcLabel1 = null;
	private GmeLabel srcLabel2 = null;
	private GmeLabel label = null; 

	private GmeConnectionEndpointLocator destLabel1Pos = null;
	private GmeConnectionEndpointLocator destLabel2Pos = null;
	private GmeConnectionEndpointLocator srcLabel1Pos = null;
	private GmeConnectionEndpointLocator srcLabel2Pos = null;
	private GmeConnectionEndpointLocator labelPos = null;
	
	public EdgeFigure(String dest1, String dest2, String src1, String src2, String labelString){
		super();
		if (dest1 != null && !dest1.equals("")){ 
			destLabel1Pos = new GmeConnectionEndpointLocator(this, true);
			destLabel1Pos.setVDistance(-2);
			destLabel1Pos.setUDistance(2);
			destLabel1 = new GmeLabel(dest1);
			this.add(destLabel1, destLabel1Pos);
		}
		if (dest2 != null && !dest2.equals("")){ 
			destLabel2Pos = new GmeConnectionEndpointLocator(this, true);
			destLabel2Pos.setVDistance(2);
			destLabel2Pos.setUDistance(2);
			destLabel2 = new GmeLabel(dest2);
			this.add(destLabel2, destLabel2Pos);
		}
		if (src1 != null && !src1.equals("")){
			srcLabel1Pos = new GmeConnectionEndpointLocator(this, false);
			srcLabel1Pos.setVDistance(-2);
			srcLabel1Pos.setUDistance(2);
			srcLabel1 = new GmeLabel(src1);
			this.add(srcLabel1, srcLabel1Pos);
		}
		if (src2 != null && !src2.equals("")){
			srcLabel2Pos = new GmeConnectionEndpointLocator(this, false);
			srcLabel2Pos.setVDistance(2);
			srcLabel2Pos.setUDistance(2);
			srcLabel2 = new GmeLabel(src2);
			this.add(srcLabel2, srcLabel2Pos);
		}
	}

	public EdgeFigure(){
		super();
	}

	public void setLineStyle(int ls){
		lineStyle = ls;
	}

	public void repaint(){
		Rectangle rect = getBounds().expand(40,40);
		super.repaint();
		/*if (destLabel1Pos != null && destLabel1 != null){
			destLabel1Pos.relocate(destLabel1);
			destLabel1.repaint();
		}*/
	}

	/* (non-Javadoc)
	 * @see org.eclipse.draw2d.PolylineConnection#getSourceDecoration()
	 */
	public RotatableDecoration getSourceDecoration() {
		return super.getSourceDecoration();
	}

	/* (non-Javadoc)
	 * @see org.eclipse.draw2d.PolylineConnection#getTargetDecoration()
	 */
	public RotatableDecoration getTargetDecoration() {
		return super.getTargetDecoration();
	}

	/* (non-Javadoc)
	 * @see org.eclipse.draw2d.Shape#outlineShape(org.eclipse.draw2d.Graphics)
	 */
	protected void outlineShape(Graphics g) {
		int prevLineStyle = g.getLineStyle();
		g.setLineStyle(lineStyle);
		super.outlineShape(g);
		g.setLineStyle(prevLineStyle);
	}
	
	public void adjustLabelPlacement(){
		if (destLabel1 != null){ 
			destLabel1Pos.relocate(destLabel1);
		}
		if (destLabel2 != null){ 
			destLabel2Pos.relocate(destLabel2);
		}
		if (srcLabel1 != null){ 
			srcLabel1Pos.relocate(srcLabel1);
		}
		if (srcLabel2 != null){ 
			srcLabel2Pos.relocate(srcLabel2);
		}
	}
}
