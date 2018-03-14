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
 * Created on Sep 25, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor.control;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.CreateConnectionRequest;
import org.eclipse.gef.requests.ReconnectRequest;
import org.isis.gme.modeleditor.model.Edge;
import org.isis.gme.modeleditor.model.GmeSubpart;
import org.isis.gme.modeleditor.model.command.ConnectionCommand;
import org.isis.gme.modeleditor.view.NodeFigure;


/**
 * GmeNodeEditPolicy.java
 * 
 * @author Papszi
 */
public class GmeNodeEditPolicy extends org.eclipse.gef.editpolicies.GraphicalNodeEditPolicy{

	/*protected Connection createDummyConnection(Request req) {
		PolylineConnection conn = FigureFactory.createNewEdge(null);
		return conn;
	}*/

	protected Command getConnectionCompleteCommand(CreateConnectionRequest request) {	
		ConnectionCommand command = (ConnectionCommand)request.getStartCommand();
		command.setTarget(getGmeSubpart());
		ConnectionAnchor ctor = getGmeEditPart().getTargetConnectionAnchor(request);
		if (ctor == null)
			return null;
		command.setTargetTerminal(getGmeEditPart().mapConnectionAnchorToTerminal(ctor));
		return command;
	}

	protected Command getConnectionCreateCommand(CreateConnectionRequest request) {
		ConnectionCommand command = new ConnectionCommand();
		command.setEdge(new Edge());
		command.setSource(getGmeSubpart());
		ConnectionAnchor ctor = getGmeEditPart().getSourceConnectionAnchor(request);
		command.setSourceTerminal(getGmeEditPart().mapConnectionAnchorToTerminal(ctor));
		request.setStartCommand(command);
		return command;
	}

	protected GmeEditPart getGmeEditPart() {
		return (GmeEditPart) getHost();
	}

	protected GmeSubpart getGmeSubpart() {
		return (GmeSubpart) getHost().getModel();
	}

	protected Command getReconnectTargetCommand(ReconnectRequest request) {
		/*if (getGmeSubpart() instanceof LiveOutput || 
			getGmeSubpart() instanceof GroundOutput)
				return null;
		*/
		ConnectionCommand cmd = new ConnectionCommand();
		cmd.setEdge((Edge)request.getConnectionEditPart().getModel());

		ConnectionAnchor ctor = getGmeEditPart().getTargetConnectionAnchor(request);
		cmd.setTarget(getGmeSubpart());
		cmd.setTargetTerminal(getGmeEditPart().mapConnectionAnchorToTerminal(ctor));
		return cmd;
	}

	protected Command getReconnectSourceCommand(ReconnectRequest request) {
		ConnectionCommand cmd = new ConnectionCommand();
		cmd.setEdge((Edge)request.getConnectionEditPart().getModel());

		ConnectionAnchor ctor = getGmeEditPart().getSourceConnectionAnchor(request);
		cmd.setSource(getGmeSubpart());
		cmd.setSourceTerminal(getGmeEditPart().mapConnectionAnchorToTerminal(ctor));
		return cmd;
	}

	protected NodeFigure getNodeFigure() {
		return (NodeFigure)((GraphicalEditPart)getHost()).getFigure();
	}

}
