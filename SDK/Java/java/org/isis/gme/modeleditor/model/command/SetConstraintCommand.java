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
 * Created on Sep 30, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor.model.command;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.isis.gme.modeleditor.model.GmeSubpart;
/**
 * SetConstraintCommand.java
 * 
 * @author Papszi
 */
public class SetConstraintCommand extends org.eclipse.gef.commands.Command{

	private Point newPos;
	private Dimension newSize;
	private Point oldPos;
	private Dimension oldSize;
	private GmeSubpart part;

	public void execute() {
		oldSize = part.getSize();
		oldPos  = part.getLocation();
		part.setLocation(newPos);
		part.setSize(newSize);
	}

	public String getLabel(){
		if (oldSize.equals(newSize))
			return "Location";
		return "Resize";
	}

	public void redo() {
		part.setSize(newSize);
		part.setLocation(newPos);
	}

	public void setLocation(Rectangle r){
		setLocation(r.getLocation());
		setSize(r.getSize());
	}

	public void setLocation(Point p) {
		newPos = p;
	}

	public void setPart(GmeSubpart part) {
		this.part = part;
	}

	public void setSize(Dimension p) {
		newSize = p;
	}

	public void undo() {
		part.setSize(oldSize);
		part.setLocation(oldPos);
	}

}
