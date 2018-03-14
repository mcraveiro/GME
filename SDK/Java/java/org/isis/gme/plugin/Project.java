/*
 * Copyright (c) 2002, Vanderbilt University
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
 * Author: Gyorgy Balogh
 * Date last modified: 10/23/03
 */

/*
 * Created on Sep 19, 2003
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package org.isis.gme.plugin;

import java.io.Serializable;
import org.isis.gme.mga.MgaProject;

/**
 * @author bogyom
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */

public class Project implements Serializable
{   
    transient private MgaProject mgaProject = null;
    transient private boolean    readOnly   = false;    
    transient private boolean    opened     = false; 
    
    private String     projectConnection = null;    
    private Projects   parent            = null;
    
    public Project( Projects parent, String projectConnection )
    {
        this.parent            = parent;
        this.projectConnection = projectConnection;
    }
    
    protected void finalize()
    {
        close();    
    }

    public boolean open()
    {                   
        try
        {
            mgaProject = MgaProject.createInstance();  
            readOnly = mgaProject.open( projectConnection );
            mgaProject.beginTransaction( null );
            opened = true;
        }
        catch( Exception e )
        {
            opened = false;
            e.printStackTrace();        
        }
        
        return opened;          
    }
    
    public void close()
    {
        if( opened )
        {
            //mgaProject.abortTransaction();
            mgaProject.commitTransaction();
            mgaProject.close();
            opened = false;            
        }
    }
    
    public boolean isOpened()
    {
        return opened; 
    }
    
    public Projects getParent()
    {
        return parent; 
    }       
    
    public MgaProject getMgaProject()
    {
        return mgaProject; 
    }     
    
    public String toString()
    {
        return projectConnection; 
    }
    
}
