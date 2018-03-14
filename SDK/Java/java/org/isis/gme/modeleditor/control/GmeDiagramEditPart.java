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
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.ConnectionLayer;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FreeformLayer;
import org.eclipse.draw2d.FreeformLayout;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.gef.AccessibleEditPart;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.DragTracker;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.Request;
import org.eclipse.gef.editpolicies.RootComponentEditPolicy;
import org.eclipse.gef.requests.SelectionRequest;
import org.eclipse.gef.tools.DeselectAllTracker;
import org.eclipse.gef.tools.MarqueeDragTracker;
import org.eclipse.swt.accessibility.AccessibleEvent;
import org.isis.gme.modeleditor.autorouter.GmeAutoRouter;
import org.isis.gme.modeleditor.model.GmeDiagram;

/**
 * GmeDiagramEditPart.java
 * 
 * @author Papszi
 */
public class GmeDiagramEditPart extends GmeContainerEditPart implements LayerConstants{

	private GmeAutoRouter router = new GmeAutoRouter();

	public GmeAutoRouter getRouter(){
		return router;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.gef.editparts.AbstractGraphicalEditPart#createFigure()
	 */
	protected IFigure createFigure() {
		//System.out.println("GmeDiagramEditPart.createFigure");
		Figure f = new FreeformLayer();
		f.setLayoutManager(new FreeformLayout());
		f.setBorder(new MarginBorder(5));
		f.setBackgroundColor(ColorConstants.listBackground);
		f.setOpaque(true);
		return f;
	}

	final private List fEmptyHandles = new ArrayList();

	protected AccessibleEditPart createAccessible() {
		return new AccessibleGraphicalEditPart(){
			public void getName(AccessibleEvent e) {
				e.result = "LogicMessages.LogicDiagram_LabelText";
			}
		};
	}

	/**
	 * Installs EditPolicies specific to this. 
	 */
	protected void createEditPolicies(){
		super.createEditPolicies();
		//System.out.println("GmeDiagramEditPart.createEditPolicies");
		installEditPolicy(EditPolicy.NODE_ROLE, null);
		installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE, null);
		installEditPolicy(EditPolicy.SELECTION_FEEDBACK_ROLE, null);
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new RootComponentEditPolicy());
	}

	protected void addChildVisual(EditPart childEditPart, int index) {
		//System.out.println("The index is: "+index);
		//System.out.println("The class is: "+childEditPart.getClass().getName());
		super.addChildVisual(childEditPart, index);
	}

	public DragTracker getDragTracker(Request req){
		if (req instanceof SelectionRequest 
			&& ((SelectionRequest)req).getLastButtonPressed() == 3)
				return new DeselectAllTracker(this);
		return new MarqueeDragTracker();
	}

	public ConnectionAnchor getSourceConnectionAnchor(ConnectionEditPart editPart) {
		return null;
	}

	public ConnectionAnchor getSourceConnectionAnchor(int x, int y) {
		return null;
	}

	public ConnectionAnchor getTargetConnectionAnchor(ConnectionEditPart editPart) {
		return null;
	}

	public ConnectionAnchor getTargetConnectionAnchor(int x, int y) {
		return null;
	}

	public void propertyChange(PropertyChangeEvent evt){
		if (GmeDiagram.LAYOUT.equals(evt.getPropertyName())){
			refreshVisuals();
			reRoute();
		}else
			super.propertyChange(evt);
	}

	protected void refreshVisuals(){
		((ConnectionLayer)getLayer(CONNECTION_LAYER)).setConnectionRouter(router);
	}
    
	public void reRoute(){
		Iterator children = getChildren().iterator();
		
		while (children.hasNext()){
			((GmeEditPart)children.next()).reRoute();
		}
		
	}


    /**
     * Returns the model of this as a LogicDiagram.
     *
     * @return  LogicDiagram of this.
     */
    protected GmeDiagram getGmeDiagram() {
        return (GmeDiagram)getModel();
    }

    /**
     * Returns the children of this through the model.
     *
     * @return  Children of this as a List.
     */
    protected List getModelChildren() {
        //System.out.println("GmeContainerEditPart.getModelChildren called");
        return getGmeDiagram().getChildren();
    }
}
