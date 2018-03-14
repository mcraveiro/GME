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

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.NonResizableEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.isis.gme.modeleditor.model.GmeDiagram;
import org.isis.gme.modeleditor.model.GmeSubpart;
import org.isis.gme.modeleditor.model.command.AddCommand;
import org.isis.gme.modeleditor.model.command.CreateCommand;
import org.isis.gme.modeleditor.model.command.SetConstraintCommand;

/**
 * GmeXYLayoutEditPolicy.java
 * 
 * @author Papszi
 */
public class GmeXYLayoutEditPolicy
	extends org.eclipse.gef.editpolicies.XYLayoutEditPolicy {

	protected Command createChangeConstraintCommand(
		EditPart child,
		Object constraint) {
		SetConstraintCommand locationCommand = new SetConstraintCommand();
		locationCommand.setPart((GmeSubpart) child.getModel());
		locationCommand.setLocation((Rectangle) constraint);
		return locationCommand;
	}

	protected EditPolicy createChildEditPolicy(EditPart child) {
		if (child instanceof NodeEditPart) {
			return new NonResizableEditPolicy();
		}
		return super.createChildEditPolicy(child);
	}

	protected Command getCreateCommand(CreateRequest request) {
		CreateCommand create = new CreateCommand();
		create.setParent((GmeDiagram) getHost().getModel());
		create.setChild((GmeSubpart) request.getNewObject());
		Rectangle constraint = (Rectangle) getConstraintFor(request);
		create.setLocation(constraint);
		create.setLabel("CreateCommand label text");
		return create;
	}

	protected Command getDeleteDependantCommand(Request request) {
		return null;
	}

	protected Command getOrphanChildrenCommand(Request request) {
		return null;
	}

	/**
	 * Returns the <code>Command</code> to resize a group of children.
	 * @param request the ChangeBoundsRequest
	 * @return the Command
	 */
/*	protected Command getResizeChildrenCommand(ChangeBoundsRequest request) {
		System.out.println("GmeXYLayoutEditPolicy.getResizeChildrenCommand");
		CompoundMoveCommand move = new CompoundMoveCommand();
		Command c;
		GraphicalEditPart child;
		List children = request.getEditParts();
		move.setGmeDiagram((GmeDiagram)getHost().getModel());

		for (int i = 0; i < children.size(); i++) {
			child = (GraphicalEditPart) children.get(i);
			c =	createChangeConstraintCommand(child, translateToModelConstraint(getConstraintFor(request, child)));
			move.add(c);
		}
		return move.unwrap();
	}

	protected Command getMoveChildrenCommand(Request request) {
		System.out.println("GmeXYLayoutEditPolicy.getMoveChildrenCommand");
		return getResizeChildrenCommand((ChangeBoundsRequest)request);
	}
*/
	protected Command createAddCommand(
		EditPart childEditPart,
		Object constraint) {

		GmeSubpart part = (GmeSubpart) childEditPart.getModel();
		Rectangle rect = (Rectangle) constraint;

		AddCommand add = new AddCommand();
		add.setParent((GmeDiagram) getHost().getModel());
		add.setChild(part);
		add.setLabel("AddCommand");
		add.setDebugLabel("GmeXYEP add subpart"); //$NON-NLS-1$
		SetConstraintCommand setConstraint = new SetConstraintCommand();

		setConstraint.setLocation(rect);
		setConstraint.setPart(part);
		setConstraint.setLabel("AddCommand");
		setConstraint.setDebugLabel("GmeXYEP setConstraint"); //$NON-NLS-1$
		return add.chain(setConstraint);
	}
}
