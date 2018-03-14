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
		void ClassCodeArcheType()
		{

			string ClassName = Subject.Name;
			if (Subject.MetaBase.Name == "RootFolder")
			{
				ClassName = "RootFolder";
			}

			#region ArcheType
			if (Subject.MetaBase.Name != "RootFolder" &&
					Subject.MetaBase.Name != "Folder")
			{

				foreach (var child in baseClasses.Distinct())
				{
					CodeMemberProperty newArcheType = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Final,
						HasGet = true,
						Name = Configuration.GetInterfaceName(child as MgaObject) + ".ArcheType",
						Type = new CodeTypeReference(Configuration.GetInterfaceName(child as MgaObject)),
					};

					newArcheType.Comments.Add(
						new CodeCommentStatement(Configuration.Comments.ArcheType, true));

					newArcheType.GetStatements.Add(
							new CodeMethodReturnStatement(
                                new CodeSnippetExpression("(Impl as global::GME.MGA.MgaFCO).ArcheType == null ? null : " + typeof(ISIS.GME.Common.Utils).FullName + ".CreateObject<" +
									Configuration.GetClassName(Subject) +
									">((Impl as global::GME.MGA.MgaFCO).ArcheType as global::GME.MGA.MgaObject)")));

					GeneratedClass.Types[0].Members.Add(newArcheType);
				}
			}
			#endregion
		}

		void InterfaceCodeArcheType()
		{
			if (Subject.MetaBase.Name != "RootFolder" &&
					Subject.MetaBase.Name != "Folder")
			{
				//(Impl as global::GME.MGA.MgaFCO).ArcheType
				CodeMemberProperty newArcheType = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "ArcheType",
					Type = new CodeTypeReference(
						Configuration.GetInterfaceName(Subject)),
				};

                if (baseClassesWoObject.Count > 0)
                {
					newArcheType.Attributes =
						newArcheType.Attributes | MemberAttributes.New;
				}

				newArcheType.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.ArcheType, true));

				GeneratedInterface.Types[0].Members.Add(newArcheType);
			}
		}
	}
}
