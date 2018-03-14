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

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

import org.eclipse.draw2d.geometry.Dimension;
/**
 * DimensionPropertySource.java
 * 
 * @author Papszi
 */
public class DimensionPropertySource implements IPropertySource{

	public static String ID_WIDTH = "width";  //$NON-NLS-1$
	public static String ID_HEIGHT = "height";//$NON-NLS-1$
	protected static IPropertyDescriptor[] descriptors;

	static{
		descriptors = new IPropertyDescriptor[] {
			new TextPropertyDescriptor(ID_WIDTH,"Width"),
			new TextPropertyDescriptor(ID_HEIGHT,"Height")
		};
	}

	protected Dimension dimension = null;

	public DimensionPropertySource(Dimension dimension){
		this.dimension = new Dimension(dimension);
	}

	public Object getEditableValue(){
		return this;
	}

	public Object getPropertyValue(Object propName){
		return getPropertyValue((String)propName);
	}

	public Object getPropertyValue(String propName){
		if(ID_HEIGHT.equals(propName)){
			return new String(new Integer(dimension.height).toString());
		}
		if(ID_WIDTH.equals(propName)){
			return new String(new Integer(dimension.width).toString());
		}
		return null;
	}

	public Dimension getValue(){
		return new Dimension(dimension);
	}

	public void setPropertyValue(Object propName, Object value){
		setPropertyValue((String)propName, value);
	}

	public void setPropertyValue(String propName, Object value){
		if(ID_HEIGHT.equals(propName)){
			Integer newInt = new Integer((String)value);
			dimension.height = newInt.intValue();
		}
		if(ID_WIDTH.equals(propName)){
			Integer newInt = new Integer((String)value);
			dimension.width = newInt.intValue();
		}
	}

	public IPropertyDescriptor[] getPropertyDescriptors(){
		return descriptors;
	}

	public void resetPropertyValue(String propName){
	}

	public void resetPropertyValue(Object propName){
	}

	public boolean isPropertySet(Object propName){
		return true;
	}

	public boolean isPropertySet(String propName){
		if(ID_HEIGHT.equals(propName) || ID_WIDTH.equals(propName))return true;
		return false;
	}

	public String toString(){
		return new String("("+dimension.width+","+dimension.height+")");//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$
	}

}
