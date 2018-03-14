using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.CodeDom;
using GME.MGA;

namespace CSharpDSMLGenerator.Generator
{
	public partial class FCO
	{
		void ClassCodeParentContainer()
		{
			#region Parent container
			if (Subject.MetaBase.Name == "RootFolder")
			{
				CodeMemberProperty libraryName = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Final,
					HasGet = true,
					Name = "LibraryName",
					Type = new CodeTypeReference(typeof(string)),
				};

				libraryName.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.LibraryName, true));

				libraryName.GetStatements.Add(
					new CodeMethodReturnStatement(
						new CodeSnippetExpression("(Impl as global::GME.MGA.MgaFolder).LibraryName")));

				GeneratedClass.Types[0].Members.Add(libraryName);

				CodeMemberProperty library = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Final,
					HasGet = true,
					Name = "LibraryCollection",
                    Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + Configuration.ProjectIntefaceNamespace +
						".RootFolder" + ">"),
				};

				library.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.LibraryCollection, true));

				library.GetStatements.Add(
					new CodeMethodReturnStatement(
						new CodeSnippetExpression(
							typeof(ISIS.GME.Common.Utils).FullName +
							".CastMgaChildren<" +
							Configuration.ProjectClassNamespace +
							".RootFolder, global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"RootFolder\")")));

				GeneratedClass.Types[0].Members.Add(library);

				CodeMemberProperty newParentContainer = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Override,
					HasGet = true,
					Name = "ParentContainer",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.Container).FullName),
				};

				newParentContainer.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.ParentContainer, true));

				newParentContainer.GetStatements.Add(
					new CodeSnippetStatement(
						"if ((Impl as global::GME.MGA.MgaFolder).ParentFolder != null) { return "
						+ typeof(ISIS.GME.Common.Utils).FullName + ".CreateObject<" +
						Configuration.ProjectClassNamespace + ".RootFolder" +
						">((Impl as global::GME.MGA.MgaFolder).ParentFolder as global::GME.MGA.MgaObject); }"));

				newParentContainer.GetStatements.Add(
					new CodeMethodReturnStatement(
						new CodeSnippetExpression("null")));

				GeneratedClass.Types[0].Members.Add(newParentContainer);
			}
			else
			{
				var folderParents = GetParentFolders(Subject as MgaFCO).Distinct().ToList();
				List<MgaFCO> derivedFolders = new List<MgaFCO>();
				folderParents.ForEach(x => derivedFolders.AddRange(GetDerivedClasses(x)));
				folderParents.AddRange(derivedFolders.Distinct());

				var modelParents = GetParentModels(Subject as MgaFCO).Distinct().ToList();
				List<MgaFCO> derivedModels = new List<MgaFCO>();
				modelParents.ForEach(x => derivedModels.AddRange(GetDerivedClasses(x)));
				modelParents.AddRange(derivedModels.Distinct());
				modelParents = modelParents.
					Distinct().
					Where(x => x.BoolAttrByName["IsAbstract"] == false).
					ToList();

				var inrootfolder = IsInRootFolder(Subject as MgaFCO);

				CodeMemberProperty newParentContainer = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Override,
					HasGet = true,
					Name = "ParentContainer",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.Container).FullName),
				};

				StringBuilder sb = new StringBuilder();

				sb.AppendLine("<summary>");
				sb.AppendLine(Configuration.Comments.ParentContainer);

				sb.AppendLine("<para>");
				sb.AppendLine("----------------------------------------------------");
				sb.AppendLine("</para>");
				sb.AppendLine("<para>");
				sb.AppendLine("The parent could be:");
				sb.AppendLine("</para>");

				if (inrootfolder)
				{
					sb.AppendLine("<para>");
					sb.AppendFormat("- RootFolder [Folder]{0}", Environment.NewLine);
					sb.AppendLine("</para>");
				}

				foreach (var item in folderParents)
				{
					sb.AppendLine("<para>");
					sb.AppendFormat("- {0} [{1}]{2}", Configuration.GetKindName(item as MgaObject), item.MetaBase.Name, Environment.NewLine);
					sb.AppendLine("</para>");
				}

				foreach (var item in modelParents)
				{
					sb.AppendLine("<para>");
					sb.AppendFormat("- {0} [{1}]{2}", Configuration.GetKindName(item as MgaObject), item.MetaBase.Name, Environment.NewLine);
					sb.AppendLine("</para>");
				}

				sb.AppendLine("</summary>");

				newParentContainer.Comments.Add(
					new CodeCommentStatement(sb.ToString(), true));

				if (Subject.MetaBase.Name == "Folder")
				{
					newParentContainer.GetStatements.Add(
							new CodeMethodReturnStatement(
								new CodeSnippetExpression(typeof(ISIS.GME.Common.Utils).FullName + ".CreateObject<" +
									typeof(ISIS.GME.Common.Classes.Folder).FullName +
                                    ">((this.Impl as global::GME.MGA.MgaFolder).ParentFolder as global::GME.MGA.MgaObject)")));
				}
				else
				{
					newParentContainer.GetStatements.Add(
                        new CodeSnippetStatement("global::GME.MGA.MgaObject parentFolder = (this.Impl as global::GME.MGA.MgaFCO).ParentFolder as global::GME.MGA.MgaObject;"));

					if (inrootfolder)
					{
						newParentContainer.GetStatements.Add(
							new CodeSnippetStatement(
								"if (parentFolder != null && parentFolder.MetaBase.MetaRef == " + Configuration.DsmlModel.GetMetaRef("RootFolder") + "/*\"RootFolder\"*/) { return " + typeof(ISIS.GME.Common.Utils).FullName + ".CreateObject<" +
									Configuration.ProjectClassNamespace + ".RootFolder" +
									">(parentFolder); }"));
					}


					foreach (var parent in folderParents.Distinct())
					{
						newParentContainer.GetStatements.Add(
							new CodeSnippetStatement(
								"if (parentFolder != null && parentFolder.MetaBase.MetaRef == " + Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(parent as MgaObject)) + "/*\"" + Configuration.GetKindName(parent as MgaObject) + "\"*/) { return " + typeof(ISIS.GME.Common.Utils).FullName + ".CreateObject<" +
									Configuration.GetClassName(parent as MgaObject) +
									">(parentFolder); }"));
					}

					newParentContainer.GetStatements.Add(
                        new CodeSnippetStatement("global::GME.MGA.MgaObject parentModel = (this.Impl as global::GME.MGA.MgaFCO).ParentModel as global::GME.MGA.MgaObject;"));

					foreach (var parent in modelParents.Distinct())
					{
						newParentContainer.GetStatements.Add(
							new CodeSnippetStatement(
								"if (parentModel != null && parentModel.MetaBase.MetaRef == " + Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(parent as MgaObject)) + "/*\"" + Configuration.GetKindName(parent as MgaObject) + "\"*/) { return " + typeof(ISIS.GME.Common.Utils).FullName + ".CreateObject<" +
									Configuration.GetClassName(parent as MgaObject) +
									">(parentModel); }"));
					}

					newParentContainer.GetStatements.Add(
							new CodeMethodReturnStatement(
								new CodeSnippetExpression("null")));
				}


				GeneratedClass.Types[0].Members.Add(newParentContainer);
			}
			#endregion

		}

		void InterfaceCodeParentContainer()
		{
			#region Parent container
			if (Subject.MetaBase.Name == "RootFolder")
			{
				CodeMemberProperty newParentContainer = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.New,
					HasGet = true,
					Name = "ParentContainer",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.Container).FullName),
				};

				newParentContainer.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.ParentContainer, true));

				GeneratedInterface.Types[0].Members.Add(newParentContainer);
			}
			else
			{
				var folderParents = GetParentFolders(Subject as MgaFCO).Distinct().ToList();
				List<MgaFCO> derivedFolders = new List<MgaFCO>();
				folderParents.ForEach(x => derivedFolders.AddRange(GetDerivedClasses(x)));
				folderParents.AddRange(derivedFolders.Distinct());

				var modelParents = GetParentModels(Subject as MgaFCO).Distinct().ToList();
				List<MgaFCO> derivedModels = new List<MgaFCO>();
				modelParents.ForEach(x => derivedModels.AddRange(GetDerivedClasses(x)));
				modelParents.AddRange(derivedModels.Distinct());

				var inrootfolder = IsInRootFolder(Subject as MgaFCO);

				CodeMemberProperty newParentContainer = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.New,
					HasGet = true,
					Name = "ParentContainer",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.Container).FullName),
				};

				StringBuilder sb = new StringBuilder();

				sb.AppendLine("<summary>");
				sb.AppendLine(Configuration.Comments.ParentContainer);

				sb.AppendLine("<para>");
				sb.AppendLine("----------------------------------------------------");
				sb.AppendLine("</para>");
				sb.AppendLine("<para>");
				sb.AppendLine("The parent could be:");
				sb.AppendLine("</para>");

				if (inrootfolder)
				{
					sb.AppendLine("<para>");
					sb.AppendFormat("- RootFolder [Folder]{0}", Environment.NewLine);
					sb.AppendLine("</para>");
				}

				foreach (var item in folderParents)
				{
					sb.AppendLine("<para>");
					sb.AppendFormat("- {0} [{1}]{2}", item.Name, item.MetaBase.Name, Environment.NewLine);
					sb.AppendLine("</para>");
				}

				foreach (var item in modelParents)
				{
					sb.AppendLine("<para>");
					sb.AppendFormat("- {0} [{1}]{2}", item.Name, item.MetaBase.Name, Environment.NewLine);
					sb.AppendLine("</para>");
				}

				sb.AppendLine("</summary>");

				newParentContainer.Comments.Add(
					new CodeCommentStatement(sb.ToString(), true));

				GeneratedInterface.Types[0].Members.Add(newParentContainer);
			}
			#endregion
		}
	}
}
