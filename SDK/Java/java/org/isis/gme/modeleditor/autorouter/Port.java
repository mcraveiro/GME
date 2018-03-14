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
 * Port.java
 *
 * Created on September 9, 2003, 5:26 PM
 */

package org.isis.gme.modeleditor.autorouter;

/**
 *
 * @author  bogyom
 */
public class Port extends Native
{
    public final static int END_ON_TOP        = 0x0001; 
    public final static int END_ON_RIGHT      = 0x0002;
    public final static int END_ON_BOTTOM     = 0x0004;
    public final static int END_ON_LEFT       = 0x0008;
    public final static int END_ON_ALL        = 0x000F;

    public final static int START_ON_TOP      = 0x0010;
    public final static int START_ON_RIGHT    = 0x0020;
    public final static int START_ON_BOTTOM   = 0x0040;
    public final static int START_ON_LEFT     = 0x0080;
    public final static int START_ON_ALL      = 0x00F0;

    public final static int CONNECT_ON_ALL    = 0x00FF;
    public final static int CONNECT_TO_CENTER = 0x0100;

    public final static int START_END_HORIZ   = 0x00AA;
    public final static int START_END_VERT    = 0x0055;

    public final static int DEFAULT           = 0x00FF;   
    
    protected native int  createNative    ();
    protected native void deleteNative    ( int address );
    protected native void setRect         ( int address, int x1, int y1, int x2, int y2 );
    protected native void setAttributes   ( int address, int attr );
    protected native int  getAttributes   ( int address );    
    
    public Port()
    {
    }   
    
    public Port( int x1, int y1, int x2, int y2 )
    {
        setRect( address, x1, y1, x2, y2 );
    }
    
    public void setRect( int x1, int y1, int x2, int y2 )
    {
        setRect( address, x1, y1, x2, y2 );
    }
    
    public void setAttributes( int attr )
    {
        setAttributes( address, attr );
    }

    public int getAttributes()
    {
        return getAttributes( address );
    }   
}
