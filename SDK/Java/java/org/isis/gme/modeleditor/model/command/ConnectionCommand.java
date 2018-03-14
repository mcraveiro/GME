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
 
package org.isis.gme.modeleditor.model.command;

import org.eclipse.gef.commands.Command;
import org.isis.gme.modeleditor.model.Edge;
import org.isis.gme.modeleditor.model.GmeSubpart;

/**
 * ConnectionCommand.java
 * 
 * @author Papszi
 */
public class ConnectionCommand extends Command{
	protected GmeSubpart oldSource;
	protected String oldSourceTerminal;
	protected GmeSubpart oldTarget;
	protected String oldTargetTerminal;
	protected GmeSubpart source;
	protected String sourceTerminal;
	protected GmeSubpart target; 
	protected String targetTerminal; 
	protected Edge edge;

	public ConnectionCommand() {
		super("ConnectionLabel");
	}

	public boolean canExecute(){
		return true;
	}

	public void execute() {
		if (source != null){
			edge.detachSource();
			edge.setSource(source);
			//edge.setSourceTerminal(sourceTerminal);
			edge.attachSource();
		}
		if (target != null) {
			edge.detachTarget();
			edge.setTarget(target);
			//edge.setTargetTerminal(targetTerminal);
			edge.attachTarget();
		}
		if (source == null && target == null){
			edge.detachSource();
			edge.detachTarget();
			edge.setTarget(null);
			edge.setSource(null);
		}
	}

	public String getLabel() {
		return "Connection command description";
	}

	public GmeSubpart getSource() {
		return source;
	}

	public java.lang.String getSourceTerminal() {
		return sourceTerminal;
	}

	public GmeSubpart getTarget() {
		return target;
	}

	public String getTargetTerminal() {
		return targetTerminal;
	}

	public Edge getEdge() {
		return edge;
	}

	public void redo() { 
		execute(); 
	}

	public void setSource(GmeSubpart newSource) {
		source = newSource;
	}

	public void setSourceTerminal(String newSourceTerminal) {
		sourceTerminal = newSourceTerminal;
	}

	public void setTarget(GmeSubpart newTarget) {
		target = newTarget;
	}

	public void setTargetTerminal(String newTargetTerminal) {
		targetTerminal = newTargetTerminal;
	}

	public void setEdge(Edge w) {
		edge = w;
		oldSource = w.getSource();
		oldTarget = w.getTarget();
		/*oldSourceTerminal = w.getSourceTerminal();
		oldTargetTerminal = w.getTargetTerminal();
		*/	
	}

	public void undo() {
		source = edge.getSource();
		target = edge.getTarget();
		/*sourceTerminal = edge.getSourceTerminal();
		targetTerminal = edge.getTargetTerminal();
		*/

		edge.detachSource();
		edge.detachTarget();

		edge.setSource(oldSource);
		edge.setTarget(oldTarget);
		/*edge.setSourceTerminal(oldSourceTerminal);
		edge.setTargetTerminal(oldTargetTerminal);
		*/
		
		edge.attachSource();
		edge.attachTarget();
	}
}
