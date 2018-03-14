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
 
package org.isis.gme.modeleditor.model;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.swt.graphics.Image;
import org.isis.gme.modeleditor.ModelAspectEditorInput;
import org.isis.gme.modeleditor.autorouter.Box;
import org.isis.gme.modeleditor.autorouter.Graph;
import org.isis.gme.modeleditor.autorouter.Path;

/**
 * GmeDiagram.java
 * 
 * @author Papszi
 */
public class GmeDiagram extends GmeSubpart {

	public static String LAYOUT = "layout";
	
	private static int count;
	private List children = new ArrayList();
	private Graph graph = new Graph();

	public GmeDiagram(ModelAspectEditorInput input) {
        
        // fill children
        Iterator nodes = input.getNodes().iterator();
        while (nodes.hasNext())
        {
            Node node = (Node)nodes.next();
            node.setParent(this);                
            children.add(node);
        }

        nodes = input.getNodes().iterator();
		int x1,y1,x2,y2;
		while (nodes.hasNext()){
			Node node = (Node)nodes.next();
            node.autoPlace();            
			Box box = new Box(node.location.x, node.location.y, node.location.x+node.size.width, node.location.y+node.size.height);
			box.addPort(node.getPort());
			Iterator ports = node.getPorts().iterator();
			while (ports.hasNext()){
				Port mPort = (Port)ports.next();
				x1 = node.location.x+mPort.location.x;
				y1 = node.location.y+mPort.location.y;
				x2 = node.location.x+mPort.location.x+mPort.size.width;
				y2 = node.location.y+mPort.location.y+mPort.size.height;
				org.isis.gme.modeleditor.autorouter.Port aPort = new org.isis.gme.modeleditor.autorouter.Port(x1,y1,x2,y2);
				aPort.setAttributes(mPort.getRoutingPref());
				mPort.setPort(aPort);
				box.addPort(aPort);
			}
			node.setBox(box);
			graph.addBox(box);
		}                

        Iterator edges = input.getEdges().iterator();        
		while (edges.hasNext()){
			Edge edge = (Edge)edges.next();
			org.isis.gme.modeleditor.autorouter.Port sourcePort, targetPort;
			if (edge.getTarget() instanceof Port){
				((Port)edge.getTarget()).getParent().connectInput(edge);
			}else{
				edge.getTarget().connectInput(edge);
			}
			if (edge.getSource() instanceof Port){
				((Port)edge.getSource()).getParent().connectOutput(edge);
			}else{
				edge.getSource().connectOutput(edge);
			}
			targetPort = edge.getTarget().getPort();
			sourcePort = edge.getSource().getPort();
			Path path = graph.addPath( sourcePort, targetPort );
			edge.setPath(path);
		}
		autoRoute();
	}

	public void layoutChanged(){
		firePropertyChange(GmeDiagram.LAYOUT, null, null);
	}

	public Graph getGraph(){
		return graph;
	}

	/* (non-Javadoc)
	 * @see org.isis.gme.modeleditor.model.GmeSubpart#getIconImage()
	 */
	public Image getIconImage() {
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see org.isis.gme.modeleditor.model.GmeSubpart#getNewID()
	 */
	protected String getNewID() {
		return Integer.toString(count++);
	}

	public void addChild(GmeElement child){
		addChild(child, -1);
	}

	public void addChild(GmeElement child, int index){
		if (index >= 0)
			children.add(index,child);
		else
			children.add(child);
		fireStructureChange(CHILDREN, child);
	}

	public List getChildren(){
		return children;
	}

	public void removeChild(GmeElement child){
		children.remove(child);
		fireStructureChange(CHILDREN, child);
	}
	
	public void autoRoute(){
		graph.autoRoute();
	}

/*	private class ConnectionRouterLabelProvider 
		extends org.eclipse.jface.viewers.LabelProvider{

		public ConnectionRouterLabelProvider(){
			super();
		}
		public String getText(Object element){
			if(element instanceof Integer){
				Integer integer = (Integer)element;
				if(ROUTER_MANUAL.intValue()==integer.intValue())
					return LogicMessages.PropertyDescriptor_LogicDiagram_Manual;
				if(ROUTER_MANHATTAN.intValue()==integer.intValue())
					return LogicMessages.PropertyDescriptor_LogicDiagram_Manhattan;
			}
			return super.getText(element);
		}

	}

	public String toString(){
		return LogicMessages.LogicDiagram_LabelText;
	}
	
	public IPropertyDescriptor[] getPropertyDescriptors() {
		if(getClass().equals(GmeDiagram.class)){
			ComboBoxPropertyDescriptor cbd = new ComboBoxPropertyDescriptor(
					ID_ROUTER, 
					LogicMessages.PropertyDescriptor_LogicDiagram_ConnectionRouter,
					new String[]{
						LogicMessages.PropertyDescriptor_LogicDiagram_Manual,
						LogicMessages.PropertyDescriptor_LogicDiagram_Manhattan});
			cbd.setLabelProvider(new ConnectionRouterLabelProvider());
			return new IPropertyDescriptor[]{cbd};
		}
		return super.getPropertyDescriptors();
	}
	*/
}
