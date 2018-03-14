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
public class SFParameterBase extends JBuilderAtom {

	/**
	 * @param iAtom
	 * @param parent
	 */
	public SFParameterBase(MgaAtom iAtom, JBuilderModel parent) {
		super(iAtom, parent);
		// TODO Auto-generated constructor stub
	}

	public void traverseChildren()
	{
		SFBONComponent.println("Name: "+this.getName());
		
		String[] ret = new String[1];
		ret[0] = "";
		int[] retInt = new int[1];
		retInt[0] = -1;
		if(this.getAttribute("DataType",ret))
			SFBONComponent.println("Data Type: "+ret[0]);
		if(this.getAttribute("Size",retInt))
			SFBONComponent.println("Size: "+retInt[0]);
							
		super.traverseChildren();
	}

}
