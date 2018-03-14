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
 * Path.java
 *
 * Created on September 10, 2003, 8:57 AM
 */

package org.isis.gme.modeleditor.autorouter;

import java.util.Vector;

/**
 *
 * @author  bogyom
 */
public class Path extends Native
{   
    public static final int END_ON_DEFAULT    = 0x0000;
    public static final int END_ON_TOP        = 0x0010; 
    public static final int END_ON_RIGHT      = 0x0020;
    public static final int END_ON_BOTTOM     = 0x0030;
    public static final int END_ON_LEFT       = 0x0040;
    public static final int END_MASK          = END_ON_TOP | END_ON_RIGHT | END_ON_BOTTOM | END_ON_LEFT;
    
    public static final int START_ON_DEFAULT  = 0x0000;
    public static final int START_ON_TOP      = 0x0100; 
    public static final int START_ON_RIGHT    = 0x0200;
    public static final int START_ON_BOTTOM   = 0x0300;
    public static final int START_ON_LEFT     = 0x0400;
    public static final int BEGIN_MASK        = START_ON_TOP | START_ON_RIGHT | START_ON_BOTTOM | START_ON_LEFT;
    
    protected native int  createNative    ();
    protected native void deleteNative    ( int address );
    protected native int  getCount        ( int address );
    protected native int  getHead         ( int address );
    protected native int  getNext         ( int address, int iterator );
    protected native int  getX            ( int address, int iterator );
    protected native int  getY            ( int address, int iterator );
    protected native void setEndDir       ( int address, int endDir );
    protected native void setStartDir     ( int address, int startDir );
    
    
    public Path()
    {
    }
    
    public Path( int address )
    {        
        super( address );
        //System.out.println( "Path.Path(), address=" + Integer.toString(address) );
    }
    
    public Vector getPoints()
    {
        //System.out.println( "Path.getPoints(), address=" + Integer.toString(address) );
        
        Vector res = new Vector();
        
        int iterator = getHead( address );
        int count    = getCount( address );
        //System.out.println("Path.getPoints count:"+ count + " addr "+ this);
        for( int i=0; i<count; ++i )
        {
			//System.out.print(".");
            res.add( new Point( getX( address, iterator ), getY( address, iterator ) ) );
            iterator = getNext( address, iterator );            
        }
        
        return res;        
    }
    
    public void setEndDir( int endDir )
    {
        setEndDir( address, endDir );            
    }
    
    public void setStartDir( int endDir )
    {
        setStartDir( address, endDir );        
    }
}
