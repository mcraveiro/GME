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
 * Created on Sep 26, 2003
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package org.isis.gme.mgadecorator;

import org.isis.gme.meta.MgaMetaPart;
import org.isis.gme.mga.*;
import org.isis.jaut.*;

/**
 * @author bogyom
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class MgaDecorator
{
    protected int         address = 0;    // address of the underlying native object
    protected native int  createNative     ();
    protected native int  createNative     ( String progid );
    protected native void initialize       ( int address, int project, int meta, int fco );
    protected native void destroy          ( int address );    
    protected native void draw             ( int address, int hdc );
    protected native void setLocation      ( int address, int sx, int sy, int ex, int ey );
    protected native void getLocation      ( int address, int[] loc );
    protected native void getPreferredSize ( int address, int[] loc );
    protected native void setActvie        ( int address, boolean active );    
    protected native int  getPorts         ( int address );
    protected native void getPortLocation  ( int address, int fco, int[] loc );
    
    public class Location
    {
        public int sx = 0;
        public int sy = 0;
        public int ex = 0;
        public int ey = 0;
        
        public Location()
        {
        }
        
        public Location( int sx, int sy, int ex, int ey )
        {
            this.sx = sx;
            this.sy = sy;
            this.ex = ex;
            this.ey = ey;
        }
    }
    
    public class Size
    {
        public int sizex = 0;
        public int sizey = 0;
        
        public Size( int sizex, int sizey )
        {
            this.sizex = sizex;
            this.sizey = sizey;
        }
    }   

    public MgaDecorator()
    {
        address = createNative();
    }
    
    public MgaDecorator( String progID )
    {
        if( progID.length() == 0 )
            address = createNative();
        else
            address = createNative( progID );
    }    
            
    public void initialize( MgaProject project, MgaMetaPart meta, MgaFCO fco )
    {
        initialize( address, project.pDispatch, meta.pDispatch, fco.pDispatch );      
    }
    
    public void destroy()
    {
        destroy( address );      
    }
    
    public Location getLocation()
    {
        int[] loc = new int[4];
        getLocation( address, loc );
        return new Location( loc[0], loc[1], loc[2], loc[3] );               
    }
    
    public void setLocation( Location loc )
    {
        setLocation( address, loc.sx, loc.sy, loc.ex, loc.ey );        
    }
    
    public Size getPreferredSize()
    {
        int[] size = new int[2];
        getPreferredSize ( address, size );
        return new Size( size[0], size[1] );        
    }
    
    public void setActvie(  boolean active )
    {
        setActvie( address, active );               
    }
    
    public MgaFCOs getPorts()
    {
        Dispatch disp = new Dispatch();
        disp.pDispatch = getPorts( address );        
        return new MgaFCOs( disp );
    }
    
    public Location getPortLocation( MgaFCO fco )
    {
        int loc[] = new int[4];
        getPortLocation( address, fco.pDispatch, loc );        
        return new Location( loc[0], loc[1], loc[2], loc[3] );        
    }
        
    public void draw( int hdc )
    {
        draw( address, hdc );        
    }
    
    /*
    [id(3), helpstring("method GetMnemonic")] HRESULT GetMnemonic([out] BSTR *mnemonic);
    [id(4), helpstring("method GetFeatures")] HRESULT GetFeatures([out] feature_code *features);
    [id(5), helpstring("method SetParam")] HRESULT SetParam([in] BSTR name, [in] VARIANT value);
    [id(6), helpstring("method GetParam")] HRESULT GetParam([in] BSTR name, [out] VARIANT *value);
    [id(7), helpstring("method SetActive")] HRESULT SetActive([in] VARIANT_BOOL isActive);
    [id(11), helpstring("method GetLabelLocation")] HRESULT GetLabelLocation([out] long *sx, [out] long *sy, [out] long *ex, [out] long *ey);
    [id(12), helpstring("method GetPortLocation")] HRESULT GetPortLocation([in] IMgaFCO *fco, [out] long *sx, [out] long *sy, [out] long *ex, [out] long *ey);
    [id(13), helpstring("method GetPorts")] HRESULT GetPorts([out, retval] IMgaFCOs **portFCOs);
    [id(14), helpstring("method Draw")] HRESULT Draw([in] HDC hdc);
    [id(15), helpstring("method SaveState")] HRESULT SaveState();
    */
    
    static
    {
        System.loadLibrary("MgaDecoratorWrap");
    }    
    
}
