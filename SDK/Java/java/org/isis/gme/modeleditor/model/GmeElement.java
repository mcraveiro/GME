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

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;

/**
 * GmeElement.java
 * 
 * @author Papszi
 */
abstract public class GmeElement implements IPropertySource, Cloneable, Serializable{

	public static final String
		CHILDREN = "children",
		INPUTS = "inputs",
		OUTPUTS = "outputs";

	transient protected PropertyChangeSupport listeners = new PropertyChangeSupport(this);

	public void addPropertyChangeListener(PropertyChangeListener l){
		listeners.addPropertyChangeListener(l);
	}

	protected void firePropertyChange(String prop, Object old, Object newValue){
		listeners.firePropertyChange(prop, old, newValue);
	}

	protected void fireStructureChange(String prop, Object child){
		listeners.firePropertyChange(prop, null, child);
	}

	public Object getEditableValue(){
		return this;
	}

	public IPropertyDescriptor[] getPropertyDescriptors(){
		return new IPropertyDescriptor[0];
	}

	public Object getPropertyValue(Object propName){
		return null;
	}

	final Object getPropertyValue(String propName){
		return null;
	}

	public boolean isPropertySet(Object propName){
		return isPropertySet((String)propName);
	}

	final boolean isPropertySet(String propName){
		return true;
	}

	private void readObject(ObjectInputStream in)throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		listeners = new PropertyChangeSupport(this);
	}

	public void removePropertyChangeListener(PropertyChangeListener l){
		listeners.removePropertyChangeListener(l);
	}

	public void resetPropertyValue(Object propName){
	}

	final void resetPropertyValue(String propName){}

	public void setPropertyValue(Object propName, Object val){}
	final void setPropertyValue(String propName, Object val){}

	public void update(){
	}
}
