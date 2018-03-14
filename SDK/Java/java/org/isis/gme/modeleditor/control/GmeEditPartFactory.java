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
 
package org.isis.gme.modeleditor.control;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.isis.gme.modeleditor.model.Edge;
import org.isis.gme.modeleditor.model.GmeDiagram;
import org.isis.gme.modeleditor.model.Node;

/**
 * GmeEditPartFactory.java
 * 
 * @author Papszi
 */
public class GmeEditPartFactory implements EditPartFactory{

	public EditPart createEditPart(EditPart context, Object model) {
		EditPart child = null;
	
		if (model instanceof Node)
			child = new NodeEditPart();
		else if (model instanceof Edge)
			child = new EdgeEditPart();
		else if (model instanceof GmeDiagram)
			child = new GmeDiagramEditPart();
		child.setModel(model);
		return child;
	}

}
