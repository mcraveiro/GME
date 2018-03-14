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

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.gef.AccessibleEditPart;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartListener;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractConnectionEditPart;
import org.eclipse.swt.accessibility.AccessibleEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.isis.gme.attributeview.AttributeView;
import org.isis.gme.mga.MgaFCO;
import org.isis.gme.modeleditor.autorouter.Point;
import org.isis.gme.modeleditor.model.Edge;
import org.isis.gme.modeleditor.view.EdgeFigure;
import org.isis.gme.modeleditor.view.FigureFactory;
import org.isis.gme.plugin.GmePlugin;

/**
 * EdgeEditPart.java
 * 
 * @author Papszi
 */
public class EdgeEditPart extends AbstractConnectionEditPart implements PropertyChangeListener{

	AccessibleEditPart acc;
    
    public EdgeEditPart()
    {
        // handles selection change to update attribute panel
        addEditPartListener( new EditPartListener.Stub() {
            public void selectedStateChanged(EditPart part)
            {
                if( getSelected() == EditPart.SELECTED_PRIMARY )
                {
                    MgaFCO fco = new MgaFCO( getEdge().getMgaSimpleConnection() );
                    AttributeView attribView = GmePlugin.getPlugin().getAttributeView();
                    if( attribView != null )                 
                        attribView.setFCO(fco);
                }
            }            
        } );
    }

	/* (non-Javadoc)
	 * @see org.eclipse.gef.editparts.AbstractEditPart#createEditPolicies()
	 */
	protected void createEditPolicies() {
		installEditPolicy(EditPolicy.CONNECTION_ENDPOINTS_ROLE, new EdgeEndpointEditPolicy());
		//Note that the Connection is already added to the diagram and knows its Router.
		installEditPolicy(EditPolicy.CONNECTION_ROLE,new EdgeEditPolicy());
		installEditPolicy(EditPolicy.CONTAINER_ROLE,new GmeContainerEditPolicy());
	}

	public static final Color
		alive = new Color(Display.getDefault(),0,74,168),
		dead  = new Color(Display.getDefault(),0,0,0);

	public void activate(){
		super.activate();
		getEdge().addPropertyChangeListener(this);
	}

	public void activateFigure(){
		super.activateFigure();
		/*Once the figure has been added to the ConnectionLayer, start listening for its
		 * router to change.
		 */
		getFigure().addPropertyChangeListener(Connection.PROPERTY_CONNECTION_ROUTER, this);
	}

	/**
	 * Returns a newly created Figure to represent the connection.
	 *
	 * @return  The created Figure.
	 */
	protected IFigure createFigure() {
		//System.out.println("EdgeEditPart.createFigure");
		if (getEdge() == null)
			return null;
		return FigureFactory.createNewBendableEdge(getEdge());
	}

	public void deactivate(){
		getEdge().removePropertyChangeListener(this);
		super.deactivate();
	}

	public void deactivateFigure(){
		getFigure().removePropertyChangeListener(Connection.PROPERTY_CONNECTION_ROUTER, this);
		super.deactivateFigure();
	}

	public AccessibleEditPart getAccessibleEditPart(){
		if (acc == null)
			acc = new AccessibleGraphicalEditPart(){
				public void getName(AccessibleEvent e) {
					e.result = "Edge label text";
				}
			};
		return acc;
	}

	/**
	 * Returns the model of this represented as a Edge.
	 * 
	 * @return  Model of this as <code>Edge</code>
	 */
	protected Edge getEdge() {
		return (Edge)getModel();
	}

	/**
	 * Returns the Figure associated with this, which draws the
	 * Edge.
	 *
	 * @return  Figure of this.
	 */
	protected EdgeFigure getEdgeFigure() {
		return (EdgeFigure)getFigure();
	}

	/**
	 * Listens to changes in properties of the Edge (like the
	 * contents being carried), and reflects is in the visuals.
	 *
	 * @param event  Event notifying the change.
	 */
	public void propertyChange(PropertyChangeEvent event) {
		String property = event.getPropertyName();
		if ("bendpoint".equals(property))   //$NON-NLS-1$
			refreshVisuals();       
	}

	/**
	 * Updates the bendpoints, based on the model.
	 */
	protected void refreshBendpoints() {
		List modelConstraint = getEdge().getBendpoints();
		List figureConstraint = new ArrayList();
		PointList pointList = new PointList(); 
		for (int i=0; i<modelConstraint.size(); i++) {
			Point ebp = (Point)modelConstraint.get(i);
			pointList.addPoint(ebp.x,ebp.y);
		}
		getConnectionFigure().setPoints(pointList);
		if (getEdgeFigure().getSourceDecoration() != null){
			getEdgeFigure().getSourceDecoration().setLocation(getEdgeFigure().getPoints().getFirstPoint());
			getEdgeFigure().getSourceDecoration().setReferencePoint(getEdgeFigure().getPoints().getPoint(1));
		}
		if (getEdgeFigure().getTargetDecoration() != null){
			getEdgeFigure().getTargetDecoration().setLocation(getEdgeFigure().getPoints().getLastPoint());
			getEdgeFigure().getTargetDecoration().setReferencePoint(getEdgeFigure().getPoints().getPoint(pointList.size()-2));
		}
		getEdgeFigure().adjustLabelPlacement();
	}

	/**
	 * Refreshes the visual aspects of this, based upon the
	 * model (Edge). It changes the Edge color depending on
	 * the state of Edge.
	 * 
	 */
	protected void refreshVisuals() {
		refreshBendpoints();
		if (getEdge().getValue())
			getEdgeFigure().setForegroundColor(alive);
		else
			getEdgeFigure().setForegroundColor(dead);
		//throw new NullPointerException();
	}
}
