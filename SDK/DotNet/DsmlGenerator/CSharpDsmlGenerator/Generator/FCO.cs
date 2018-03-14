using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.CodeDom;
using GME.MGA;
using ISIS.GME.Common;
using ISIS.GME.Common.Classes;
using System.Diagnostics.Contracts;

namespace CSharpDSMLGenerator.Generator
{
    public partial class FCO : Base
    {
        public FCO(
            MgaObject subject,
            CodeTypeReferenceCollection baseTypes,
            CodeTypeReferenceCollection attributes) :
            base(subject, baseTypes, attributes)
        {
        }

        public override void GenerateClassCode()
        {

            base.GenerateClassCode();

            string ClassName = Subject.Name;

            if (Subject.MetaBase.Name == "RootFolder")
            {
                ClassName = "RootFolder";
            }

            ClassCodeGetRootFolder();

            ClassCodeArcheType();

            ClassCodeRoles();

            ClassCodeSrcDstConnections();

            ClassCodeSrcDstEnd();

            ClassCodeParentContainer();

            ClassCodeAttributes();

            ClassCodeChildren();

            ClassCodeMakeConnection();

            ClassCodeCreateObject();

            ClassCodeCast();

            ClassCodeReferred();

            ClassCodeReferencedBy();

            ClassCodeSetMembers();

            ClassCodeMemberOfSets();
        }

        private void ClassCodeCast()
        {
            if (Subject.MetaBase.Name != "RootFolder")
            {
                CodeMemberMethod newCast = new CodeMemberMethod()
                {
                    Attributes = MemberAttributes.Public | MemberAttributes.Static,
                    Name = "Cast",
                    ReturnType = new CodeTypeReference(
                        Configuration.GetInterfaceName(Subject as MgaObject)),
                };

                newCast.Comments.Add(
                    new CodeCommentStatement(@"<summary>", true));

                newCast.Comments.Add(
                    new CodeCommentStatement("Gets a domain specific object from a COM object.", true));

                newCast.Comments.Add(
                    new CodeCommentStatement(@"</summary>", true));


                newCast.Parameters.Add(
                    new CodeParameterDeclarationExpression("global::" + typeof(IMgaObject).FullName, "subject"));

                newCast.Statements.Add(
                    new CodeMethodReturnStatement(
                        new CodeSnippetExpression(
                        typeof(ISIS.GME.Common.Utils).FullName + ".CreateObject<" +
                        Configuration.GetClassName(Subject as MgaObject) + ">(subject)")));

                GeneratedClass.Types[0].Members.Add(newCast);
            }
        }

        public override void GenerateInterfaceCode()
        {
            base.GenerateInterfaceCode();


            if (Subject.MetaBase.Name == "RootFolder")
            {
                // rootfolder specific properties
                CodeMemberProperty libraryName = new CodeMemberProperty()
                {
                    Attributes = MemberAttributes.Public,
                    HasGet = true,
                    Name = "LibraryName",
                    Type = new CodeTypeReference(typeof(string)),
                };

                libraryName.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.LibraryName, true));

                GeneratedInterface.Types[0].Members.Add(libraryName);

                CodeMemberProperty library = new CodeMemberProperty()
                {
                    Attributes = MemberAttributes.Public,
                    HasGet = true,
                    Name = "LibraryCollection",
                    Type = new CodeTypeReference(
                        "global::System.Collections.Generic.IEnumerable<" + Configuration.ProjectIntefaceNamespace +
                        ".RootFolder" + ">"),
                };

                library.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.LibraryCollection, true));

                GeneratedInterface.Types[0].Members.Add(library);
            }


            InterfaceCodeSrcDstConnections();

            IntefaceCodeSrcDstEnd();

            InterfaceCodeArcheType();

            InterfaceCodeAttributes();

            InterfaceCodeChildren();

            InterfaceCodeReferred();

            InterfaceCodeReferencedBy();

            InterfaceCodeSetMembers();

            InterfaceCodeMemberOfSets();
        }
    }
}
