using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace DSM.Generators
{
    public class RootFolder : Folder
    {
        new public class Template
        {
            public static readonly string Class =
@"
namespace {0}
{{
    public class {1} : {2}
    {{
        internal {3} mgaObject;
        public {1}({3} mgaObject)
        {{
            this.mgaObject = mgaObject;
        }}
    
        #region Common
{4}
        #endregion

        #region Containment
{5}
        #endregion
    }}
}}
";
        }

        public RootFolder()
            : base(null)
        {
            className = "RootFolder";
        }

        public static List<Object> RootObject = new List<Object>();

        protected override IEnumerable<Object> Contained
        {
            get
            {
                foreach (Object item in RootObject)
                {
                    yield return item;
                }
            }
        }

        public override string GenerateClass()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(Init());

            sb.AppendFormat(
                RootFolder.Template.Class,
                namespaceName,
                className,
                baseInterfaceName,
                memberType,
                GenerateCommon(),
                GenerateContainments());

            return sb.ToString();
        }

        public override void Save()
        {
            Directory.CreateDirectory(Path.Combine(Generator.Path, Generator.ClassName));

            string pathName = Path.Combine(Generator.Path, Generator.ClassName) + @"\RootFolder.cs";
            GeneratorFacade.generatedFiles.Add(pathName);
            using (TextWriter tw = new StreamWriter(pathName))
            {
                tw.WriteLine(GenerateClass());
            }
        }
    }
}
