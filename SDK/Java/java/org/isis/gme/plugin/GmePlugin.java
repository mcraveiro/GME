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

package org.isis.gme.plugin;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.MissingResourceException;
import java.util.ResourceBundle;

import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPluginDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.isis.gme.attributeview.AttributeView;
import org.isis.jaut.Apartment;

/**
 * The main plugin class to be used in the desktop.
 */
public class GmePlugin extends AbstractUIPlugin
{
    private static GmePlugin plugin;
        
    private ResourceBundle resourceBundle;    
    private Projects       projects;
    private String         projectsFile;
    
    private AttributeView  attributeView = null;    

    public GmePlugin(IPluginDescriptor descriptor)
    {
        super(descriptor);
        plugin = this;
        try
        {
            resourceBundle = ResourceBundle.getBundle("org.isis.gme.GmePluginResources");
        }
        catch (MissingResourceException x)
        {
            resourceBundle = null;
        }
        
        // initialize com java bridge for this thread
        Apartment.enter(false);
        
        // create empty projects
        projects = new Projects();
        
        // create and read projects
        projectsFile = getStateLocation().toString() + "/projects.txt";
        System.out.println( "projectsFile=" + projectsFile );        
        loadProjects();              
    }   
    
    public synchronized void loadProjects()
    {
        try
        {           
            ObjectInputStream stream = new ObjectInputStream(new FileInputStream( projectsFile ));
            projects = (Projects)stream.readObject();
            stream.close();            
        }
        catch( Exception e )
        {
            System.out.println("Could not load projects file.");
            projects = new Projects();
            saveProjects();                       
        }                  
    }
    
    public synchronized void saveProjects()
    {
        try
        {
            ObjectOutputStream stream = new ObjectOutputStream(new FileOutputStream( projectsFile ));
            stream.writeObject( projects );
            stream.close();    
        }
        catch( Exception e )
        {
            System.out.println("Could not save projects file.");           
        }        
    }
    
    public void setAttributeView( AttributeView attributeView )
    {
        this.attributeView = attributeView;
    }
    
    public AttributeView getAttributeView()
    {
        return attributeView;
    }

    /**
     * Returns the shared instance.
     */
    public static GmePlugin getPlugin()
    {
        return plugin;
    }

    /**
     * Returns the workspace instance.
     */
    public static IWorkspace getWorkspace()
    {
        return ResourcesPlugin.getWorkspace();
    }

    /**
     * Returns the string from the plugin's resource bundle,
     * or 'key' if not found.
     */
    public static String getResourceString(String key)
    {
        ResourceBundle bundle = GmePlugin.getPlugin().getResourceBundle();
        try
        {
            return bundle.getString(key);
        }
        catch (MissingResourceException e)
        {
            return key;
        }
    }


    public ResourceBundle getResourceBundle()
    {
        return resourceBundle;
    }
    
    public Projects getProjects()
    {
        return projects;
    }
}
