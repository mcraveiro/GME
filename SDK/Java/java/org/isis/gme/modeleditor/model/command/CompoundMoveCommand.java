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
 * Created on Oct 8, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
 
package org.isis.gme.modeleditor.model.command;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.isis.gme.modeleditor.model.GmeDiagram;

/**
 * CompoundMoveCommand.java
 * 
 * @author Papszi
 */
public class CompoundMoveCommand extends Command{

	private List commandList = new ArrayList();

	private GmeDiagram diagram;

	public CompoundMoveCommand() { }

	public CompoundMoveCommand(String label) {
		super(label);
	}

	public void setGmeDiagram(GmeDiagram p){
		diagram = p;
	}

	/**
	 * Adds the specified command if it is not <code>null</code>.
	 * @param command <code>null</code> or a Command
	 */
	public void add(Command command) {
		if (command != null)
			commandList.add(command);
	}

	/**
	 * @see org.eclipse.gef.commands.Command#canExecute()
	 */
	public boolean canExecute() {
		if (commandList.size() == 0)
			return false;
		for (int i = 0; i < commandList.size(); i++) {
			Command cmd = (Command) commandList.get(i);
			if (cmd == null)
				return false;
			if (!cmd.canExecute())
				return false;
		}
		return true;
	}

	/**
	 * @see org.eclipse.gef.commands.Command#canUndo()
	 */
	public boolean canUndo() {
		if (commandList.size() == 0)
			return false;
		for (int i = 0; i < commandList.size(); i++) {
			Command cmd = (Command) commandList.get(i);
			if (cmd == null)
				return false;
			if (!cmd.canUndo())
				return false;
		}
		return true;
	}

	/**
	 * Disposes all contained Commands.
	 * @see org.eclipse.gef.commands.Command#dispose()
	 */
	public void dispose() {
		for (int i = 0; i < commandList.size(); i++)
			((Command)getCommands().get(i))
				.dispose();
	}

	/**
	 * Execute the command.For a compound command this
	 * means executing all of the commands that it contains.
	 */
	public void execute() {
		for (int i = 0; i < commandList.size(); i++) {
			Command cmd = (Command) commandList.get(i);
			cmd.execute();
		}
		diagram.autoRoute();
		diagram.layoutChanged();
	}

	/**
	 * This is useful when implementing {@link
	 * org.eclipse.jface.viewers.ITreeContentProvider#getChildren(Object)} to display the
	 * Command's nested structure.
	 * @return returns the Commands as an array of Objects.
	 */
	public Object [] getChildren() {
		return commandList.toArray();
	}

	/**
	 * @return the List of contained Commands
	 */
	public List getCommands() {
		return commandList;
	}

	/**
	 * @see org.eclipse.gef.commands.Command#getLabel()
	 */
	public String getLabel() {
		String label = super.getLabel();
		if (label == null)
			if (commandList.isEmpty())
				return null;
		if (label != null)
			return label;
		return ((Command)commandList.get(0)).getLabel();
	}

	/**
	 * @return <code>true</code> if the CompoundCommand is empty
	 */
	public boolean isEmpty() {
		return commandList.isEmpty();
	}

	/**
	 * @see org.eclipse.gef.commands.Command#redo()
	 */
	public void redo() {
		for (int i = 0; i < commandList.size(); i++)
			((Command) commandList.get(i)).redo();
		diagram.autoRoute();
	}

	/**
	 * @return the number of contained Commands
	 */
	public int size() {
		return commandList.size();
	}

	/**
	 * @see org.eclipse.gef.commands.Command#undo()
	 */
	public void undo() {
		for (int i = commandList.size() - 1; i >= 0; i--)
			((Command) commandList.get(i))
				.undo();
		diagram.autoRoute();
	}

	/**
	 * Returns the simplest form of this Command that is equivalent.  This is useful for
	 * removing unnecessary nesting of Commands.
	 * @return the simplest form of this Command that is equivalent
	 */
	public Command unwrap() {
		switch (commandList.size()) {
			case 0 :
				return UnexecutableCommand.INSTANCE;
			case 1 :
				return (Command) commandList.get(0);
			default :
				return this;
		}
	}


}
