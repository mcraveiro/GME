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

import org.eclipse.draw2d.geometry.Rectangle;
import org.isis.gme.modeleditor.model.GmeDiagram;
import org.isis.gme.modeleditor.model.GmeSubpart;

/**
 * CreateCommand.java
 * 
 * @author Papszi
 */
public class CreateCommand extends org.eclipse.gef.commands.Command{

	private GmeSubpart child;
	private Rectangle rect;
	private GmeDiagram parent;
	private int index = -1;

	public CreateCommand() {
		super("CreateCommand label");
	}

	public void execute() {
		if (rect != null) {
			child.setLocation(rect.getLocation());
			if (!rect.isEmpty())
				child.setSize(rect.getSize());
		}
		if( index < 0 )
			parent.addChild(child);
		else
			parent.addChild(child,index);
	}

	public GmeDiagram getParent() {
		return parent;
	}

	public void redo() {
		if (rect != null) {
			child.setLocation(rect.getLocation());
			child.setSize    (rect.getSize());
		}
		if( index < 0 )
			parent.addChild(child);
		else
			parent.addChild(child,index);
	}

	public void setChild(GmeSubpart subpart) {
		child = subpart;
	}

	public void setIndex( int index ){
		this.index = index;
	}

	public void setLocation (Rectangle r) {
		rect = r;
	}

	public void setParent(GmeDiagram newParent) {
		parent = newParent;
	}

	public void undo() {
		parent.removeChild(child);
	}


}
