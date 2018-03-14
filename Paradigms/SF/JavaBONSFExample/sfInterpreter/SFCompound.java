/*
 * Created on Feb 17, 2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package sfInterpreter;

import org.isis.gme.mga.MgaModel;
import org.isis.gme.bon.*;

/**
 * @author BrianW
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class SFCompound extends SFProcessing 
{
	public SFCompound(MgaModel iModel, JBuilderModel parent)
	{
		super(iModel,parent);
	}

	public void traverseChildren()
	{
		SFBONComponent.startElem();
		SFBONComponent.println("Compound Processing");
		SFBONComponent.println("Name: "+this.getName());					
		super.traverseChildren();
		SFBONComponent.endElem();
	}

}
