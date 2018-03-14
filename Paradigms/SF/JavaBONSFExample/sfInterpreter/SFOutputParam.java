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
public class SFOutputParam extends SFParameterBase {

	/**
	 * @param iAtom
	 * @param parent
	 */
	public SFOutputParam(MgaAtom iAtom, JBuilderModel parent) {
		super(iAtom, parent);
		// TODO Auto-generated constructor stub
	}

	public void traverseChildren()
	{
		SFBONComponent.startElem();
		SFBONComponent.println("Output Parameter");					
		super.traverseChildren();
		SFBONComponent.endElem();
	}

}
