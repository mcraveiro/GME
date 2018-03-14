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

import org.eclipse.gef.AccessibleEditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.swt.accessibility.AccessibleEvent;


/**
 * GmeContainerEditPart.java
 * 
 * @author Papszi
 */
abstract public class GmeContainerEditPart extends GmeEditPart {

	/* (non-Javadoc)
	 * @see org.isis.gme.modeleditor.control.GmeEditPart#createAccessible()
	 */
	protected AccessibleEditPart createAccessible() {
		return new AccessibleGraphicalEditPart(){
			public void getName(AccessibleEvent e) {
				e.result = "GmeContainerEditPart.createAccessible";
			}
		};
	}

	/**
	 * Installs the desired EditPolicies for this.
	 */
	protected void createEditPolicies() {
		super.createEditPolicies();
		installEditPolicy(EditPolicy.CONTAINER_ROLE, new GmeContainerEditPolicy());
		installEditPolicy(EditPolicy.LAYOUT_ROLE, new GmeXYLayoutEditPolicy());
	}
}
