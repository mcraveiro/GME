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
 * Created on Sep 24, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */

package org.isis.gme.modeleditor.view;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.geometry.PointList;
import org.isis.gme.mgadecorator.MgaDecorator;
import org.isis.gme.modeleditor.model.Edge;

/**
 * FigureFactory.java
 * 
 * @author Papszi
 */
public class FigureFactory {

	public static final PointList DIAMOND = new PointList();
	static {
		DIAMOND.addPoint(0, 0);
		DIAMOND.addPoint(-1, 1);
		DIAMOND.addPoint(-2, 0);
		DIAMOND.addPoint(-1, -1);
	}

	public static IFigure createNewNode(MgaDecorator decorator) {
		return new NodeFigure(decorator);
	}

	public static PolylineConnection createNewBendableEdge(Edge edge){
		if (edge == null){
			return new EdgeFigure();
		}
		EdgeFigure conn = new EdgeFigure(edge.getDestLabel1(),edge.getDestLabel2(),edge.getSrcLabel1(),edge.getSrcLabel2(),edge.getLabel());
		PolygonDecoration arrow, diamond;

		if (edge.getLineType() == Edge.LINE_TYPE_DASH){
			conn.setLineStyle(Graphics.LINE_DOT);
		}
		
		switch (edge.getSrcEndType()){
			case Edge.END_TYPE_ARROW:
				arrow = new PolygonDecoration();
				arrow.setScale(5, 2.5);
				conn.setSourceDecoration(arrow);
				break;
			case Edge.END_TYPE_DIAMOND:
				diamond = new PolygonDecoration();
				diamond.setTemplate(DIAMOND);
				diamond.setScale(5, 2.5);
				conn.setSourceDecoration(diamond);
				break;
		}
		switch (edge.getDestEndType()){
			case Edge.END_TYPE_ARROW:
				arrow = new PolygonDecoration();
				arrow.setScale(5, 2.5);
				conn.setTargetDecoration(arrow);
				break;
			case Edge.END_TYPE_DIAMOND:
				diamond = new PolygonDecoration();
				diamond.setTemplate(DIAMOND);
				diamond.setScale(5, 2.5);
				conn.setTargetDecoration(diamond);
				break;
		}
		return conn;
	}
}
