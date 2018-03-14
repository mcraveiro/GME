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
		void ClassCodeChildren()
		{
			#region Domain Specific Children
			if (Subject.MetaBase.Name == "Model" ||
				Subject.MetaBase.Name == "Folder" ||
				Subject.MetaBase.Name == "RootFolder")
			{
				CodeTypeDeclaration newChildrenClass = new CodeTypeDeclaration("ChildrenClass")
				{
					Attributes = MemberAttributes.Public,
					IsClass = true,
					TypeAttributes = System.Reflection.TypeAttributes.NestedPublic,
				};

                newChildrenClass.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				CodeMemberField impl = new CodeMemberField("global::GME.MGA.IMgaObject", "Impl");
				newChildrenClass.Members.Add(impl);

				CodeFieldReferenceExpression implReferenceObject =
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), "Impl");

				CodeMemberField implFolder = new CodeMemberField("global::GME.MGA.MgaFolder", "ImplFolder");
				newChildrenClass.Members.Add(implFolder);

				CodeFieldReferenceExpression implReferenceFolder =
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), "ImplFolder");

				CodeMemberField implModel = new CodeMemberField("global::GME.MGA.MgaModel", "ImplModel");
				newChildrenClass.Members.Add(implModel);

				CodeFieldReferenceExpression implReferenceModel =
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), "ImplModel");


				CodeConstructor ctorFolder = new CodeConstructor();
				ctorFolder.Attributes = MemberAttributes.Public;
				ctorFolder.Parameters.Add(new CodeParameterDeclarationExpression("global::GME.MGA.MgaFolder", "impl"));

                ctorFolder.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				ctorFolder.Statements.Add(new CodeAssignStatement(implReferenceFolder,
						new CodeArgumentReferenceExpression("impl")));

				ctorFolder.Statements.Add(new CodeAssignStatement(implReferenceObject,
					new CodeCastExpression("global::GME.MGA.IMgaObject", new CodeArgumentReferenceExpression("impl"))));

				newChildrenClass.Members.Add(ctorFolder);


				CodeConstructor ctorModel = new CodeConstructor();
				ctorModel.Attributes = MemberAttributes.Public;
				ctorModel.Parameters.Add(new CodeParameterDeclarationExpression("global::GME.MGA.MgaModel", "impl"));

                ctorModel.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				ctorModel.Statements.Add(new CodeAssignStatement(implReferenceModel,
						new CodeArgumentReferenceExpression("impl")));

				ctorModel.Statements.Add(new CodeAssignStatement(implReferenceObject,
					new CodeCastExpression("global::GME.MGA.IMgaObject", new CodeArgumentReferenceExpression("impl"))));

				newChildrenClass.Members.Add(ctorModel);

				// get children
				List<MgaFCO> children = GetChildren(Subject).ToList();

				List<MgaFCO> childBases = children.ToList();
				childBases.ForEach(x => children.AddRange(GetDerivedClasses(x)));

				// skip multiple containments
                foreach (var childName in Configuration.GetUniqueNames(children, children.Count))
				{
                    var child = childName.Item1;
					// include all derived class child objects
					IEnumerable<MgaFCO> derivedClasses = GetDerivedClasses(child);

					CodeMemberProperty childrenField = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Public | MemberAttributes.Final,
						HasGet = true,
                        Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + Configuration.GetInterfaceName(child as MgaObject) + ">"),
						Name = childName.Item2 + "Collection",
					};

					StringBuilder sb = new StringBuilder();
					sb.AppendLine("<summary>");
					sb.AppendLine("<para>");
					sb.AppendLine("Retrieves with specific kinds in the container.");
					sb.AppendLine("</para>");

					var filteredDerivedClasses = derivedClasses.
						Where(x =>
							x.MetaBase.Name == "Folder" ?
							true :
							x.BoolAttrByName["IsAbstract"] == false).Distinct();

					if (filteredDerivedClasses.Count() > 0)
					{
						sb.AppendLine("<para>");
						sb.AppendLine("----------------------------------------------------");
						sb.AppendLine("</para>");
						sb.AppendLine("<para>");
						sb.AppendLine("Result will contain the kinds as follows.");
						sb.AppendLine("</para>");
						if (
							child.MetaBase.Name == "Folder" ?
							true :
							child.BoolAttrByName["IsAbstract"] == false)
						{
							sb.AppendLine("<para>");
							sb.AppendFormat("- {0}{1}", child.Name, Environment.NewLine);
							sb.AppendLine("</para>");
						}
						foreach (var item in filteredDerivedClasses)
						{
							sb.AppendLine("<para>");
							sb.AppendFormat("- {0}{1}", item.Name, Environment.NewLine);
							sb.AppendLine("</para>");
						}
					}
					sb.AppendLine("</summary>");

					childrenField.Comments.Add(new CodeCommentStatement(sb.ToString(), true));


					string codeDef;
					if (Configuration.DsmlModel.GetMetaRef(
						Configuration.GetKindName(child as MgaObject)) != 0)
					{
						//codeDef = String.Format(
						//  "global::System.Collections.Generic.IEnumerable<{3}.{0}> result = {2}.CastMgaChildren<{1}.{0}, global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"{0}\" /* {4} */);",
						//  child.Name,
						//  Configuration.ProjectClassNamespace,
						//  typeof(ISIS.GME.Common.Utils).FullName,
						//  Configuration.ProjectIntefaceNamespace,
						//  Configuration.LocalFactory[child.Name]);

						codeDef = String.Format(
                            "global::System.Collections.Generic.IEnumerable<{3}> result = {2}.CastMgaChildren<{1}, global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"{0}\" /* {4} */);",
							Configuration.GetKindName(child as MgaObject),
							Configuration.GetClassName(child as MgaObject),
							typeof(ISIS.GME.Common.Utils).FullName,
							Configuration.GetInterfaceName(child as MgaObject),
							Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(child as MgaObject)));
					}
					else
					{
						// base classes which are abstract or FCO-s
						// we need a MetaRef list...
						//codeDef = String.Format(
						//  "global::System.Collections.Generic.IEnumerable<{3}.{0}> result = {2}.CastMgaChildren<{1}.{0}, global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"{0}\");",
						//  child.Name,
						//  Configuration.ProjectClassNamespace,
						//  typeof(ISIS.GME.Common.Utils).FullName,
						//  Configuration.ProjectIntefaceNamespace);
						codeDef = String.Format(
                            "global::System.Collections.Generic.IEnumerable<{3}> result = {2}.CastMgaChildren<{1}, global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"{0}\");",
							Configuration.GetKindName(child as MgaObject),
							Configuration.GetClassName(child as MgaObject),
							typeof(ISIS.GME.Common.Utils).FullName,
							Configuration.GetInterfaceName(child as MgaObject));
					}

					CodeSnippetStatement codeDefinition = new CodeSnippetStatement(codeDef);
					childrenField.GetStatements.Add(codeDefinition);


					foreach (var derived in derivedClasses)
					{
						if (Configuration.DsmlModel.GetMetaRef(
							Configuration.GetKindName(derived as MgaObject)) != 0)
						{
							//codeDef = String.Format(
							//"result = result.Concat({2}.CastMgaChildren<{1}.{0}, global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"{0}\" /* {3} */));",
							//derived.Name,
							//Configuration.ProjectClassNamespace,
							//typeof(ISIS.GME.Common.Utils).FullName,
							//Configuration.LocalFactory[derived.Name]);
							codeDef = String.Format(
								"result = result.Concat({2}.CastMgaChildren<{1}, global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"{0}\" /* {3} */));",
								Configuration.GetKindName(derived as MgaObject),
								Configuration.GetClassName(derived as MgaObject),
								typeof(ISIS.GME.Common.Utils).FullName,
								Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(derived as MgaObject)));
						}
						else
						{
							//codeDef = String.Format(
							//"result = result.Concat({2}.CastMgaChildren<{1}.{0}, global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"{0}\"));",
							//derived.Name,
							//Configuration.ProjectClassNamespace,
							//typeof(ISIS.GME.Common.Utils).FullName);
							codeDef = String.Format(
								"result = result.Concat({2}.CastMgaChildren<{1}, global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"{0}\"));",
								Configuration.GetKindName(derived as MgaObject),
								Configuration.GetClassName(derived as MgaObject),
								typeof(ISIS.GME.Common.Utils).FullName);
						}
						CodeSnippetStatement codeDerived = new CodeSnippetStatement(codeDef);
						childrenField.GetStatements.Add(codeDerived);
					}

					// TODO: this is not a good way to define...
					CodeSnippetStatement codeReturn = new CodeSnippetStatement("return result;");
					childrenField.GetStatements.Add(codeReturn);

					newChildrenClass.Members.Add(childrenField);
				}

				GeneratedClass.Types[0].Members.Add(newChildrenClass);

				if (Subject.MetaBase.Name == "RootFolder")
				{
					CodeMemberProperty newChildrenProperty = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Final,
						HasGet = true,
						Name = Configuration.ProjectIntefaceNamespace + ".RootFolder.Children",
						Type = new CodeTypeReference("RootFolder.ChildrenClass"),
					};

					newChildrenProperty.Comments.Add(
						new CodeCommentStatement(Configuration.Comments.Children, true));

					if (Subject.MetaBase.Name == "Model")
					{
						newChildrenProperty.GetStatements.Add(
							new CodeMethodReturnStatement(
								new CodeObjectCreateExpression("RootFolder.ChildrenClass",
									new CodeCastExpression("global::GME.MGA.MgaModel",
										new CodeFieldReferenceExpression(
											new CodeThisReferenceExpression(),
											"Impl")))));
					}
					else if (Subject.MetaBase.Name == "Folder" ||
						Subject.MetaBase.Name == "RootFolder")
					{
						newChildrenProperty.GetStatements.Add(
							new CodeMethodReturnStatement(
								new CodeObjectCreateExpression("RootFolder.ChildrenClass",
									new CodeCastExpression("global::GME.MGA.MgaFolder",
										new CodeFieldReferenceExpression(
											new CodeThisReferenceExpression(),
											"Impl")))));
					}
					else
					{
						throw new Exception("Type cannot contain child objects.");
					}

					GeneratedClass.Types[0].Members.Add(newChildrenProperty);

					CodeMemberProperty newAllChildrenProperty = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Public | MemberAttributes.Override,
						HasGet = true,
						Name = "AllChildren",
                        Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Base).FullName + ">"),
					};

					newAllChildrenProperty.Comments.Add(
						new CodeCommentStatement(Configuration.Comments.AllChildren, true));

					//var allChildren = children.
					//  Where(x => (x.MetaBase.Name == "Model" || x.MetaBase.Name == "Folder") && GetDerivedClasses(x).Count() == 0);
					var allChildren = children.
						Where(x =>
							x.MetaBase.Name == "Folder" ?
							true :
							x.BoolAttrByName["IsAbstract"] == false).Distinct();

					newAllChildrenProperty.GetStatements.Add(
							new CodeSnippetExpression(
                                "global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Base).FullName + "> result = " +
								typeof(ISIS.GME.Common.Utils).FullName + ".CastMgaChildren<" +
								Configuration.ProjectClassNamespace + ".RootFolder" +
								", global::GME.MGA.MgaObjects>(Impl.ChildObjects, \"RootFolder\")"));

					foreach (var childFco in allChildren)
					{
						newAllChildrenProperty.GetStatements.Add(
							new CodeSnippetExpression(
								"result = result.Concat(" +
								typeof(ISIS.GME.Common.Utils).FullName + ".CastMgaChildren<" +
								Configuration.GetClassName(childFco as MgaObject) +
								", global::GME.MGA.MgaObjects>(Impl.ChildObjects, " +
										Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(childFco as MgaObject)) + "/*\"" +
										Configuration.GetKindName(childFco as MgaObject) + "\"*/))"));
					}

					newAllChildrenProperty.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("result")));

					GeneratedClass.Types[0].Members.Add(newAllChildrenProperty);

				}
				else if (Subject.MetaBase.Name == "Model" ||
					Subject.MetaBase.Name == "Folder")
				{
					// not rootfolder
					foreach (MgaFCO fco in baseClasses.Where(x => x.MetaBase.Name == "Model" || x.MetaBase.Name == "Folder"))
					{
						CodeMemberProperty newChildrenProperty = new CodeMemberProperty()
						{
							Attributes = MemberAttributes.Final,
							HasGet = true,
							Name = Configuration.GetInterfaceName(fco as MgaObject) + ".Children",
							Type = new CodeTypeReference(Configuration.GetClassName(fco as MgaObject) + ".ChildrenClass"),
						};

						newChildrenProperty.Comments.Add(
							new CodeCommentStatement(Configuration.Comments.Children, true));

						if (Subject.MetaBase.Name == "Model")
						{
							newChildrenProperty.GetStatements.Add(
								new CodeMethodReturnStatement(
									new CodeObjectCreateExpression(Configuration.GetClassName(fco as MgaObject) + ".ChildrenClass",
										new CodeCastExpression("global::GME.MGA.MgaModel",
											new CodeFieldReferenceExpression(
												new CodeThisReferenceExpression(),
												"Impl")))));
						}
						else if (Subject.MetaBase.Name == "Folder" ||
							Subject.MetaBase.Name == "RootFolder")
						{
							newChildrenProperty.GetStatements.Add(
								new CodeMethodReturnStatement(
									new CodeObjectCreateExpression(Configuration.GetClassName(fco as MgaObject) + ".ChildrenClass",
										new CodeCastExpression("global::GME.MGA.MgaFolder",
											new CodeFieldReferenceExpression(
												new CodeThisReferenceExpression(),
												"Impl")))));
						}
						else
						{
							throw new Exception("Type cannot contain child objects.");
						}

						GeneratedClass.Types[0].Members.Add(newChildrenProperty);

					}
					CodeMemberProperty newAllChildrenProperty = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Public | MemberAttributes.Override,
						HasGet = true,
						Name = "AllChildren",
						Type = new CodeTypeReference(
							typeof(System.Collections.Generic.IEnumerable<>).FullName,
							CodeTypeReferenceOptions.GenericTypeParameter | CodeTypeReferenceOptions.GlobalReference),

					};

					newAllChildrenProperty.Type.TypeArguments.Add(
						new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.Base).FullName,
							CodeTypeReferenceOptions.GlobalReference));

					newAllChildrenProperty.Comments.Add(
						new CodeCommentStatement(Configuration.Comments.AllChildren, true));

					if (Subject.MetaBase.Name == "Model" ||
						Subject.MetaBase.Name == "Folder")
					{
						// select all folders and those children, which are not abstract
						var allChildren = children.
							Where(x =>
								x.MetaBase.Name == "Folder" ?
								true :
								x.BoolAttrByName["IsAbstract"] == false).Distinct();

						if (allChildren.Count() == 0)
						{
							newAllChildrenProperty.GetStatements.Add(
								new CodeMethodReturnStatement(new CodeSnippetExpression("new ISIS.GME.Common.Interfaces.Base[] { }")));
						}
						else
						{
							//  newAllChildrenProperty.GetStatements.Add(
							//    new CodeSnippetExpression(
							//      "global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Base).FullName +
							//      "> result = " +
							//      typeof(ISIS.GME.Common.Utils).FullName + ".CastMgaChildren<" +
							//      Configuration.GetClassName(allChildren.FirstOrDefault() as MgaObject) +
							//      ", global::GME.MGA.MgaObjects>(Impl.ChildObjects, " +
							//        Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(allChildren.FirstOrDefault() as MgaObject)) + "/*\"" +
							//        Configuration.GetKindName(allChildren.FirstOrDefault() as MgaObject) + "\"*/)"));

							//  //newAllChildrenProperty.GetStatements.Add(
							//  //  new CodeSnippetExpression(
							//  //    "List<" + typeof(ISIS.GME.Common.Interfaces.Base).FullName +
							//  //    "> result = " +
							//  //    typeof(ISIS.GME.Common.Utils).FullName + ".CastMgaChildren<" +
							//  //    Configuration.GetClassName(allChildren.FirstOrDefault() as MgaObject) +
							//  //    ", global::GME.MGA.MgaObjects>(Impl.ChildObjects, " +
							//  //      Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(allChildren.FirstOrDefault() as MgaObject)) + "/*\"" +
							//  //      Configuration.GetKindName(allChildren.FirstOrDefault() as MgaObject) + "\"*/).Cast<ISIS.GME.Common.Interfaces.Base>().ToList()"));

							//  foreach (var childFco in allChildren.Skip(1))
							//  {
							//    newAllChildrenProperty.GetStatements.Add(
							//      new CodeSnippetExpression(
							//        "result = result.Concat(" +
							//        typeof(ISIS.GME.Common.Utils).FullName + ".CastMgaChildren<" +
							//        Configuration.GetClassName(childFco as MgaObject) +
							//        ", global::GME.MGA.MgaObjects>(Impl.ChildObjects, " +
							//        Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(childFco as MgaObject)) + "/*\"" +
							//        Configuration.GetKindName(childFco as MgaObject) + "\"*/))"));
							//    //newAllChildrenProperty.GetStatements.Add(
							//    //  new CodeSnippetExpression(
							//    //    "result.AddRange(" +
							//    //    typeof(ISIS.GME.Common.Utils).FullName + ".CastMgaChildren<" +
							//    //    Configuration.GetClassName(childFco as MgaObject) +
							//    //    ", global::GME.MGA.MgaObjects>(Impl.ChildObjects, " +
							//    //    Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(childFco as MgaObject)) + "/*\"" +
							//    //    Configuration.GetKindName(childFco as MgaObject) + "\"*/).Cast<ISIS.GME.Common.Interfaces.Base>())"));

							//  }
							StringBuilder sb = new StringBuilder();
                            sb.AppendLine("global::System.Collections.Generic.IEnumerable<ISIS.GME.Common.Interfaces.Base> result = ISIS.GME.Common.Utils.CastMgaChildren(Impl.ChildObjects, new global::System.Collections.Generic.Dictionary<int, global::System.Type>() { ");
							foreach (var item in allChildren)
							{
								sb.Append("{ ");
								sb.Append(Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(item as MgaObject)));
								sb.Append(" /*");
								sb.Append(Configuration.GetKindName(item as MgaObject));
								sb.Append("*/ ");
								sb.Append(", typeof(");
								sb.Append(Configuration.GetClassName(item as MgaObject));
								sb.Append(" ) },");
								sb.AppendLine();
							}
							sb.AppendLine("});");

							newAllChildrenProperty.GetStatements.Add(
								new CodeExpressionStatement(new CodeSnippetExpression(sb.ToString())));

							//ISIS.GME.Common.Utils.CastMgaChildren(Impl.ChildObjects, new Dictionary<int, System.Type>() { { 34, typeof(ISIS.GME.Dsml.CyPhyML.Classes.WorkBreakdownStructures) }, });

							newAllChildrenProperty.GetStatements.Add(
								new CodeMethodReturnStatement(new CodeSnippetExpression("result")));
						}
					}
					else
					{
						throw new Exception("Type cannot contain child objects.");
					}

					GeneratedClass.Types[0].Members.Add(newAllChildrenProperty);
				}
			}
			#endregion
		}

		void InterfaceCodeChildren()
		{
			#region Domain Specific Children
			if (Subject.MetaBase.Name == "Model" ||
					Subject.MetaBase.Name == "Folder" ||
					Subject.MetaBase.Name == "RootFolder")
			{
				CodeMemberProperty newChildrenProperty = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "Children",
					Type = new CodeTypeReference("Classes." + ClassName + ".ChildrenClass"),
				};

				newChildrenProperty.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.Children, true));

                if (baseClassesWoObject.Any(x => x.MetaBase.MetaRef == Subject.MetaBase.MetaRef))
                {
                    newChildrenProperty.Attributes =
                        newChildrenProperty.Attributes | MemberAttributes.New;
                }

				GeneratedInterface.Types[0].Members.Add(newChildrenProperty);

				CodeMemberProperty newAllChildrenProperty = new CodeMemberProperty()
				{
                    Attributes = MemberAttributes.Public | MemberAttributes.New,
					HasGet = true,
					Name = "AllChildren",
					Type = new CodeTypeReference(
                        "global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Base).FullName + ">"),
				};

				newAllChildrenProperty.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.AllChildren, true));

				GeneratedInterface.Types[0].Members.Add(newAllChildrenProperty);

			}
			#endregion
		}
	}
}
