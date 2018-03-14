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
 * Author: Gyorgy Balogh, Gabor Pap
 * Date last modified: 10/23/03
 */

/*
 * Created on Oct 6, 2003
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package org.isis.gme.attributeview;

import org.eclipse.jface.viewers.ColumnLayoutData;
import org.eclipse.jface.viewers.ColumnWeightData;
import org.eclipse.jface.viewers.TableLayout;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.custom.TableTree;
import org.eclipse.swt.custom.TableTreeItem;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.part.ViewPart;
import org.isis.gme.mga.MgaAttribute;
import org.isis.gme.mga.MgaFCO;
import org.isis.gme.plugin.GmePlugin;

/**
 * @author bogyom
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class AttributeView extends ViewPart
{   
    private CTabFolder  tabFolder        = null;
    private Composite   tabPage1         = null;
    private TableTree   tableTreeAttribs = null;
    private MgaFCO      fco              = null;
    
    public AttributeView()
    {                      
        super();
        GmePlugin.getPlugin().setAttributeView( this );
    }
    
    public void dispose()
    {
        GmePlugin.getPlugin().setAttributeView( null );
        super.dispose();
    }
            
    public void createPartControl( Composite parent )
    {
        // create tab folder and tab items
        tabFolder = new CTabFolder(parent, SWT.TOP);        
        CTabItem tabItem1 = new CTabItem(tabFolder, SWT.NULL);
        tabItem1.setText("Attributes");       
        /*CTabItem tabItem2 = new CTabItem(tabFolder, SWT.NULL);
        tabItem2.setText("Preferences");        
        CTabItem tabItem3 = new CTabItem(tabFolder, SWT.NULL);
        tabItem3.setText("Properties");*/
        
        // create attributes page
        createAttributesPage( tabItem1 );
        tabFolder.setSelection(tabItem1);                                              
    }
    
    public void createAttributesPage( CTabItem tabitem )
    {
        tabPage1 = new Composite (tabFolder, SWT.NONE);
        tabPage1.setLayout( new FillLayout() );
        tabPage1.setVisible( true );

        tableTreeAttribs = new TableTree( tabPage1, 0 );        
        tabitem.setControl( tabPage1 );
              
        Table table = tableTreeAttribs.getTable();
        table.setLinesVisible(true);
        table.setHeaderVisible(false);

        TableColumn c1 = new TableColumn(table,0);                        
        TableColumn c2 = new TableColumn(table,0);
        
        ColumnLayoutData c1Layout = new ColumnWeightData( 60, true );
        ColumnLayoutData c2Layout = new ColumnWeightData( 200, true );

        TableLayout layout = new TableLayout();
        layout.addColumnData(c1Layout);
        layout.addColumnData(c1Layout);
        table.setLayout(layout);
        tabPage1.setVisible(true);        
    }
    
    public void setFCO( MgaFCO fco )
    {
        this.fco = fco;
        
        // fill attributes page
        tableTreeAttribs.removeAll();
        if( fco != null )
        {                        
            MgaAttribute attribs[] = fco.getAttributes().getAll();
            for( int i=0; i<attribs.length; ++i )
            {
                TableTreeItem item = new TableTreeItem( tableTreeAttribs, SWT.NONE, 0 );
                item.setText( 0, attribs[i].getMeta().getName() );
                item.setText( 1, attribs[i].getValue().toString() );                        
            }           
        }
    }
    
    public void setFocus()
    {
        tabPage1.setFocus();
    }
}
