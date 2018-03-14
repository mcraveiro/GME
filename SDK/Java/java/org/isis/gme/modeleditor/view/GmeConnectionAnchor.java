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
 * Created on Sep 23, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor.view;

import org.eclipse.draw2d.*;
import org.eclipse.draw2d.geometry.*;

/**
 * GmeConnectionAnchor.java
 * 
 * @author Papszi
 */
public class GmeConnectionAnchor extends AbstractConnectionAnchor{
	private Object direction;
	public boolean leftToRight = true;
	public int offsetH;
	public int offsetV;
	public boolean topDown = true;

	public GmeConnectionAnchor(IFigure owner) {
		super(owner);
	}

	/**
	 * @see org.eclipse.draw2d.AbstractConnectionAnchor#ancestorMoved(IFigure)
	 */
	public void ancestorMoved(IFigure figure) {
		if (figure instanceof ScalableFigure)
			return;
		super.ancestorMoved(figure);
	}

	public Point getLocation(Point reference) {
		Rectangle r = getOwner().getBounds();
		int x,y;
		if (topDown)
			y = r.y + offsetV;
		else
			y = r.bottom() - 1 - offsetV;

		if (leftToRight)
			x = r.x + offsetH;
		else
			x = r.right() - 1 - offsetH;
	
		Point p = new PrecisionPoint(x,y);
		getOwner().translateToAbsolute(p);
		return p;
	}

	public Point getReferencePoint(){
		return getLocation(null);
	}
}
