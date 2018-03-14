package org.isis.gme.bon;

import java.util.*;
import org.isis.gme.mga.*;
import org.isis.gme.meta.*;

public class JBuilderModelReference extends JBuilderReference 
{
	protected Hashtable objectmap;
	protected Vector refPorts;
	//protected JBuilderObject ref;
	
	public JBuilderModelReference(MgaReference iModelRef, JBuilderModel parent)
	{	super(iModelRef,parent);
		
		objectmap = new Hashtable();
		refPorts = new Vector();
		createReferencePorts();
	}
	
	protected void createReferencePorts()
	{	MgaFCOs children;
		MgaFCO r = getIModelRef();
		while(true)
		{	
            //MgaFCO rr = ((MgaReference)r).getReferred();
            MgaReference ref = new MgaReference(r);
            MgaFCO rr = ref.getReferred();
			r = rr;
			if(r==null)	
				break;
			
			int o = r.getObjType();
			if(o==MgaObject.OBJTYPE_MODEL)
				break;
			
			//Support.Assert(o==objtype_enum.OBJTYPE_REFERENCE," Not a reference");
		}
		if(r!=null)
		{	children = (new MgaModel(r)).getChildFCOs();
			int chCount = children.getCount();
			for(int i=0;i<chCount;i++)
			{	MgaFCO child = children.getItem(i);
				int o = child.getObjType();
				MgaMetaRole role = child.getMetaRole();
				MgaMetaParts parts = role.getParts();
				int paCount = parts.getCount();
				int j;
				for(j=0;j<paCount;j++)
				{	MgaMetaPart part = parts.getItem(j);
					boolean vv = part.getIsLinked();
					if(vv)	
						break;		
				}
				if(j<=paCount)
				{	
					JBuilderReferencePort bPort = new JBuilderReferencePort(child,this);
					refPorts.add(bPort);
				}
			}
		}
	}
		
	public void resolve()
	{	
		super.resolve();
		//Support.Assert(ref!=null,"JBuilderModelReference Resolve");
		
		Class JBuilderModelReferenceClass;
		Class JBuilderModelClass;
		try
		{
			JBuilderModelReferenceClass = Class.forName("org.isis.gme.bon.JBuilderModelReference");
			JBuilderModelClass = Class.forName("org.isis.gme.bon.JBuilderModel");
		}
		catch(ClassNotFoundException ex)
		{	//Support.Assert(false, "In Model and Reference class not found");
			return;
		}
		boolean ex = JBuilderModelReferenceClass.isAssignableFrom(ref.getClass());
		boolean exp = JBuilderModelClass.isAssignableFrom(ref.getClass());
		//Support.Assert(ex || exp, "In Resolve of Model Ref referred object is not of correct type");
		
		int reCount = refPorts.size();
		for(int it=0;it<reCount;it++)
		{	((JBuilderReferencePort)refPorts.elementAt(it)).resolve();
		}
	}
		
	protected MgaObject getPort(String aspectName, JBuilderReferencePort port)
	{	return port.getIObject();
	}
	
	protected JBuilderReferencePort findPortRef(MgaFCO i)
	{	//Support.Assert(i!=null,"JBuilderModelReference FindPortRef");
		MgaFCO ii = i;
		
		JBuilderReferencePort o = (JBuilderReferencePort)objectmap.get(ii.getID());
		if(o==null)
		{	String idstr = i.getID();
			//Support.Assert(false, " JBuilderModelReference FindPortRef");
		}
		return o;
	}
	
	protected void setPortRef(MgaFCO i, JBuilderReferencePort o)
	{	//Support.Assert(i!=null && o!=null, "JBuilderModelReference SetPortRef");
		JBuilderReferencePort o2 = (JBuilderReferencePort)objectmap.get(i.getID());
		if(o2!=null)
		{	//Support.Assert(o==o2,"Difference in the object map and provided");
		}
		else
		{	String strId = i.getID();
			objectmap.put(strId,o);
		}
	}
	
	protected void forgetPortRef(MgaFCO i)
	{	//Support.Assert(i!=null,"JBuilderModelReference ForgetPortRef");
		objectmap.remove(i.getID());
	}
	
	public void initialize()
	{
	}
	
	///////////////////////////////For the users ///////////////////////////////////
	
	public MgaReference getIModelRef()
	{	return getIRef();
	}
	
	public Vector getReferencePorts()
	{	return refPorts;
	}
	
	public JBuilderObject getReferred()
	{	return ref;
	}
	
}
