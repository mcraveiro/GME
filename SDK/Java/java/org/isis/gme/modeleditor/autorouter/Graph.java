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
 * autorouter.java
 *
 * Created on September 9, 2003, 2:59 PM
 */

package org.isis.gme.modeleditor.autorouter;

/*public class CPPWrapper
{
    protected int cpDispatch = 0;    
    attach()    
}*/

public class Graph extends Native
{    
    protected native int  createNative    ();
    protected native void deleteNative    ( int address );
    protected native void addBox          ( int address, int boxAddress );
    protected native int  addPath         ( int address, int srcPortAddr, int destPortAddr );
    protected native void removeBox       ( int address, int boxAddress );
    protected native void removePath      ( int address, int pathAddress );
    protected native int  autoRoute       ( int address );                
       
    public void addBox( Box box )
    {        
        //System.out.println( "Graph.addBox(), box=" + box.getAddress() );
        addBox( address, box.getAddress() );        
        box.doNotDeleteNativeInFinalize();
    }
    
    public void removeBox( Box box )
    {
        removeBox( address, box.address );           
    }
    
    public Path addPath( Port src, Port dest )
    {
        //System.out.println( "Graph.addPath(), src=" + src.getAddress() + " dst=" + dest.getAddress() );               
        Path path = new Path( addPath( address, src.getAddress(), dest.getAddress() ) );
        path.doNotDeleteNativeInFinalize();
        return path;
    }
    
    public void removePath( Path path )
    {
        removePath( address, path.address );    
    }
    
    public int autoRoute()
    {
        return autoRoute( address );
    }
}
