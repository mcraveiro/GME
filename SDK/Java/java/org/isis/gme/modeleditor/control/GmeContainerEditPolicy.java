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
 
package org.isis.gme.modeleditor.control;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.ContainerEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gef.requests.GroupRequest;
import org.isis.gme.modeleditor.model.GmeDiagram;
import org.isis.gme.modeleditor.model.GmeSubpart;
import org.isis.gme.modeleditor.model.command.CompoundMoveCommand;
import org.isis.gme.modeleditor.model.command.OrphanChildCommand;

/**
 * GmeContainerEditPolicy.java
 * 
 * @author Papszi
 */
public class GmeContainerEditPolicy extends ContainerEditPolicy{

	protected Command getCreateCommand(CreateRequest request) {
		return null;
	}

	public Command getOrphanChildrenCommand(GroupRequest request) {
		List parts = request.getEditParts();
		CompoundMoveCommand result = 
			new CompoundMoveCommand("GmeMessages.GmeContainerEditPolicy_OrphanCommandLabelText");
		for (int i = 0; i < parts.size(); i++) {
			OrphanChildCommand orphan = new OrphanChildCommand();
			orphan.setChild((GmeSubpart)((EditPart)parts.get(i)).getModel());
			orphan.setParent((GmeDiagram)getHost().getModel());
			orphan.setLabel("GmeMessages.GmeElementEditPolicy_OrphanCommandLabelText");
			result.add(orphan);
		}
		return result.unwrap();
	}

}
