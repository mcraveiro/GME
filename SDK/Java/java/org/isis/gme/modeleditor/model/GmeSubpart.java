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
 
package org.isis.gme.modeleditor.model;


import java.util.ArrayList;
import java.util.Iterator;
import java.util.Vector;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
/**
 * GmeSubpart.java
 * 
 * @author Papszi
 */
abstract public class GmeSubpart extends GmeElement{

	private String id;
	protected ArrayList inputs = new ArrayList(4);
	protected Point location = new Point(0,0);
	protected ArrayList outputs  = new ArrayList(4);
	protected GmeSubpart parent = null;
	static final long serialVersionUID = 1;
	protected Dimension size = new Dimension(-1,-1);

	protected static IPropertyDescriptor[] descriptors = null;
	public static String ID_SIZE = "size";         //$NON-NLS-1$
	public static String ID_LOCATION = "location"; //$NON-NLS-1$

	protected org.isis.gme.modeleditor.autorouter.Port port;

	static{
		descriptors = new IPropertyDescriptor[]{
			new PropertyDescriptor(ID_SIZE, "Size"),
			new PropertyDescriptor(ID_LOCATION,"Location")
		};
	}

	public GmeSubpart() {
		setID(getNewID());
	}

	public GmeSubpart(GmeSubpart parent) {
		this.parent = parent;
	}

	public GmeSubpart getParent(){
		return parent;
	}

	public void setParent(GmeSubpart parent){
		this.parent = parent;
	}

	public void setPort(org.isis.gme.modeleditor.autorouter.Port aPort){
		port = aPort;
	}

	public org.isis.gme.modeleditor.autorouter.Port getPort(){
		return port;
	}

	public ArrayList getConnections() {
		ArrayList a = (ArrayList)outputs.clone();
		Iterator ins = inputs.iterator();
		while (ins.hasNext())
			a.add(ins.next());
		return a;
	}

	public Image getIcon() {
		return getIconImage();
	}

	abstract public Image getIconImage();

	public String getID() {
		return id; 
	}
/*
	protected boolean getEdge(String terminal) {
		Edge w = (Edge)connections.get(terminal);
		return (w == null) ? false : w.getValue();
	}
*/
	public Point getLocation() {
		return location;
	}
	
	public void setLocation(Point p) {
		if (location.equals(p)) return;
		location = p;
		firePropertyChange("location", null, p);
	}

	abstract protected String getNewID();

	/**
	 * Returns useful property descriptors for the use
	 * in property sheets. this supports location and
	 * size.
	 *
	 * @return  Array of property descriptors.
	 */
	public IPropertyDescriptor[] getPropertyDescriptors() {
		return descriptors;
	}


	/**
	 * Returns an Object which represents the appropriate
	 * value for the property name supplied.
	 *
	 * @param propName  Name of the property for which the
	 *                  the values are needed.
	 * @return  Object which is the value of the property.
	 */
	public Object getPropertyValue(Object propName) {
		if (ID_SIZE.equals(propName))
			return new DimensionPropertySource(getSize());
		else if( ID_LOCATION.equals(propName))
			return new LocationPropertySource(getLocation());
		return null;
	}

	public Dimension getSize() {
		return size;
	}

	/**
	 * 
	 */
	public boolean isPropertySet(){
		return true;	
	}

	public void setID(String s) {
		id = s;
	}

	/**
	 * Sets the value of a given property with the value
	 * supplied. Also fires a property change if necessary.
	 * 
	 * @param id  Name of the parameter to be changed.
	 * @param value  Value to be set to the given parameter.
	 */
	public void setPropertyValue(Object id, Object value){
		if (ID_SIZE.equals(id)){
			DimensionPropertySource dimPS = (DimensionPropertySource)value;
			setSize(new Dimension(dimPS.getValue()));
		}
		else if (ID_LOCATION.equals(id)){
			LocationPropertySource locPS = (LocationPropertySource)value;
			setLocation(new Point(locPS.getValue()));
		}
	}

	public void setSize(Dimension d) {
		if (size.equals(d)) return;
		size = d;
		firePropertyChange("size", null, size);
	}

	public void connectInput(Edge w) {
		inputs.add(w);
		update();
		fireStructureChange(INPUTS, w);
	}

	public void connectOutput(Edge w) {
		outputs.add(w);
		update();
		fireStructureChange(OUTPUTS, w);
	}

	public void disconnectInput(Edge w) {
		inputs.remove(w.getTarget());
		update();
		fireStructureChange(INPUTS,w);
	}

	public void disconnectOutput(Edge w) {
		outputs.remove(w);
		update();
		fireStructureChange(OUTPUTS,w);
	}

	/*protected boolean getInput(String terminal) {
		Edge w = (Edge)inputs.get(terminal);
		return (w == null) ? false : w.getValue();
	}*/

	public ArrayList getSourceConnections() {
		return (ArrayList)outputs.clone();
	}

	public Vector getTargetConnections() {
		Iterator iter = inputs.iterator();
		Vector v = new Vector(inputs.size());
		while (iter.hasNext())
			v.addElement(iter.next());
		return v;
	}

	protected void setOutput(String terminal, boolean val) {
		Iterator iter = outputs.iterator();
		Edge e;
		while (iter.hasNext()) {
			e = (Edge) iter.next();
			if (e.getSource().equals(terminal) && this.equals(e.getSource()))
				e.setValue(val);
		}
	}
}
