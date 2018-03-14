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
 * Test.java
 *
 * Created on September 9, 2003, 3:31 PM
 */

package org.isis.gme.modeleditor.autorouter;

import java.util.*;

/**
 *
 * @author  bogyom
 */
public class Test1
{
    public static void main(String[] args)
    {
        System.out.println( "start building graph" );
        
        Graph g = new Graph();              
        
        // create first box
        Box b1  = new Box( 10, 10, 100, 60 );
        Port p1 = new Port( 10, 10, 100, 60 );
        b1.addPort( p1 );
        g.addBox( b1 );
              
        // create second box
        Box b2  = new Box( 10, 200, 100, 260 );
        Port p2 = new Port( 10, 200, 100, 260 );
        b2.addPort( p2 );        
        g.addBox( b2 );
        
        // create second box
        Box b3  = new Box( 10, 100, 100, 160 );
        //Port p2 = new Port( 10, 200, 100, 260 );
        //b2.addPort( p2 );        
        g.addBox( b3 );
        
        // connect ports
        Path path = g.addPath( p1, p2 );
        
        System.out.println( "start autorouter" );
        
        // run the autorouting algorithm
        g.autoRoute();
        
        System.out.println( "finished" );
        
        // print out path points
        Vector points = path.getPoints();
        Iterator it = points.iterator();
        while( it.hasNext() )
        {
            Point point = (Point)it.next();
            System.out.println( point.x + "," + point.y );
        }
        
        // test incremental autorouting
        System.out.println( "test incremental autorouting" );
        g.removeBox( b3 );
        b3.setRect( 200, 100, 290, 160);
        g.addBox(b3);
        g.autoRoute();
        points = path.getPoints();
        it = points.iterator();
        while( it.hasNext() )
        {
            Point point = (Point)it.next();
            System.out.println( point.x + "," + point.y );
        }               
    }
}
