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
 * Created on Sep 29, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor.model.command;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.isis.gme.modeleditor.model.Edge;
import org.isis.gme.modeleditor.model.GmeDiagram;
import org.isis.gme.modeleditor.model.GmeSubpart;

/**
 * DeleteCommand.java
 * 
 * @author Papszi
 */
public class DeleteCommand extends Command{

	private GmeSubpart child;
	private GmeDiagram parent;
	private int index = -1;
	private List sourceConnections = new ArrayList();
	private List targetConnections = new ArrayList();

	public DeleteCommand() {
		super("Delete command");
	}

	private void deleteConnections(GmeSubpart part) {
		if (part instanceof GmeDiagram) {
			List children = ((GmeDiagram)part).getChildren();
			for (int i = 0; i < children.size(); i++)
				deleteConnections((GmeSubpart)children.get(i));
		}
		sourceConnections.addAll(part.getSourceConnections());
		for (int i = 0; i < sourceConnections.size(); i++) {
			Edge edge = (Edge)sourceConnections.get(i);
			edge.detachSource();
			edge.detachTarget();
		}
		targetConnections.addAll(part.getTargetConnections());
		for (int i = 0; i < targetConnections.size(); i++) {
			Edge edge = (Edge)targetConnections.get(i);
			edge.detachSource();
			edge.detachTarget();
		}
	}

	public void execute() {
		primExecute();
	}

	protected void primExecute() {
		deleteConnections(child);
		index = parent.getChildren().indexOf(child);
		parent.removeChild(child);
	}

	public void redo() {
		primExecute();
	}

	private void restoreConnections() {
		for (int i = 0; i < sourceConnections.size(); i++) {
			Edge edge = (Edge)sourceConnections.get(i);
			edge.attachSource();
			edge.attachTarget();
		}
		sourceConnections.clear();
		for (int i = 0; i < targetConnections.size(); i++) {
			Edge edge = (Edge)targetConnections.get(i);
			edge.attachSource();
			edge.attachTarget();
		}
		targetConnections.clear();
	}

	public void setChild (GmeSubpart c) {
		child = c;
	}

	public void setParent(GmeDiagram p) {
		parent = p;
	}

	public void undo() {
		parent.addChild(child, index);
		restoreConnections();
	}
}
