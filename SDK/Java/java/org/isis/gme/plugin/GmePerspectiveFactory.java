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
 * Created on Sep 17, 2003
 * 
 * Contributors: Gyorgy Balogh, Gabor Pap
 */

package org.isis.gme.plugin;

import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;


/**
 * GmePerspectiveFactory.java
 * 
 * @author Papszi
 */
public class GmePerspectiveFactory implements IPerspectiveFactory{

	public GmePerspectiveFactory() 
    {
	}

	public void createInitialLayout(IPageLayout layout) 
    {        
		String editorArea = layout.getEditorArea();

		//IFolderLayout bottomFolder = layout.createFolder("bottom", IPageLayout.BOTTOM, (float) 0.70, editorArea);
		//IFolderLayout leftFolder  = layout.createFolder("left", IPageLayout.LEFT, (float) 0.75, editorArea);
                
        layout.addView( "org.isis.gme.modelexplorer.ModelExplorer", IPageLayout.RIGHT, (float) 0.75, editorArea  );
        layout.addView( "org.isis.gme.attributeview.AttributeView", IPageLayout.BOTTOM, (float) 0.70, editorArea  );
        layout.addView( "org.isis.gme.partbrowser.PartBrowser", IPageLayout.BOTTOM, (float) 0.70, editorArea  );                               
	}
}

