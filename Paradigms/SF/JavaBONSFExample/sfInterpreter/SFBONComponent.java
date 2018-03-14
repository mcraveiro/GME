/*
 * Created on Feb 13, 2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package sfInterpreter;

import java.util.*;
import java.io.*;

//import org.isis.gme.bon.BONComponent;
//import org.isis.gme.bon.JBuilder;
//import org.isis.gme.bon.JBuilderObject;
import org.isis.gme.bon.*;

import javax.swing.JOptionPane;

/**
 * @author BrianW
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class SFBONComponent implements BONComponent {

	/* (non-Javadoc)
	 * @see org.isis.gme.bon.BONComponent#invokeEx(org.isis.gme.bon.JBuilder, org.isis.gme.bon.JBuilderObject, java.util.Collection, int)
	 */
	 
	private static PrintStream out;
	private static int indent = 0;
	 
	public void invokeEx(
		JBuilder builder,
		JBuilderObject focus,
		Collection selected,
		int param) 
	{
		if(focus == null)
		{
			JOptionPane.showMessageDialog(null,"Please run with a model in Focus");
			return;
		}
		String outputFileName = JOptionPane.showInputDialog("Output file name:");
		
		if(outputFileName == null)
			JOptionPane.showMessageDialog(null,"Cancelled");
		else
		{
			try
			{
				out = new PrintStream(new FileOutputStream(outputFileName));
				focus.traverseChildren();
				out.close();
			}
			catch(FileNotFoundException e)
			{
			}
		}
	}

	/* (non-Javadoc)
	 * @see org.isis.gme.bon.BONComponent#registerCustomClasses()
	 */
	public void registerCustomClasses() 
	{
		JBuilderFactory.addCustomModel("Primitive","sfInterpreter.SFPrimitive");
		JBuilderFactory.addCustomModel("Compound","sfInterpreter.SFCompound");
		JBuilderFactory.addCustomAtom("InputSignal","sfInterpreter.SFInputSignal");
		JBuilderFactory.addCustomAtom("OutputSignal","sfInterpreter.SFOutputSignal");
		JBuilderFactory.addCustomAtom("Param","sfInterpreter.SFParam");
		JBuilderFactory.addCustomAtom("InputParam","sfInterpreter.SFInputParam");
		JBuilderFactory.addCustomAtom("OutputParam","sfInterpreter.SFOutputParam");
		JBuilderFactory.addCustomConnection("DataflowConn","sfInterpreter.SFDataflowConn");
		JBuilderFactory.addCustomConnection("ParameterConn","sfInterpreter.SFParameterConn");

	}
	
	public static void startElem()
	{
		++indent;
		out.println();
	}
	
	public static void endElem()
	{
		if(indent>0) --indent;
		out.println();
	}
	
	public static void println(String str)
	{
		String output = str;
		for(int i=0;i<indent;i++)
		{
			output = "     "+output;
		}
		out.println(output);	
	}
	

}
