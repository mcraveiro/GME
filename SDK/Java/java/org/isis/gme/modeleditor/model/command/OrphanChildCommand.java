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

import java.util.List;

import org.eclipse.gef.commands.Command;
import org.isis.gme.modeleditor.model.GmeDiagram;
import org.isis.gme.modeleditor.model.GmeSubpart;

/**
 * OrphanChildCommand.java
 * 
 * @author Papszi
 */
public class OrphanChildCommand extends Command{

	private GmeDiagram diagram;
	private GmeSubpart child;
	private int index;

	public OrphanChildCommand () {
		super("GmeMessages.OrphanChildCommand_Label");
	}

	public void execute() {
		List children = diagram.getChildren();
		index = children.indexOf(child);
		diagram.removeChild(child);
	}

	public void redo() {
		diagram.removeChild(child);
	}

	public void setChild(GmeSubpart child) {
		this.child = child;
	}

	public void setParent(GmeDiagram parent) { 
		diagram = parent;
	}

	public void undo() {
		diagram.addChild(child,index);
	}

}
