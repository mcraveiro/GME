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
 * Created on Oct 9, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor.tools;

import java.util.Iterator;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.tools.DragEditPartsTracker;
import org.isis.gme.modeleditor.control.NodeEditPart;
import org.isis.gme.modeleditor.model.GmeDiagram;
import org.isis.gme.modeleditor.model.Node;
import org.isis.gme.modeleditor.model.command.CompoundMoveCommand;

/**
 * GmeDragEditPartsTracker.java
 * 
 * @author Papszi
 */
public class GmeDragEditPartsTracker extends DragEditPartsTracker{

	/**
	 * Constructs a new DragEditPartsTracker with the given source edit part.
	 * @param sourceEditPart the source edit part
	 */
	public GmeDragEditPartsTracker(EditPart sourceEditPart) {
		super(sourceEditPart);
	}

	protected Command getCommand() {
		CompoundMoveCommand command = new CompoundMoveCommand();
		command.setDebugLabel("Drag Object Tracker");//$NON-NLS-1$

		Iterator iter = getOperationSet().iterator();

		Request  request = getTargetRequest();
		request.setType(isMove() ? REQ_MOVE : REQ_ORPHAN);

		while (iter.hasNext()) {
			EditPart editPart = (EditPart)iter.next();
			command.add(editPart.getCommand(request));
			if (editPart instanceof NodeEditPart){
				command.setGmeDiagram((GmeDiagram)((Node)editPart.getModel()).getParent());
			}
		}

		//If reparenting, add all editparts to target editpart.
		if (!isMove()) {
			request.setType(REQ_ADD);
			if (getTargetEditPart() == null)
				command.add(UnexecutableCommand.INSTANCE);
			else
				command.add(getTargetEditPart().getCommand(getTargetRequest()));
		}
		return command;
	}
}
