/*
 * Created on Feb 18, 2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package sfInterpreter;

import org.isis.gme.bon.*;
import org.isis.gme.mga.MgaAtom;

/**
 * @author BrianW
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class SFParam extends SFParameterBase {

	/**
	 * @param iAtom
	 * @param parent
	 */
	public SFParam(MgaAtom iAtom, JBuilderModel parent) {
		super(iAtom, parent);
		// TODO Auto-generated constructor stub
	}

	public void traverseChildren()
	{
		SFBONComponent.startElem();
		SFBONComponent.println("Parameter");
		
		super.traverseChildren();
		
		String[] ret = new String[1];
		ret[0] = "";
		boolean[] retBool = new boolean[1];
		retBool[0] = false;
		if(this.getAttribute("InitValue",ret))
			SFBONComponent.println("Initial Value: "+ret[0]);
		if(this.getAttribute("Global",retBool))
			SFBONComponent.println("Global: "+retBool[0]);
		
		SFBONComponent.endElem();				
	}

}
