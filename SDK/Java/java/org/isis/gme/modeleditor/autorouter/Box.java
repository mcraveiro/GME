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
 * Box.java
 *
 * Created on September 9, 2003, 3:12 PM
 */

package org.isis.gme.modeleditor.autorouter;


/**
 *
 * @author  bogyom
 */
public class Box extends Native
{       
    protected native int  createNative    ();
    protected native void deleteNative    ( int address );
    protected native void setRect         ( int address, int x1, int y1, int x2, int y2 );
    protected native void addPort         ( int address, int portAddress );   

    public Box()
    {
        //System.out.println( "Box.Box(), address=" + address );
    }
    
    public Box( int x1, int y1, int x2, int y2 )
    {
        //System.out.println( "Box.Box(), address=" + address );
        setRect( address, x1, y1, x2, y2 );        
    }
       
    public void setRect( int x1, int y1, int x2, int y2 )
    {
        setRect( address, x1, y1, x2, y2 );
    }
    
    public void addPort( Port port )
    {        
        //System.out.println( "Box.addPort(), addr=" + address + " port=" + port.getAddress() );
        addPort( address, port.getAddress() );
        port.doNotDeleteNativeInFinalize();
    }
}
