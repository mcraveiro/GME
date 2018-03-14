/*
 * Created on Feb 16, 2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package sfInterpreter;

import org.isis.gme.bon.*;
import org.isis.gme.mga.MgaModel;

/**
 * @author BrianW
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class SFPrimitive extends SFProcessing 
{
	public SFPrimitive(MgaModel iModel, JBuilderModel parent)
		{
			super(iModel,parent);
		}

	public void traverseChildren()
	{
		SFBONComponent.startElem();
		SFBONComponent.println("Primitive Processing");
		SFBONComponent.println("Name: "+this.getName());
	
		String[] ret = new String[1];
		ret[0] = "";
		int[] retInt = new int[1];
		retInt[0] = -1;
		if(this.getAttribute("Firing",ret))
			SFBONComponent.println("Firing: "+ret[0]);
		if(this.getAttribute("Script",ret))
			SFBONComponent.println("Script: "+ret[0]);
		if(this.getAttribute("Priority",retInt))
			SFBONComponent.println("Priority: "+retInt[0]);
		
		super.traverseChildren();
		SFBONComponent.endElem();
	}

}
