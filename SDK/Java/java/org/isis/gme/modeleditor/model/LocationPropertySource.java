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

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

import org.eclipse.draw2d.geometry.Point;

/**
 * LocationPropertySource.java
 * 
 * @author Papszi
 */
public class LocationPropertySource implements IPropertySource{

	public static String ID_XPOS = "xPos"; //$NON-NLS-1$
	public static String ID_YPOS = "yPos"; //$NON-NLS-1$
	protected static IPropertyDescriptor[] descriptors;

	static{
		descriptors = new IPropertyDescriptor[] {
			new TextPropertyDescriptor(ID_XPOS,"XPosition"),
			new TextPropertyDescriptor(ID_YPOS,"YPosition")
		};
	}

	protected Point point = null;

	public LocationPropertySource(Point point){
		this.point = new Point(point);
	}

	public Object getEditableValue(){
		return this;
	}

	public IPropertyDescriptor[] getPropertyDescriptors(){
		return descriptors;
	}

	public Object getPropertyValue(Object propName){
		if(ID_XPOS.equals(propName)){
			return new String(new Integer(point.x).toString());
		}
		if(ID_YPOS.equals(propName)){
			return new String(new Integer(point.y).toString());
		}
		return null;
	}

	public Point getValue(){
		return new Point(point);
	}

	public boolean isPropertySet(Object propName){
		if(ID_XPOS.equals(propName) || ID_YPOS.equals(propName))return true;
		return false;
	}

	public void resetPropertyValue(Object propName){}

	public void setPropertyValue(Object propName, Object value){
		if(ID_XPOS.equals(propName)){
			Integer newInt = new Integer((String)value);
			point.x = newInt.intValue();
		}
		if(ID_YPOS.equals(propName)){
			Integer newInt = new Integer((String)value);
			point.y = newInt.intValue();
		}
	}

	public String toString(){
		return new String("["+point.x+","+point.y+"]");//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$
	}

}
