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
 * Last modified on 10/23/03
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */
/*
 * Port.java
 *
 * Created on September 9, 2003, 10:26 AM
 */

package org.isis.gme.modeleditor.model;

import org.eclipse.swt.graphics.Image;
import org.isis.gme.mga.MgaFCO;

/**
 *
 * @author  bogyom
 */
public class Port extends GmeSubpart
{
    private MgaFCO  fco           = null;
    private int     routingPrefs  = 0; 
    
	public Port(MgaFCO fco )
	{
		this.fco    = fco;
	}
    
    public Port( Node parent, MgaFCO fco )
    {
        super(parent);
        this.fco    = fco;
    }

    public MgaFCO getFCO()
    {
        return fco;
    }
    
    public void calcRoutingPrefs()
    {
        // location, size, size of parent must be set before
        if( location.x + size.width / 2 < parent.size.width / 2 )
        {
            // left side
            routingPrefs = org.isis.gme.modeleditor.autorouter.Port.START_ON_LEFT
                         | org.isis.gme.modeleditor.autorouter.Port.END_ON_LEFT;
          
        }
        else
        {
            // right side
            routingPrefs = org.isis.gme.modeleditor.autorouter.Port.START_ON_RIGHT
                         | org.isis.gme.modeleditor.autorouter.Port.END_ON_RIGHT;            
        }
        
        org.eclipse.jface.text.IDocument a = null;
    }
    
    public int getRoutingPref()
    {
        return routingPrefs; 
    }
       
	public Image getIconImage() 
    {
		return null;
	}

	protected String getNewID() 
    {
		return null;
	}
}
