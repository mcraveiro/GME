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
		void ClassCodeRoles()
		{
			#region Roles

			if (Subject.MetaBase.Name == "Connection" ||
				Subject.MetaBase.Name == "Atom" ||
				Subject.MetaBase.Name == "Set" ||
				Subject.MetaBase.Name == "Reference" ||
				Subject.MetaBase.Name == "Model")
			{
				if ((Subject as MgaFCO).BoolAttrByName["IsAbstract"])
				{
					// do not generate roles for abstract types
					return;
				}

				CodeTypeDeclaration newDefaultRole = new CodeTypeDeclaration()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Final,
					IsEnum = true,
					Name = "DefaultRole",
				};
				newDefaultRole.Comments.Add(
						new CodeCommentStatement("Default role", true));

				CodeMemberField codeMemberFieldDefault = new CodeMemberField()
				{
					Name = Subject.Name,
				};

				codeMemberFieldDefault.Comments.Add(new CodeCommentStatement(@"<summary>", true));
				codeMemberFieldDefault.Comments.Add(new CodeCommentStatement("Default role", true));
				codeMemberFieldDefault.Comments.Add(new CodeCommentStatement(@"</summary>", true));

				newDefaultRole.Members.Add(codeMemberFieldDefault);
				GeneratedClass.Types[0].Members.Add(newDefaultRole);

				CodeTypeDeclaration newRoles = new CodeTypeDeclaration()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Final,
					IsStruct = true,
					Name = "Roles",
				};

				newRoles.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.Roles, true));

				IEnumerable<MgaFCO> parents = GetParentModels(Subject as MgaFCO).Distinct();
				//parents = parents.Where(x => x.BoolAttrByName["IsAbstract"] == false);

				foreach (var parent in parents)
				{
					Dictionary<MgaFCO, List<string>> roles = GetChildRoles(parent);

					CodeTypeDeclaration newParentRoles = new CodeTypeDeclaration()
					{
						Attributes = MemberAttributes.Public | MemberAttributes.Final,
						IsEnum = true,
						Name = parent.Name,
					};

					newParentRoles.Comments.Add(
						new CodeCommentStatement("Roles for " + parent.Name + " parent.", true));

					foreach (var role in roles[Subject as MgaFCO].Distinct())
					{
						CodeMemberField codeMemberField = new CodeMemberField()
						{
							Name = role,
						};

						codeMemberField.Comments.Add(new CodeCommentStatement(@"<summary>", true));
						codeMemberField.Comments.Add(new CodeCommentStatement(role, true));
						codeMemberField.Comments.Add(new CodeCommentStatement(@"</summary>", true));

						int roleMetaRef = Configuration.DsmlModel.GetChildRoleRef(
							Configuration.GetKindName(parent as MgaObject),
							Configuration.GetKindName(Subject),
                            role);

						codeMemberField.InitExpression = new CodePrimitiveExpression(roleMetaRef);

						//codeMemberField.InitExpression = new CodePrimitiveExpression(idx);
						newParentRoles.Members.Add(codeMemberField);
					}
					newRoles.Members.Add(newParentRoles);
				}
				GeneratedClass.Types[0].Members.Add(newRoles);
			}
			#endregion
		}
	}
}
