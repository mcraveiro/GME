using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using GME.MGA;
using GME.MGA.Meta;
using System.IO;
using System.Text.RegularExpressions;

namespace DumpWMF
{
    // Dump all
    // import win32com.client; win32com.client.DispatchEx("MGA.DumpWMF").DumpWMFs(r'C:\Users\ksmyth\Desktop\tmp', gme)
    // Dump under opened model
    // import win32com.client; win32com.client.DispatchEx("MGA.DumpWMF").DumpWMFs(r'C:\Users\ksmyth\Desktop\tmp', gme.oleit.MgaModel)
    // Dump under selected node in Tree Browser
    // import win32com.client; win32com.client.DispatchEx("MGA.DumpWMF").DumpWMFs(r'C:\Users\ksmyth\Desktop\tmp', [panel for panel in gme.Panels if panel.Name == 'Browser'][0].Interface.GetSelectedMgaObjects().Item(1))

    [ComVisible(true),
    Guid("A051FEEA-E310-3F6A-8D71-A55E3F4F2E14"),
    ProgId("MGA.DumpWMF")]
    public class DumpWMF
    {
        public void DumpWMFs(string outdir, object obj)
        {
            GME.IGMEOLEApp app;
            IMgaObject root;
            if (obj is IMgaObject)
            {
                root = obj as IMgaObject;
                app = (GME.IGMEOLEApp)root.Project.GetClientByName("GME.Application").OLEServer;
                root.Project.BeginTransactionInNewTerr(transactiontype_enum.TRANSACTION_NON_NESTED);
            }
            else
            {
                app = obj as GME.IGMEOLEApp;
                app.MgaProject.BeginTransactionInNewTerr(transactiontype_enum.TRANSACTION_NON_NESTED);
                root = app.MgaProject.RootFolder;
            }
            Dictionary<IMgaModel, string> models = new Dictionary<IMgaModel, string>();
            try
            {
                Queue<IMgaObject> objects = new Queue<IMgaObject>();
                objects.Enqueue(root);
                while (objects.Count > 0)
                {
                    IMgaObject o = objects.Dequeue();
                    if (o.ObjType == objtype_enum.OBJTYPE_FOLDER)
                    {
                        foreach (var i in (o as IMgaFolder).ChildObjects)
                            objects.Enqueue(i as IMgaObject);
                    }
                    else if (o.ObjType == objtype_enum.OBJTYPE_MODEL)
                    {
                        foreach (var i in (o as IMgaModel).ChildFCOs)
                            objects.Enqueue(i as IMgaFCO);
                        if ((o as IMgaModel).ChildFCOs.Count > 0)
                            models.Add(o as IMgaModel, o.Name + " " + o.ID);
                    }
                }
            }
            finally
            {
                root.Project.AbortTransaction();
            }
            foreach (var ent in models)
            {
                app.ShowFCO(ent.Key as MgaFCO);
                app.OleIt.DumpWindowsMetaFile(Path.Combine(outdir, RemoveInvalidFilePathCharacters(ent.Value, "")) + ".wmf");
                app.OleIt.Close();
            }
        }

        public static string RemoveInvalidFilePathCharacters(string filename, string replaceChar)
        {
            string regexSearch = new string(Path.GetInvalidFileNameChars()) + new string(Path.GetInvalidPathChars());
            Regex r = new Regex(string.Format("[{0}]", Regex.Escape(regexSearch)));
            return r.Replace(filename, replaceChar);
        }
    }
}
