using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using GME.MGA;
using DSM.Generators;
using System.Diagnostics;

namespace DSM
{
    public static class GeneratorFacade
    {

        // Check this after the generation
        public static List<string> Errors = new List<string>();

        // Full path of the enerated files - add to the project after the generation
        public static List<string> generatedFiles = new List<string>();

        // Generator Entry Point
        public static string main(string projectPathFull, string targetPath, string namespaceName)
        {

            Clear();
            Generator.Path = targetPath;
            IMgaProject project = new MgaProject();
            IMgaTerritory territory = null;
            try
            {
                bool ro_mode = true;
                project.Open("MGA=" + projectPathFull, out ro_mode);
                territory = project.BeginTransactionInNewTerr(transactiontype_enum.TRANSACTION_READ_ONLY);
                GME.MGA.IMgaFolder root = project.RootFolder as GME.MGA.IMgaFolder;

                Generator.ClassName = root.Name;
                Generator.NamespaceName = namespaceName;


                foreach (GME.MGA.IMgaFCO fco in root.ChildFCOs)
                {
                    ProcessParadigmSheet(fco);
                }

                foreach (GME.MGA.IMgaFolder fol in root.ChildFolders)
                {
                    foreach (GME.MGA.IMgaFCO fco in fol.ChildFCOs)
                    {
                        ProcessParadigmSheet(fco);
                    }
                }

                RootFolder rootFolder = new RootFolder();
                rootFolder.Save();

                foreach (DSM.Generators.Object elem in DSM.Generators.Object.ElementsByName.Values)
                {
                    elem.Save();
                }

                DSM.Generators.Enum.Save();
                DSM.Generators.Generator.Save();

                if (Errors.Count > 0)
                {
                    StringBuilder sb = new StringBuilder();
                    sb.AppendLine("Errors during generation process:");
                    foreach (string item in Errors)
                    {
                        sb.AppendLine(item);
                    }
                }

                return Generator.ClassName;
            }
            catch(Exception e)
            {
                MessageBox.Show("Error generationg domain-specific interface: " + e.Message);
                return null;
            }
            finally
            {
                if (territory != null)
                {
                    territory.Destroy();
                    project.AbortTransaction();
                    
                }
                project.Close(true);

            }
        }

        // Private functions
        private static void Clear()
        {
            DSM.Generators.Object.ElementsByName.Clear();
            DSM.Generators.Object.ProxyObjects.Clear();
            DSM.Generators.Object.ProxyCache.Clear();
            DSM.Generators.Enum.Clear();
            DSM.Generators.RootFolder.RootObject.Clear();
            Errors.Clear();
            generatedFiles.Clear();
        }
        private static void ProcessParadigmSheet(GME.MGA.IMgaFCO fco)
        {
            Debug.Assert(fco.Meta.Name == "ParadigmSheet");

            GME.MGA.IMgaModel model = fco as GME.MGA.IMgaModel;

            foreach (GME.MGA.IMgaObject obj in model.ChildObjects)
            {
                DSM.Generators.Object object1;
                if (obj is GME.MGA.IMgaAtom)
                {
                    GME.MGA.IMgaAtom o = obj as GME.MGA.IMgaAtom;
                    switch (o.Meta.Name)
                    {
                        case "Folder":
                            object1 = new Folder(o);
                            break;
                        case "Model":
                            object1 = new Model(o);
                            break;
                        case "FCO":
                            object1 = new FCO(o);
                            break;
                        case "Atom":
                            object1 = new Atom(o);
                            break;
                        case "Reference":
                            object1 = new Reference(o);
                            break;
                        case "Set":
                            object1 = new Set(o);
                            break;
                        case "Connection":
                            object1 = new Connection(o);
                            break;
                        default:

                            //not supported
                            break;
                    }
                }
                else
                {
                    if (obj.MetaBase.Name.Contains("Proxy"))
                    {
                        GME.MGA.IMgaReference proxy = obj as GME.MGA.IMgaReference;

                        string referred = proxy.Referred.Name;

                        if (DSM.Generators.Object.ProxyCache.ContainsKey(proxy.Name) &&
                            DSM.Generators.Object.ProxyCache[proxy.Name] != referred)
                        {
                            Errors.Add(string.Format("Proxies with name '{0}' refer different objects", proxy.Name));
                        }
                        else
                        {
                            DSM.Generators.Object.ProxyCache[proxy.Name] = referred;
                            DSM.Generators.Object.ProxyObjects[proxy.Name] = proxy;
                        }
                    }
                }
            }  
        }
    }
}
