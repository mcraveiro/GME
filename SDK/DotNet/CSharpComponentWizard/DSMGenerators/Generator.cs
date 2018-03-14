using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using System.IO;
using Microsoft.Win32;

namespace DSM.Generators
{
    public class Generator
    {
        public static string Path = @"";
        public static string ClassName = "Paradigm";
        public static string NamespaceName = "GME.CSharp";       

        private static void GenerateUtil()
        {
            /*
            Assembly assembly = Assembly.GetExecutingAssembly();
            using (StreamReader textStreamReader = new StreamReader(assembly.GetManifestResourceStream(assembly.GetName().Name +".Templates.Util.cs")))
            {
                string content = textStreamReader.ReadToEnd();

                content = content.Replace("##1##", Generator.NamespaceName);
                content = content.Replace("##2##", Generator.ClassName);

                Directory.CreateDirectory(System.IO.Path.Combine(Generator.Path, Generator.ClassName));
                string utilPath = System.IO.Path.Combine(Generator.Path, Generator.ClassName) + "\\Util.cs";
                GeneratorFacade.generatedFiles.Add(utilPath);
                using (TextWriter tw = new StreamWriter(utilPath))
                {
                    tw.WriteLine(content);
                }
            }
            */
        }

      
       

        private static void GenerateGeneralInterfaces()
        {
            Assembly assembly = Assembly.GetExecutingAssembly();
            using (StreamReader textStreamReader = new StreamReader(assembly.GetManifestResourceStream(assembly.GetName().Name+ ".Templates.General.cs")))
            {
                string content = textStreamReader.ReadToEnd();

                content = content.Replace("##1##", Generator.NamespaceName);

                Directory.CreateDirectory(System.IO.Path.Combine(Generator.Path, Generator.ClassName));
                string pathName = System.IO.Path.Combine(Generator.Path, Generator.ClassName) + "\\General.cs";
                GeneratorFacade.generatedFiles.Add(pathName);
                using (TextWriter tw = new StreamWriter(pathName))
                {
                    tw.WriteLine(content);
                }
            }
        }

      

        public static void Save()
        {
          
            GenerateGeneralInterfaces();
            GenerateUtil();
        }        
    }
}
