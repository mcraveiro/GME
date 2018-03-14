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
 * Native.java
 *
 * Created on September 10, 2003, 9:22 AM
 */

package org.isis.gme.modeleditor.autorouter;

/**
 *
 * @author  bogyom
 */
class Native
{      
    protected int       address = 0;    // address of the underlying object     
    protected boolean   deleteNativeInFinalize = true;
    
    protected int createNative()
    {
        return 0;
    }
    
    protected void deleteNative( int address )
    {
    }
                
    public Native()
    {
        address = createNative();
    }
    
    public Native( int address )
    {
        // todo: throw exception if null or not??
        this.address = address;
    }
    
    public void doNotDeleteNativeInFinalize()
    {
        deleteNativeInFinalize = false;        
    }
    
    public int getAddress()
    {
        return address;
    }
    
    protected void finalize()
    {
        if( deleteNativeInFinalize )
            deleteNative( address );
    }
    
    static
    {
        System.loadLibrary("AutoRouter");
    }
}
