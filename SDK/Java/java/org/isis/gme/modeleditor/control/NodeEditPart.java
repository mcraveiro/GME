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
 * Created on Sep 24, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor.control;

import java.beans.PropertyChangeEvent;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Random;
import java.util.Vector;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.AccessibleAnchorProvider;
import org.eclipse.gef.AccessibleEditPart;
import org.eclipse.gef.DragTracker;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartListener;
import org.eclipse.gef.Request;
import org.eclipse.swt.accessibility.AccessibleEvent;
import org.eclipse.ui.IWorkbenchPage;
import org.isis.gme.attributeview.AttributeView;
import org.isis.gme.mga.MgaFCO;
import org.isis.gme.mga.MgaModel;
import org.isis.gme.mga.MgaObject;
import org.isis.gme.mgadecorator.MgaDecorator;
import org.isis.gme.mgadecorator.MgaDecorator.Location;
import org.isis.gme.modeleditor.ModelEditorInput;
import org.isis.gme.modeleditor.autorouter.Box;
import org.isis.gme.modeleditor.autorouter.Graph;
import org.isis.gme.modeleditor.autorouter.Path;
import org.isis.gme.modeleditor.model.Edge;
import org.isis.gme.modeleditor.model.GmeDiagram;
import org.isis.gme.modeleditor.model.GmeSubpart;
import org.isis.gme.modeleditor.model.Node;
import org.isis.gme.modeleditor.model.Port;
import org.isis.gme.modeleditor.tools.GmeDragEditPartsTracker;
import org.isis.gme.modeleditor.view.FigureFactory;
import org.isis.gme.plugin.GmePlugin;

/**
 * NodeEditPart.java
 * 
 * @author Papszi
 */
public class NodeEditPart extends GmeEditPart{

    public NodeEditPart(){
        // handles selection change to update attribute panel
        addEditPartListener( new EditPartListener.Stub() {
            public void selectedStateChanged(EditPart part){
                if( getSelected() == EditPart.SELECTED_PRIMARY ){
                    MgaFCO fco = getNodeModel().getFCO();
                    AttributeView attribView = GmePlugin.getPlugin().getAttributeView();
                    if( attribView != null )                 
                        attribView.setFCO(fco);
                }
            }            
        } );                
    }
   
    // handles double click to open a model
    public void performRequest(Request request){
        if( request.getType() == REQ_OPEN ){
            try{                        
                MgaFCO fco = getNodeModel().getFCO();
                if( fco != null ){
                    if( fco.getObjType() == MgaObject.OBJTYPE_MODEL )
                    {
                        IWorkbenchPage   page = GmePlugin.getPlugin().getWorkbench().getActiveWorkbenchWindow().getActivePage();
                        page.openEditor( new ModelEditorInput(new MgaModel( fco )), "org.isis.gme.modeleditor.ModelEditor" );                           
                    }
                }
            }
            catch( Exception e ){
                e.printStackTrace();
            }
        }
    }

	/* (non-Javadoc)
	 * @see org.isis.gme.modeleditor.control.GmeEditPart#createAccessible()
	 */
	protected AccessibleEditPart createAccessible() {
		return new AccessibleGraphicalEditPart(){
			public void getName(AccessibleEvent e) {
				e.result = "Node";
			}
		};
	}

	/* (non-Javadoc)
	 * @see org.eclipse.gef.editparts.AbstractGraphicalEditPart#createFigure()
	 */
	protected IFigure createFigure() {
		return FigureFactory.createNewNode(getNodeModel().getDecorator());
	}

	protected Node getNodeModel() {
		return (Node)getModel();
	}
	
	public Object getAdapter(Class key) {
		if (key == AccessibleAnchorProvider.class)
			return new DefaultAccessibleAnchorProvider() { 
				public List getSourceAnchorLocations() {
					List list = new ArrayList();
					Vector sourceAnchors = getNodeFigure().getSourceConnectionAnchors();
					for (int i=0; i<sourceAnchors.size(); i++) {
						ConnectionAnchor anchor = (ConnectionAnchor)sourceAnchors.get(i);
						list.add(anchor.getReferencePoint().getTranslated(0, -3));
					}
					return list;
				}

			};
		return super.getAdapter(key);
	}
    
	/**
	 * Updates the visual aspect of this. 
	 */
	protected void refreshVisuals() {
		super.refreshVisuals();

		// refresh the model!
		Point loc = getGmeSubpart().getLocation();
		Dimension size = getGmeSubpart().getSize();

		MgaDecorator decorator = getNodeModel().getDecorator();
		decorator.setLocation(decorator.new Location(loc.x, loc.y, loc.x+size.width, loc.y+size.height));
	}
           
	public void propertyChange(PropertyChangeEvent evt) {
		String prop = evt.getPropertyName();
		if (prop.equals(GmeSubpart.ID_SIZE) || prop.equals(GmeSubpart.ID_LOCATION)){
            Node node = (Node)getGmeSubpart();
            node.autoPlace();
			modifyRoutingInfo();
		}
		super.propertyChange(evt);
	}

	private void modifyRoutingInfo(){
		int x1,y1,x2,y2;
		Node node = getNodeModel();

		Iterator edges = node.getConnections().iterator();
		GmeDiagram diagram = (GmeDiagram)getNodeModel().getParent();
		Graph graph = diagram.getGraph();

		graph.removeBox(node.getBox());
		Box box = new Box(node.getLocation().x, node.getLocation().y, node.getLocation().x+node.getSize().width, node.getLocation().y+node.getSize().height);
		node.setBox(box);
		org.isis.gme.modeleditor.autorouter.Port aPort1 = new org.isis.gme.modeleditor.autorouter.Port(node.getLocation().x, node.getLocation().y, node.getLocation().x+node.getSize().width, node.getLocation().y+node.getSize().height);
		node.setPort(aPort1); 
		box.addPort(aPort1);
		Iterator ports = node.getPorts().iterator();
		while (ports.hasNext()){
			Port mPort = (Port)ports.next();
			x1 = node.getLocation().x+mPort.getLocation().x;
			y1 = node.getLocation().y+mPort.getLocation().y;
			x2 = node.getLocation().x+mPort.getLocation().x+mPort.getSize().width;
			y2 = node.getLocation().y+mPort.getLocation().y+mPort.getSize().height;
			org.isis.gme.modeleditor.autorouter.Port aPort = new org.isis.gme.modeleditor.autorouter.Port(x1,y1,x2,y2);
			aPort.setAttributes(mPort.getRoutingPref());
			mPort.setPort(aPort);
			box.addPort(aPort);
		}
		graph.addBox(box);
		while (edges.hasNext()){
			Edge edge = (Edge)edges.next();
			org.isis.gme.modeleditor.autorouter.Port sourcePort, targetPort;
			sourcePort = edge.getSource().getPort();
			targetPort = edge.getTarget().getPort();
			Path path = graph.addPath( sourcePort, targetPort );
			edge.setPath(path);
		}
        node.setRoutingPrefs();
	}

	public DragTracker getDragTracker(Request request) {
		return new GmeDragEditPartsTracker(this);
	}

	/* (non-Javadoc)
	 * @see org.isis.gme.modeleditor.control.GmeEditPart#redrawAll()
	 */
	public void reRoute() {
		Iterator connections = getNodeModel().getSourceConnections().iterator();

		while (connections.hasNext()){
			((Edge)connections.next()).refreshBendPoints();
		}		
	}
}
