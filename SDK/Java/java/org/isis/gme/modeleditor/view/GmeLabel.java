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
 * Created on Oct 17, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor.view;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.widgets.Display;

/**
 * GmeLabel.java
 * 
 * @author Papszi
 */
public class GmeLabel extends Label{

	static Font GME_EDGE_FONT = new Font(Display.getDefault(),new FontData("GmeEdgeFont",7,0));
	 
	public GmeLabel(String text){
		super(text);
	}

	/* (non-Javadoc)
	 * @see org.eclipse.draw2d.Figure#paintFigure(org.eclipse.draw2d.Graphics)
	 */
	protected void paintFigure(Graphics graphics) {
		SWTGraphics swtGraphics = (SWTGraphics)graphics;
		graphics.setFont(GME_EDGE_FONT);
		org.eclipse.swt.internal.win32.OS.SetTextAlign(swtGraphics.gc.handle,0);
		
		super.paintFigure(graphics);
		//Rectangle bounds = getTextBounds();
		//graphics.drawText(getSubStringText(), bounds.x,bounds.y);
	}
}
