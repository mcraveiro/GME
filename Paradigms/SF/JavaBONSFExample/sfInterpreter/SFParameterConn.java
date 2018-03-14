/*
 * Created on Feb 18, 2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package sfInterpreter;

import org.isis.gme.bon.*;
import org.isis.gme.mga.MgaSimpleConnection;

/**
 * @author BrianW
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class SFParameterConn extends JBuilderConnection {

	/**
	 * @param iConn
	 * @param parent
	 */
	public SFParameterConn(MgaSimpleConnection iConn, JBuilderModel parent) {
		super(iConn, parent);
		// TODO Auto-generated constructor stub
	}

	public void traverseChildren()
	{
		SFBONComponent.startElem();
		SFBONComponent.println("Parameter Connection");
		SFBONComponent.println("Name: "+this.getName());
		SFBONComponent.println("SRC: "+this.getSource().getName());
		SFBONComponent.println("DST: "+this.getDestination().getName());					
		super.traverseChildren();
		SFBONComponent.endElem();
	}

}
