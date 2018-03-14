using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace DSM.Generators
{
    public class Enum
    {
        public class Template
        {
            public static readonly string Enums =
@"
namespace {0}
{{
    {1}
}}
";
        }

        private static StringBuilder enumsForAttributes = new StringBuilder();
        private static List<string> names = new List<string>();
        public static void AddEnum(string name, string enum1)
        {
            if (!names.Contains(name))
            {
                enumsForAttributes.AppendLine(enum1);
                names.Add(name);
            }
        }
        public static void Clear()
        {
            enumsForAttributes = new StringBuilder();
            names.Clear();
        }

        public static string GenerateEnums()
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendFormat(
                Enum.Template.Enums,
                Generator.NamespaceName,
                enumsForAttributes.ToString());

            return sb.ToString();
        }

        public static void Save()
        {
            Directory.CreateDirectory(Path.Combine(Generator.Path, Generator.ClassName));

            string enumsPath = Path.Combine(Generator.Path, Generator.ClassName) + @"\Enums.cs";
            GeneratorFacade.generatedFiles.Add(enumsPath);
            using (TextWriter tw = new StreamWriter(enumsPath))
            {
                tw.WriteLine(GenerateEnums());
            }
        }
    }
}
