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
 * Created on Sep 25, 2003
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package org.isis.gme.mgautil;

import org.isis.jaut.Dispatch;

/**
 * @author bogyom
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class MgaRegistrar extends Dispatch
{   
    public static final int REGACCESS_NONE     = 0;
    public static final int REGACCESS_USER     = 1;
    public static final int REGACCESS_SYSTEM   = 2;
    public static final int REGACCESS_PRIORITY = 5;
    public static final int REGACCESS_BOTH     = 3;
    public static final int REGACCESS_TEST     = 8;
    
    public MgaRegistrar()
    {
        attachNewInstance( "Mga.MgaRegistrar", Dispatch.CLSCTX_INPROC_SERVER );            
    }
    
    public MgaRegistrar( Dispatch d )
    {
        attach( d );
        changeInterface( "{F1D6BB05-42EE-11D4-B3F4-005004D38590}" );
    }
    
    public String getIconPath( int accessMode )
    {
        return (String)get( "IconPath", new Integer(accessMode) );        
    }
    
    
    /*[propput, helpstring("property IconPath")]
    HRESULT IconPath([in] regaccessmode_enum mode, [in] BSTR path);

    [propget, helpstring("property ShowMultipleView")]
    HRESULT ShowMultipleView([in] regaccessmode_enum mode, [out, retval] VARIANT_BOOL *enabled);

    [propput, helpstring("property ShowMultipleView")]
    HRESULT ShowMultipleView([in] regaccessmode_enum mode, [in] VARIANT_BOOL enabled);*/
    
    
}
