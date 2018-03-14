using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;
using System.CodeDom;

namespace CSharpDSMLGenerator.Generator
{
    public partial class FCO
    {
        // note: do not include static functions in the generated interface

        void ClassCodeGetRootFolder()
        {
            if (Subject.MetaBase.Name == "RootFolder")
            {
                CodeMemberMethod newGetRootFolder = new CodeMemberMethod()
                {
                    Attributes = MemberAttributes.Public | MemberAttributes.Static | MemberAttributes.New,
                    Name = "GetRootFolder",
                    ReturnType = new CodeTypeReference(
                        Configuration.ProjectIntefaceNamespace + ".RootFolder"),
                };
                newGetRootFolder.Comments.Add(
                    new CodeCommentStatement("Gets the root folder form an Mga project.", true));

                newGetRootFolder.Parameters.Add(
                    new CodeParameterDeclarationExpression("global::" + typeof(MgaProject).FullName, "project"));

                newGetRootFolder.Statements.Add(
                    new CodeMethodReturnStatement(
                        new CodeSnippetExpression(
                        typeof(ISIS.GME.Common.Utils).FullName + ".CreateObject<RootFolder>(project.RootFolder as global::GME.MGA.MgaObject)")));

                GeneratedClass.Types[0].Members.Add(newGetRootFolder);
            }
        }

        void ClassCodeMakeConnection()
        {
            if (Subject.MetaBase.Name == "Connection")
            {
                if ((Subject as MgaFCO).BoolAttrByName["IsAbstract"])
                {
                    // abstract
                    return;
                }

                IEnumerable<MgaFCO> srcEnds = null;
                IEnumerable<MgaFCO> dstEnds = null;
                IEnumerable<MgaFCO> parents = null;

                if (Configuration.UseOnlyBaseForFunctions)
                {
                    srcEnds = GetSrcEnd(Subject as MgaFCO).Distinct();
                    dstEnds = GetDstEnd(Subject as MgaFCO).Distinct();
                    parents = GetParents(Subject as MgaFCO).Distinct();
                }
                else
                {
                    srcEnds = GetSrcEnd(Subject as MgaFCO, true)
                        .Distinct()
                        .Where(x => x.BoolAttrByName["IsAbstract"] == false);

                    dstEnds = GetDstEnd(Subject as MgaFCO, true)
                        .Distinct()
                        .Where(x => x.BoolAttrByName["IsAbstract"] == false);

                    parents = GetParents(Subject as MgaFCO, true)
                        .Distinct()
                        .Where(x => x.BoolAttrByName["IsAbstract"] == false);
                }

                List<string> srcEndNames = GetSrcEnd(Subject as MgaFCO, true).
                        Where(x => x.BoolAttrByName["IsAbstract"] == false).
                        Select(x => x.Name)
                        .Distinct().
                        ToList();

                srcEndNames.Sort();

                List<string> dstEndNames = GetDstEnd(Subject as MgaFCO, true).
                        Where(x => x.BoolAttrByName["IsAbstract"] == false).
                        Select(x => x.Name)
                        .Distinct().
                        ToList();

                dstEndNames.Sort();

                // using this for comments
                List<string> parentsWderived = GetParents(Subject as MgaFCO, true).Distinct().
                    Where(x => x.BoolAttrByName["IsAbstract"] == false).
                    Select(x => x.Name).
                    ToList();

                parentsWderived.Sort();

                string role = "";
                StringBuilder sb = new StringBuilder();

                foreach (var parent in parents)
                {
                    foreach (var src in srcEnds)
                    {
                        foreach (var dst in dstEnds)
                        {
                            CodeMemberMethod newConnect = new CodeMemberMethod()
                            {
                                Attributes = MemberAttributes.Public | MemberAttributes.Static,
                                Name = "Connect",
                                ReturnType = new CodeTypeReference(
                                    Configuration.GetInterfaceName(Subject)),
                            };

                            sb.Clear();
                            sb.AppendLine("<summary>");
                            sb.AppendLine("Creates a new " + Subject.Name + " connection.");
                            sb.AppendLine("</summary>");

                            sb.AppendLine("<param name=\"src\">");
                            sb.AppendLine("<para>Source of the connection, which could be:</para>");
                            foreach (var item in srcEndNames)
                            {
                                sb.AppendFormat("<para>- {0}</para>{1}", item, Environment.NewLine);
                            }
                            sb.AppendLine("</param>");

                            sb.AppendLine("<param name=\"dst\">");
                            sb.AppendLine("<para>Destination of the connection, which could be:</para>");
                            foreach (var item in dstEndNames)
                            {
                                sb.AppendFormat("<para>- {0}</para>{1}", item, Environment.NewLine);
                            }
                            sb.AppendLine("</param>");

                            sb.AppendLine("<param name=\"srcRef\">");
                            sb.AppendLine("<para>Source reference parent.</para>");
                            // TODO: domain specific src ref parent
                            sb.AppendLine("</param>");

                            sb.AppendLine("<param name=\"dstRef\">");
                            sb.AppendLine("<para>Destination reference parent.</para>");
                            // TODO: domain specific dst ref parent
                            sb.AppendLine("</param>");

                            sb.AppendLine("<param name=\"parent\">");
                            sb.AppendLine("<para>Parent of the connection, which could be:</para>");
                            foreach (var item in parentsWderived)
                            {
                                sb.AppendFormat("<para>- {0}</para>{1}", item, Environment.NewLine);
                            }
                            sb.AppendLine("</param>");

                            sb.AppendLine("<param name=\"role\">");
                            sb.AppendLine("<para>Role of the connection</para>");
                            sb.AppendLine("</param>");
                            sb.AppendLine("<returns>The connection object, which has been created.</returns>");

                            newConnect.Comments.Add(
                                new CodeCommentStatement(sb.ToString(), true));

                            newConnect.Parameters.Add(
                                new CodeParameterDeclarationExpression(
                                    Configuration.GetInterfaceName(src as MgaObject), "src"));

                            newConnect.Parameters.Add(
                                new CodeParameterDeclarationExpression(
                                    Configuration.GetInterfaceName(dst as MgaObject), "dst"));

                            // TODO: domain specific src ref parent
                            newConnect.Parameters.Add(
                                new CodeParameterDeclarationExpression(
                                    typeof(ISIS.GME.Common.Interfaces.Reference).FullName, "srcRef = null"));

                            // TODO: domain specific dst ref parent
                            newConnect.Parameters.Add(
                                new CodeParameterDeclarationExpression(
                                    typeof(ISIS.GME.Common.Interfaces.Reference).FullName, "dstRef = null"));

                            newConnect.Parameters.Add(
                                new CodeParameterDeclarationExpression(
                                    Configuration.GetInterfaceName(parent as MgaObject), "parent = null"));

                            if (parent.MetaBase.Name != "Folder")
                            {
                                // parent is not a folder we need roles
                                Dictionary<MgaFCO, List<string>> parentRoles = GetChildRoles(parent);

                                role = parentRoles[Subject as MgaFCO].FirstOrDefault();
                                if (string.IsNullOrEmpty(role))
                                {
                                    newConnect.Parameters.Add(
                                        new CodeParameterDeclarationExpression(
                                            typeof(string), "role = \"\""));
                                }
                                else
                                {
                                    if (parentRoles[Subject as MgaFCO].Count == 1 &&
                                            role == Subject.Name)
                                    {
                                        // if there is only one role, which matches with the
                                        // object's name use the default role, which is the name
                                        newConnect.Parameters.Add(
                                            new CodeParameterDeclarationExpression(
                                                "DefaultRole",
                                                "role = DefaultRole." + Subject.Name));
                                    }
                                    else
                                    {
                                        // use parent specific role
                                        newConnect.Parameters.Add(
                                            new CodeParameterDeclarationExpression(
                                                Configuration.GetClassName(Subject) + ".Roles." + parent.Name,
                                                "role = " + Configuration.GetClassName(Subject) + ".Roles." + parent.Name + "." + role));
                                    }
                                }

                                string namespacePrefix = Configuration.GetGmeNamespaceName(Subject);

                                newConnect.Statements.Add(
                                    new CodeMethodReturnStatement(
                                        new CodeSnippetExpression(typeof(ISIS.GME.Common.Utils).FullName +
                                            ".MakeConnection<" +
                                            Configuration.GetClassName(Subject) + ">(src, dst, srcRef, dstRef, parent, \"" + namespacePrefix + "\" + role.ToString())")));
                            }
                            else
                            {
                                // parent is a folder we do not need roles
                                // the type is encoded in the name of the object
                                newConnect.Statements.Add(
                                    new CodeMethodReturnStatement(
                                        new CodeSnippetExpression(typeof(ISIS.GME.Common.Utils).FullName +
                                            ".MakeConnection<" +
                                            Configuration.GetClassName(Subject) + ">(src, dst, srcRef, dstRef, parent)")));
                            }

                            GeneratedClass.Types[0].Members.Add(newConnect);
                        }
                    }
                }
            }
        }

        void ClassCodeCreateObject()
        {
            if (Subject.MetaBase.Name != "Connection" &&
                Subject.MetaBase.Name != "FCO" &&
                Subject.MetaBase.Name != "RootFolder")
            {
                if (Subject.MetaBase.Name != "Folder")
                {
                    if ((Subject as MgaFCO).BoolAttrByName["IsAbstract"])
                    {
                        // abstract
                        // user should not create abstract objects
                        return;
                    }
                }

                // folder does not have roles
                IEnumerable<MgaFCO> parents = GetParents(Subject as MgaFCO, true).Distinct().
                    Where(x =>
                        x.MetaBase.Name == "Folder" ?
                        true :
                        x.BoolAttrByName["IsAbstract"] == false);

                // name of the current role
                string role = "";

                if (IsInRootFolder(Subject as MgaFCO))
                {
                    CodeMemberMethod newCreateObject = new CodeMemberMethod()
                    {
                        Attributes = MemberAttributes.Public | MemberAttributes.Static,
                        Name = "Create",
                        ReturnType = new CodeTypeReference(
                            Configuration.GetInterfaceName(Subject)),
                    };
                    newCreateObject.Comments.Add(
                        new CodeCommentStatement("Creates an object in the RootFolder.", true));

                    newCreateObject.Parameters.Add(
                        new CodeParameterDeclarationExpression(
                            Configuration.ProjectIntefaceNamespace + ".RootFolder", "parent"));

                    // parent is a folder we do not need roles
                    // the type is encoded in the name of the object
                    newCreateObject.Statements.Add(
                        new CodeMethodReturnStatement(
                            new CodeSnippetExpression(typeof(ISIS.GME.Common.Utils).FullName +
                                ".CreateObject<" +
                                Configuration.GetClassName(Subject) + ">(parent)")));

                    GeneratedClass.Types[0].Members.Add(newCreateObject);
                }

                foreach (var parent in parents)
                {
                    CodeMemberMethod newCreateObject = new CodeMemberMethod()
                    {
                        Attributes = MemberAttributes.Public | MemberAttributes.Static,
                        Name = "Create",
                        ReturnType = new CodeTypeReference(
                            Configuration.GetInterfaceName(Subject)),
                    };
                    newCreateObject.Comments.Add(
                        new CodeCommentStatement("Creates an object in a container.", true));

                    newCreateObject.Parameters.Add(
                        new CodeParameterDeclarationExpression(
                            Configuration.GetInterfaceName(parent as MgaObject), "parent"));

                    if (parent.MetaBase.Name != "Folder")
                    {
                        // parent is not a folder we need roles

                        Dictionary<MgaFCO, List<string>> parentRoles = GetChildRoles(parent);

                        role = parentRoles[Subject as MgaFCO].FirstOrDefault();

                        if (string.IsNullOrEmpty(role))
                        {
                            newCreateObject.Parameters.Add(
                                new CodeParameterDeclarationExpression(
                                    typeof(string), "roleStr = \"\""));
                        }
                        else
                        {
                            if (parentRoles[Subject as MgaFCO].Count == 1 &&
                                role == Subject.Name)
                            {
                                // if there is only one role, which matches with the
                                // object's name use the default role, which is the name
                                newCreateObject.Parameters.Add(
                                    new CodeParameterDeclarationExpression(
                                        "DefaultRole",
                                        "roleStr = DefaultRole." + Subject.Name));
                            }
                            else
                            {
                                // use parent specific role
                                newCreateObject.Parameters.Add(
                                    new CodeParameterDeclarationExpression(
                                        Configuration.GetClassName(Subject) + ".Roles." + parent.Name,
                                        "roleStr = " + Configuration.GetClassName(Subject) + ".Roles." + parent.Name + "." + role));
                            }
                        }

                        string namespacePrefix = Configuration.GetGmeNamespaceName(Subject);

                        newCreateObject.Statements.Add(
                            new CodeMethodReturnStatement(
                                new CodeSnippetExpression(typeof(ISIS.GME.Common.Utils).FullName +
                                    ".CreateObject<" +
                                    Configuration.GetClassName(Subject) + ">(parent, \"" + namespacePrefix + "\" + roleStr.ToString())")));
                    }
                    else
                    {
                        // parent is a folder we do not need roles
                        // the type is encoded in the name of the object
                        newCreateObject.Statements.Add(
                            new CodeMethodReturnStatement(
                                new CodeSnippetExpression(typeof(ISIS.GME.Common.Utils).FullName +
                                    ".CreateObject<" +
                                    Configuration.GetClassName(Subject) + ">(parent)")));
                    }
                    GeneratedClass.Types[0].Members.Add(newCreateObject);
                }
            }
        }
    }
}
