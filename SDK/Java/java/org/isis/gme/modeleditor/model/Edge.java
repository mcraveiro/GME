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
 * Created on Sep 24, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */

package org.isis.gme.modeleditor.model;

import java.util.List;

import org.isis.gme.mga.MgaAttribute;
import org.isis.gme.mga.MgaFCO;
import org.isis.gme.mga.MgaSimpleConnection;
import org.isis.gme.modeleditor.autorouter.Path;

/**
 *
 * @author  bogyom
 */
public class Edge extends GmeElement
{
    // line types
    public static final int LINE_TYPE_SOLID   = 0;
    public static final int LINE_TYPE_DASH    = 1;
    
    // edge end types
    public static final int END_TYPE_NONE     = 0;
    public static final int END_TYPE_ARROW    = 1;
    public static final int END_TYPE_DIAMOND  = 2;
    
    private GmeSubpart          source        = null;
    private GmeSubpart          target        = null;
    private MgaSimpleConnection mgaConnection = null;
    private Path                path          = null;
    
    private int                 lineType      = LINE_TYPE_SOLID;
    private int                 srcEndType    = END_TYPE_NONE;
    private int                 destEndType   = END_TYPE_NONE;
    private String 				destLabel1    = "1";
	private String 				destLabel2    = "2";
	private String 				srcLabel1     = "3";
	private String 				srcLabel2     = "4";
	private String 				label    	  = "5";    
    
    protected boolean value;

    public Edge()
    {
    }
        
    public Edge(MgaSimpleConnection mgaConnection)
    {
        this.mgaConnection = mgaConnection;

        // set line style        
        if( mgaConnection.getRegistryValue( MgaFCO.CONN_LINE_TYPE_PREF ).equals("dash") )
            lineType = LINE_TYPE_DASH;
            
        // set line src enad style        
        srcEndType = endTypeFromString( mgaConnection.getRegistryValue( MgaFCO.CONN_SRC_END_STYLE_PREF ) );      
            
        // set line dest end style
        destEndType = endTypeFromString( mgaConnection.getRegistryValue( MgaFCO.CONN_DST_END_STYLE_PREF ) );
        
        // set src label
        srcLabel1 = mgaConnection.getRegistryValue( MgaFCO.CONN_SRC_LABEL1_PREF );
        srcLabel1 = formatLabel( srcLabel1 );
        srcLabel2 = mgaConnection.getRegistryValue( MgaFCO.CONN_SRC_LABEL2_PREF );
        srcLabel2 = formatLabel( srcLabel2 );
        
        // set dst label       
        destLabel1 = mgaConnection.getRegistryValue( MgaFCO.CONN_DST_LABEL1_PREF );
        destLabel1 = formatLabel( destLabel1 );
        destLabel2 = mgaConnection.getRegistryValue( MgaFCO.CONN_DST_LABEL2_PREF );
        destLabel2 = formatLabel( destLabel2 );
        
        // set label
        label = mgaConnection.getRegistryValue( MgaFCO.CONN_LABEL_FORMATSTR_PREF );
        label = formatLabel( label );
    }
    
    public String formatLabel( String label )
    {        
        label = label.replaceAll( "%name%", mgaConnection.getName() );
        label = label.replaceAll( "%kind%", mgaConnection.getMeta().getDisplayedName() );
        label = label.replaceAll( "%role%", mgaConnection.getMetaRole().getDisplayedName() );
        
        MgaAttribute attribs[] = mgaConnection.getAttributes().getAll();
        for( int i=0; i<attribs.length; ++i )           
            label = label.replaceAll( "%" + attribs[i].getMeta().getName() + "%", attribs[i].getValue().toString() );
            
        return label;
    }
    
    public int getLineType(){
    	return lineType;
    }
    
	public int getSrcEndType(){
		return srcEndType;
	}

	public int getDestEndType(){
		return destEndType;
	}

    private int endTypeFromString( String str )
    {
        if( str.equals( "arrow" ) )
            return END_TYPE_ARROW;
        if( str.equals( "diamond" ) )
            return END_TYPE_DIAMOND;
        return  END_TYPE_NONE;
    }
    
    public MgaSimpleConnection getMgaSimpleConnection()
    {
        return mgaConnection;
    }

    public void attachSource()
    {
        if (getSource() == null || getSource().getConnections().contains(this))
            return;
        getSource().connectOutput(this);
    }

    public void attachTarget()
    {
        if (getTarget() == null || getTarget().getConnections().contains(this))
            return;
        getTarget().connectInput(this);
    }

    public void detachSource()
    {
        if (getSource() == null)
            return;
        getSource().disconnectOutput(this);
    }

    public void detachTarget()
    {
        if (getTarget() == null)
            return;
        getTarget().disconnectInput(this);
    }

    public List getBendpoints(){
    	return path.getPoints();
    }

    public GmeSubpart getSource()
    {
        return source;
    }

    public Path getPath()
    {
        return path;
    }

    public void setPath(Path aPath)
    {
        path = aPath;
    }

    public GmeSubpart getTarget()
    {
        return target;
    }

    public boolean getValue()
    {
        return value;
    }

    public void setSource(GmeSubpart e)
    {
        Object old = source;
        source = e;
        firePropertyChange("source", old, source); //$NON-NLS-1$
    }

    public void setTarget(GmeSubpart e)
    {
        target = e;
    }

    public void setValue(boolean value)
    {
        if (value == this.value)
            return;
        this.value = value;
        if (target != null)
            target.update();
        firePropertyChange("value", null, null); //$NON-NLS-1$
    }

    public String toString(){
        return "Wire(" + getSource() + "," + getSource() + "->" + getTarget() + "," + getTarget() + ")";
    }

	public void refreshBendPoints(){
		firePropertyChange("bendpoint", null, null);
	}

	/**
	 * @return
	 */
	public String getDestLabel1() {
		return destLabel1;
	}

	/**
	 * @return
	 */
	public String getDestLabel2() {
		return destLabel2;
	}

	/**
	 * @return
	 */
	public String getLabel() {
		return label;
	}

	/**
	 * @return
	 */
	public String getSrcLabel1() {
		return srcLabel1;
	}

	/**
	 * @return
	 */
	public String getSrcLabel2() {
		return srcLabel2;
	}

}
