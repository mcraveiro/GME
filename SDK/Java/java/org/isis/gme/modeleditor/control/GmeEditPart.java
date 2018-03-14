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

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.List;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.AccessibleEditPart;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.requests.DropRequest;
import org.isis.gme.modeleditor.model.Edge;
import org.isis.gme.modeleditor.model.GmeSubpart;
import org.isis.gme.modeleditor.view.NodeFigure;

/**
 * GmeEditPart.java
 * 
 * @author Papszi
 */
abstract public class GmeEditPart extends org.eclipse.gef.editparts.AbstractGraphicalEditPart implements PropertyChangeListener { //, NodeEditPart {
	private AccessibleEditPart acc;               

	public void activate() {
		if (isActive())
			return;
		super.activate();
		getGmeSubpart().addPropertyChangeListener(this);
	}
    
    /*public void performRequest(Request request)
    {
        if( request.getType() == REQ_OPEN )
        {
            System.out.println("performRequest called with REQ_OPEN"); 
        }
    }*/

	protected void createEditPolicies() {
		installEditPolicy(EditPolicy.COMPONENT_ROLE,new GmeElementEditPolicy());
		installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE,new GmeNodeEditPolicy());
	}

	abstract protected AccessibleEditPart createAccessible();

	/**
	 * Makes the EditPart insensible to changes in the model
	 * by removing itself from the model's list of listeners.
	 */
	public void deactivate() {
		if (!isActive())
			return;
		super.deactivate();
		getGmeSubpart().removePropertyChangeListener(this);
	}

	protected AccessibleEditPart getAccessibleEditPart() {
		if (acc == null)
			acc = createAccessible();
		return acc;
	}

	/**
	 * Returns the model associated with this as a GmeSubpart.
	 *
	 * @return  The model of this as a GmeSubpart.
	 */
	protected GmeSubpart getGmeSubpart() {
		return (GmeSubpart) getModel();
	}

	/**
	 * Returns a list of connections for which this is the 
	 * source.
	 *
	 * @return List of connections.
	 */
	protected List getModelConnections() {
		return getGmeSubpart().getConnections();
	}

	/**
	 * Returns a list of connections for which this is the 
	 * source.
	 *
	 * @return List of connections.
	 */
	protected List getModelSourceConnections(){
		return (List)getGmeSubpart().getSourceConnections();
	}

	/**
	 * Returns a list of connections for which this is the 
	 * target.
	 *
	 * @return  List of connections.
	 */
	protected List getModelTargetConnections(){
		return (List)getGmeSubpart().getTargetConnections();
	}

	/**
	 * Returns the connection anchor for the given
	 * ConnectionEditPart's source. 
	 *
	 * @return  ConnectionAnchor.
	 */
	public ConnectionAnchor getSourceConnectionAnchor(ConnectionEditPart connEditPart) {
		Edge edge = (Edge) connEditPart.getModel();
		return getNodeFigure().getConnectionAnchor("");//edge.getSource());
	}

	/**
	 * Returns the connection anchor of a source connection which
	 * is at the given point.
	 * 
	 * @return  ConnectionAnchor.
	 */
	public ConnectionAnchor getSourceConnectionAnchor(Request request) {
		Point pt = new Point(((DropRequest) request).getLocation());
		return getNodeFigure().getSourceConnectionAnchorAt(pt);
	}

	/**
	 * Returns the connection anchor for the given 
	 * ConnectionEditPart's target.
	 *
	 * @return  ConnectionAnchor.
	 */
	public ConnectionAnchor getTargetConnectionAnchor(ConnectionEditPart connEditPart) {
		Edge edge = (Edge) connEditPart.getModel();
		return getNodeFigure().getConnectionAnchor("");//edge.getTarget());
	}

	/**
	 * Returns the connection anchor of a terget connection which
	 * is at the given point.
	 *
	 * @return  ConnectionAnchor.
	 */
	public ConnectionAnchor getTargetConnectionAnchor(Request request) {
		Point pt = new Point(((DropRequest) request).getLocation());
		return getNodeFigure().getTargetConnectionAnchorAt(pt);
	}

	/**
	 * Returns the name of the given connection anchor.
	 *
	 * @return  The name of the ConnectionAnchor as a String.
	 */
	final protected String mapConnectionAnchorToTerminal(ConnectionAnchor c) {
		return getNodeFigure().getConnectionAnchorName(c);
	}

	/**
	 * Handles changes in properties of this. It is 
	 * activated through the PropertyChangeListener.
	 * It updates children, source and target connections,
	 * and the visuals of this based on the property
	 * changed.
	 *
	 * @param evt  Event which details the property change.
	 */
	public void propertyChange(PropertyChangeEvent evt) {
		String prop = evt.getPropertyName();
		if (GmeSubpart.CHILDREN.equals(prop))
			refreshChildren();
		else if (GmeSubpart.INPUTS.equals(prop))
			refreshTargetConnections();
		else if (GmeSubpart.OUTPUTS.equals(prop))
			refreshSourceConnections();
		else if (prop.equals(GmeSubpart.ID_SIZE) || prop.equals(GmeSubpart.ID_LOCATION)){
			refreshVisuals();
		}
	}

	/**
	 * Updates the visual aspect of this. 
	 */
	protected void refreshVisuals() {
		Point loc = getGmeSubpart().getLocation();
		Dimension size = getGmeSubpart().getSize();
		Rectangle r = new Rectangle(loc, size);

		((GraphicalEditPart) getParent()).setLayoutConstraint(this,getFigure(),r);
	}

	/** 
	 * Returns the Figure of this, as a node type figure.
	 *
	 * @return  Figure as a NodeFigure.
	 */
	protected NodeFigure getNodeFigure() {
		return (NodeFigure) getFigure();
	}
	
	public abstract void reRoute();
}
