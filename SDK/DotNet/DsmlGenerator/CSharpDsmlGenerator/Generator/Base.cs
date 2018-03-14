using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.CodeDom;
using GME.MGA;
using System.Reflection;

namespace CSharpDSMLGenerator.Generator
{
    public class Base
    {
        protected MgaObject Subject;
        // These are not used yet.
        protected CodeTypeReferenceCollection BaseTypes;
        protected CodeTypeReferenceCollection Attributes;

        public CodeNamespace GeneratedClass { get; set; }

        public CodeNamespace GeneratedInterface { get; set; }

        protected List<MgaFCO> baseClasses = new List<MgaFCO>();

        protected List<MgaFCO> baseClassesWoObject = new List<MgaFCO>();

        protected string ClassName { get; set; }

        public Base(
            MgaObject subject,
            CodeTypeReferenceCollection baseTypes,
            CodeTypeReferenceCollection attributes)
        {
            Subject = subject;
            BaseTypes = baseTypes;
            Attributes = attributes;

            if (Subject.MetaBase.Name == "RootFolder")
            {
                ClassName = "RootFolder";
            }
            else
            {
                ClassName = subject.Name;
            }

            if (Subject.MetaBase.Name != "RootFolder")
            {
                baseClasses.Add(Subject as MgaFCO);
                baseClasses.AddRange(FCO.GetBaseClasses(Subject as MgaFCO));
            }

            if (Subject is MgaFCO)
            {
                baseClassesWoObject.AddRange(FCO.GetBaseClasses(Subject as MgaFCO));
            }

            //GeneratedClass = new CodeNamespace(Configuration.ProjectClassNamespace);
            //GeneratedInterface = new CodeNamespace(Configuration.ProjectIntefaceNamespace);

            GeneratedClass = new CodeNamespace(Configuration.GetClassNamespace(Subject));
            GeneratedInterface = new CodeNamespace(Configuration.GetInterfaceNamespace(Subject));

            GeneratedClass.Imports.Add(new CodeNamespaceImport("global::System.Linq"));
            //GeneratedClass.Imports.Add(new CodeNamespaceImport("global::GME.MGA"));

            //GeneratedInterface.Imports.Add(new CodeNamespaceImport("global::GME.MGA"));

            GenerateClassCode();
            GenerateInterfaceCode();
        }

        public virtual void GenerateClassCode()
        {
            string ClassName = Subject.Name;
            if (Subject.MetaBase.Name == "RootFolder")
            {
                ClassName = "RootFolder";
            }

            // Declare the new class
            CodeTypeDeclaration newClass = new CodeTypeDeclaration(ClassName);
            newClass.IsPartial = Configuration.IsPartial;

            if (ClassName == "RootFolder")
            {
                StringBuilder sb = new StringBuilder();
                sb.AppendLine("<summary>");
                sb.AppendLine(
                    "<para>This API code is compatible with the following paradigm:</para>");

                sb.AppendFormat(
                    "<para> - Name: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.name,
                    Environment.NewLine);

                sb.AppendFormat(
                    "<para> - Guid: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.guid,
                    Environment.NewLine);

                sb.AppendLine(
                    "<para>Additional information: </para>");

                sb.AppendFormat(
                    "<para> - Date: {0}</para>{1}",
                    Configuration.DsmlModel.ParadigmDate,
                    Environment.NewLine);

                sb.AppendFormat(
                    "<para> - Author: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.author,
                    Environment.NewLine);

                sb.AppendFormat(
                    "<para> - Version: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.version,
                    Environment.NewLine);

                sb.AppendFormat(
                    "<para> - Comment: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.comment,
                    Environment.NewLine);

                sb.AppendLine("</summary>");

                newClass.Comments.Add(new CodeCommentStatement(sb.ToString(), true));

                //        public class InfoClass
                //{
                //  public System.Guid ParadigmGuid 
                //  {
                //    get { return new System.Guid(""); }
                //  }
                //}

                //public InfoClass Info
                //{
                //  get { return new InfoClass(); }
                //}
                CodeTypeDeclaration newInfoClass = new CodeTypeDeclaration()
                {
                    Attributes = MemberAttributes.Public | MemberAttributes.Final,
                    Name = "InfoClass",
                };

                newInfoClass.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

                CodeMemberProperty GuidProperty = new CodeMemberProperty()
                {
                    Attributes = MemberAttributes.Public | MemberAttributes.Final,
                    HasGet = true,
                    Name = "ParadigmGuid",
                    Type = new CodeTypeReference("global::" + typeof(Guid).FullName),
                };

                GuidProperty.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

                GuidProperty.GetStatements.Add(
                    new CodeMethodReturnStatement(
                        new CodeObjectCreateExpression(
                            new CodeTypeReference("global::" + typeof(Guid).FullName),
                            new CodeSnippetExpression("\"" + Configuration.DsmlModel.Paradigm.guid + "\""))));

                newInfoClass.Members.Add(GuidProperty);

                CodeMemberProperty ParadigmNameProperty = new CodeMemberProperty()
                {
                    Attributes = MemberAttributes.Public | MemberAttributes.Final,
                    HasGet = true,
                    Name = "ParadigmName",
                    Type = new CodeTypeReference(typeof(string).FullName),
                };

                ParadigmNameProperty.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

                ParadigmNameProperty.GetStatements.Add(
                    new CodeMethodReturnStatement(
                        new CodeSnippetExpression("\"" + Configuration.DsmlModel.Paradigm.name + "\"")));

                newInfoClass.Members.Add(ParadigmNameProperty);

                newClass.Members.Add(newInfoClass);

                CodeMemberProperty Version = new CodeMemberProperty()
                {
                    Attributes = MemberAttributes.Public | MemberAttributes.Final,
                    HasGet = true,
                    Name = "ParadigmVersion",
                    Type = new CodeTypeReference(typeof(string).FullName),
                };

                Version.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

                Version.GetStatements.Add(
                    new CodeMethodReturnStatement(
                        new CodeSnippetExpression("\"" + Configuration.DsmlModel.Paradigm.version + "\"")));

                newInfoClass.Members.Add(Version);

                Dictionary<string, string> metarefs = new Dictionary<string, string>()
                {
                    { Configuration.DsmlModel.Paradigm.folder.metaref, Configuration.DsmlModel.Paradigm.folder.name }
                };

                foreach (var item in Configuration.DsmlModel.Paradigm.folder.Items ?? new object[] {})
                {
                    if (item is atom)
                    {
                        metarefs.Add((item as atom).metaref, Configuration.GetClassName((item as atom).name));
                    }
                    else if (item is model)
                    {
                        metarefs.Add((item as model).metaref, Configuration.GetClassName((item as model).name));
                    }
                    else if (item is folder)
                    {
                        metarefs.Add((item as folder).metaref, Configuration.GetClassName((item as folder).name));
                    }
                    else if (item is connection)
                    {
                        metarefs.Add((item as connection).metaref, Configuration.GetClassName((item as connection).name));
                    }
                    else if (item is set)
                    {
                        metarefs.Add((item as set).metaref, Configuration.GetClassName((item as set).name));
                    }
                    else if (item is reference)
                    {
                        metarefs.Add((item as reference).metaref, Configuration.GetClassName((item as reference).name));
                    }
                }

                CodeMemberProperty MetaRefs = new CodeMemberProperty()
                {
                    Attributes =
                        MemberAttributes.Public |
                        MemberAttributes.Override,
                    HasGet = true,
                    Name = "MetaRefs",
                    Type = new CodeTypeReference("global::System.Collections.Generic.Dictionary<int, global::System.Type>"),
                };

                MetaRefs.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

                StringBuilder sbMetaRefs = new StringBuilder();
                sbMetaRefs.AppendLine("new global::System.Collections.Generic.Dictionary<int, global::System.Type>()");
                sbMetaRefs.AppendLine("{");

                foreach (var kvp in metarefs)
                {
                    sbMetaRefs.Append("{");
                    sbMetaRefs.Append(kvp.Key); // metaref
                    sbMetaRefs.Append(", typeof(");
                    sbMetaRefs.Append(kvp.Value);
                    sbMetaRefs.Append(") }"); // name
                    if (kvp.Key != metarefs.LastOrDefault().Key)
                    {
                        sbMetaRefs.Append(",");
                    }
                    sbMetaRefs.AppendLine("");
                }

                sbMetaRefs.AppendLine("}");

                MetaRefs.GetStatements.Add(
                    new CodeMethodReturnStatement(
                        new CodeSnippetExpression(sbMetaRefs.ToString())));

                newClass.Members.Add(MetaRefs);


                CodeMemberProperty Info = new CodeMemberProperty()
                {
                    Attributes = MemberAttributes.Public | MemberAttributes.Final,
                    HasGet = true,
                    Name = "Info",
                    Type = new CodeTypeReference(Configuration.ProjectClassNamespace + ".RootFolder.InfoClass"),
                };

                Info.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

                Info.GetStatements.Add(
                    new CodeMethodReturnStatement(
                        new CodeObjectCreateExpression(
                            new CodeTypeReference(Configuration.ProjectClassNamespace + ".RootFolder.InfoClass"))));

                newClass.Members.Add(Info);
            }

            newClass.Comments.Add(
                new CodeCommentStatement(@"<summary>", true));

            //newClass.Comments.Add(new CodeCommentStatement("Class comments", true));

            newClass.Comments.Add(
                new CodeCommentStatement(@"</summary>", true));


            // only one class inheritance
            Type t = GetBaseType(Subject, false);
            newClass.BaseTypes.Add(new CodeTypeReference(t));

            // interface inheritance
            newClass.BaseTypes.Add(GeneratedInterface.Name + "." + ClassName);
            // TODO: add multiple inheritance
            if (Subject is MgaFCO)
            {
                foreach (MgaFCO fco in FCO.GetBaseClasses(Subject as MgaFCO).Distinct())
                {
                    newClass.BaseTypes.Add(Configuration.GetInterfaceName(fco as MgaObject));
                }
            }

            //if (Subject.MetaBase.Name == "Folder" ||
            //  Subject.MetaBase.Name == "Model" ||
            //  Subject.MetaBase.Name == "RootFolder")
            //{

            //  CodeMemberProperty newAllChildrenProperty = new CodeMemberProperty()
            //  {
            //    Attributes = MemberAttributes.Final,
            //    HasGet = true,
            //    Name = typeof(ISIS.GME.Common.Interfaces.Folder).FullName + ".AllChildren",
            //    Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Base).FullName + ">"),
            //  };

            //  newAllChildrenProperty.Comments.Add(new CodeCommentStatement("Contains all type of child objects", true));

            //  newAllChildrenProperty.GetStatements.Add(
            //    new CodeMethodReturnStatement(new CodeSnippetExpression("((" + Configuration.ProjectNamespace + "." + ClassName + ")this).AllChildren")));

            //  newClass.Members.Add(newAllChildrenProperty);
            //}


            // Add the new type to the namespace type collection.
            GeneratedClass.Types.Add(newClass);

        }
        public virtual void GenerateInterfaceCode()
        {
            string ClassName = Subject.Name;
            if (Subject.MetaBase.Name == "RootFolder")
            {
                ClassName = "RootFolder";
            }

            // Declare interface
            CodeTypeDeclaration newInterface = new CodeTypeDeclaration(ClassName);
            newInterface.IsPartial = Configuration.IsPartial;

            if (ClassName == "RootFolder")
            {
                StringBuilder sbComment = new StringBuilder();
                sbComment.AppendLine("<summary>");
                sbComment.AppendLine(
                    "<para>This API code is compatible with the following paradigm:</para>");

                sbComment.AppendFormat(
                    "<para> - Name: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.name,
                    Environment.NewLine);

                sbComment.AppendFormat(
                    "<para> - Guid: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.guid,
                    Environment.NewLine);

                sbComment.AppendLine(
                    "<para>Additional information: </para>");

                sbComment.AppendFormat(
                    "<para> - Date: {0}</para>{1}",
                    Configuration.DsmlModel.ParadigmDate,
                    Environment.NewLine);

                sbComment.AppendFormat(
                    "<para> - Author: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.author,
                    Environment.NewLine);

                sbComment.AppendFormat(
                    "<para> - Version: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.version,
                    Environment.NewLine);

                sbComment.AppendFormat(
                    "<para> - Comment: {0}</para>{1}",
                    Configuration.DsmlModel.Paradigm.comment,
                    Environment.NewLine);

                sbComment.AppendLine("</summary>");

                newInterface.Comments.Add(new CodeCommentStatement(sbComment.ToString(), true));

                CodeMemberProperty Info = new CodeMemberProperty()
                {
                    Attributes = MemberAttributes.Public | MemberAttributes.Final,
                    HasGet = true,
                    Name = "Info",
                    Type = new CodeTypeReference(Configuration.ProjectClassNamespace + ".RootFolder.InfoClass"),
                };

                Info.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

                newInterface.Members.Add(Info);

                CodeMemberProperty MetaRefs = new CodeMemberProperty()
                {
                    Attributes =
                        MemberAttributes.Public |
                        MemberAttributes.New,
                    HasGet = true,
                    Name = "MetaRefs",
                    Type = new CodeTypeReference("global::System.Collections.Generic.Dictionary<int, global::System.Type>"),
                };

                MetaRefs.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

                newInterface.Members.Add(MetaRefs);
            }


            Type t = GetBaseType(Subject);
            newInterface.BaseTypes.Add(new CodeTypeReference(t));

            // TODO: add multiple inheritance ==> we do not need multiple inheritance...

            newInterface.Comments.Add(
                new CodeCommentStatement(@"<summary>", true));

            StringBuilder sb = new StringBuilder();
            sb.Append(@"<para>");
            sb.AppendFormat("{0} interface", ClassName);
            sb.Append(@"</para>");
            newInterface.Comments.Add(
                new CodeCommentStatement(sb.ToString(), true));

            if (Subject is MgaFCO)
            {
                string description = (Subject as MgaFCO).RegistryValue["description"];
                if (string.IsNullOrEmpty(description) == false)
                {
                    sb.Clear();
                    sb.Append(@"<para>");
                    sb.AppendFormat("{0}", description);
                    sb.Append(@"</para>");
                    newInterface.Comments.Add(
                        new CodeCommentStatement(sb.ToString(), true));
                }

                var baseClasses = FCO.GetBaseClasses(Subject as MgaFCO).Distinct();

                if (baseClasses.Count() > 0)
                {
                    sb.Clear();
                    sb.Append(@"<para>");
                    sb.AppendFormat("-----------------------------------------------");
                    sb.Append(@"</para>");
                    newInterface.Comments.Add(
                        new CodeCommentStatement(sb.ToString(), true));

                    sb.Clear();
                    sb.Append(@"<para>");
                    sb.AppendFormat("Base types:");
                    sb.Append(@"</para>");
                    newInterface.Comments.Add(
                        new CodeCommentStatement(sb.ToString(), true));
                }

                foreach (MgaFCO fco in baseClasses)
                {
                    sb.Clear();
                    sb.Append(@"<para>");
                    sb.AppendFormat("- {0}", Configuration.GetKindName(fco as MgaObject));
                    sb.Append(@"</para>");
                    newInterface.Comments.Add(
                        new CodeCommentStatement(sb.ToString(), true));
                }
            }

            newInterface.Comments.Add(
                new CodeCommentStatement(@"</summary>", true));

            newInterface.IsInterface = true;

            if (Subject is MgaFCO)
            {
                foreach (MgaFCO fco in FCO.GetBaseClasses(Subject as MgaFCO).Distinct())
                {
                    newInterface.BaseTypes.Add(Configuration.GetInterfaceName(fco as MgaObject));
                }
            }

            // Add the new type to the namespace type collection.
            GeneratedInterface.Types.Add(newInterface);
        }

        private Type GetBaseType(MgaObject subject, bool iface = true)
        {
            Type result = null;
            // TODO: use appropriate base class ...
            switch (subject.MetaBase.Name)
            {
                case "RootFolder":
                    if (iface)
                    {
                        result = typeof(ISIS.GME.Common.Interfaces.RootFolder);
                    }
                    else
                    {
                        result = typeof(ISIS.GME.Common.Classes.RootFolder);
                    }
                    break;

                case "Folder":
                    if (iface)
                    {
                        result = typeof(ISIS.GME.Common.Interfaces.Folder);
                    }
                    else
                    {
                        result = typeof(ISIS.GME.Common.Classes.Folder);
                    }
                    break;

                case "Model":
                    if (iface)
                    {
                        result = typeof(ISIS.GME.Common.Interfaces.Model);
                    }
                    else
                    {
                        result = typeof(ISIS.GME.Common.Classes.Model);
                    }
                    break;

                case "Connection":
                    if (iface)
                    {
                        result = typeof(ISIS.GME.Common.Interfaces.Connection);
                    }
                    else
                    {
                        result = typeof(ISIS.GME.Common.Classes.Connection);
                    }
                    break;

                case "Atom":
                    if (iface)
                    {
                        result = typeof(ISIS.GME.Common.Interfaces.Atom);
                    }
                    else
                    {
                        result = typeof(ISIS.GME.Common.Classes.Atom);
                    }
                    break;

                case "FCO":
                    if (iface)
                    {
                        result = typeof(ISIS.GME.Common.Interfaces.FCO);
                    }
                    else
                    {
                        result = typeof(ISIS.GME.Common.Classes.FCO);
                    }
                    break;

                case "Reference":
                    if (iface)
                    {
                        result = typeof(ISIS.GME.Common.Interfaces.Reference);
                    }
                    else
                    {
                        result = typeof(ISIS.GME.Common.Classes.Reference);
                    }
                    break;

                case "Set":
                    if (iface)
                    {
                        result = typeof(ISIS.GME.Common.Interfaces.Set);
                    }
                    else
                    {
                        result = typeof(ISIS.GME.Common.Classes.Set);
                    }
                    break;

                default:
                    if (iface)
                    {
                        result = typeof(ISIS.GME.Common.Interfaces.Base);
                    }
                    else
                    {
                        result = typeof(ISIS.GME.Common.Classes.Base);
                    }
                    break;
            }
            return result;
        }
    }
}
