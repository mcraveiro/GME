using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.CodeDom;
using GME.MGA;
using ISIS.GME.Common;

namespace CSharpDSMLGenerator.Generator
{
	public partial class FCO
	{
        public static string ConvertToDOSLineEnding(string input)
        {
            return input.Replace("\r\n", "\n").Replace("\n", "\r\n");
        }

		void ClassCodeAttributes()
		{
			#region Domain Specific Attributes
			if (Subject.MetaBase.Name != "Folder" &&
				Subject.MetaBase.Name != "RootFolder")
			{
				CodeTypeDeclaration newAttrClass = new CodeTypeDeclaration("AttributesClass")
				{
					Attributes = MemberAttributes.Public,
					IsClass = true,
					TypeAttributes = System.Reflection.TypeAttributes.NestedPublic,
				};

                newAttrClass.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				CodeMemberField impl = new CodeMemberField("global::GME.MGA.IMgaObject", "Impl");
				newAttrClass.Members.Add(impl);

				CodeConstructor ctor = new CodeConstructor();
				ctor.Attributes = MemberAttributes.Public;
				ctor.Parameters.Add(new CodeParameterDeclarationExpression("global::GME.MGA.IMgaObject", "impl"));

                ctor.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				CodeFieldReferenceExpression implReference =
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), "Impl");

				ctor.Statements.Add(new CodeAssignStatement(implReference,
						new CodeArgumentReferenceExpression("impl")));

				newAttrClass.Members.Add(ctor);


				// get attributes
				List<MgaFCO> attrs = new List<MgaFCO>();
				List<MgaFCO> allFcosWProxies = new List<MgaFCO>();
				foreach (MgaFCO fco in baseClasses)
				{
					allFcosWProxies.Add(fco);
					if (fco.ReferencedBy != null)
					{
						allFcosWProxies.AddRange(fco.ReferencedBy.Cast<MgaFCO>());
					}
				}

				foreach (MgaFCO baseFco in allFcosWProxies)
				{
					foreach (MgaConnPoint cp in baseFco.PartOfConns)
					{
						MgaSimpleConnection conn = cp.Owner as MgaSimpleConnection;
						if (conn.Meta.Name == "HasAttribute")
						{
							attrs.Add(conn.Src);
						}
					}
				}

				// create attributes
				// skip multiple containments
				foreach (MgaFCO attribute in attrs.Distinct())
				{
					// TODO: get/set
					if (attribute.Meta.Name == "BooleanAttribute")
					{
						CodeMemberProperty attr = new CodeMemberProperty()
						{
							Attributes = MemberAttributes.Public | MemberAttributes.Final,
							HasGet = true,
							HasSet = true,
							Name = attribute.Name,
							Type = new CodeTypeReference(typeof(bool)),
						};

						attr.Comments.Add(
							new CodeCommentStatement(@"<summary>", true));

						StringBuilder sb = new StringBuilder();
						sb.Append(@"<para>");
						sb.AppendFormat("{0} attribute", attribute.Name);
						sb.Append(@"</para>");
						attr.Comments.Add(
							new CodeCommentStatement(sb.ToString(), true));

						string help = attribute.RegistryValue["help"];
						if (string.IsNullOrEmpty(help) == false)
						{
							sb.Clear();
							sb.Append(@"<para>");
							sb.AppendFormat("{0}", help);
							sb.Append(@"</para>");
							attr.Comments.Add(
								new CodeCommentStatement(sb.ToString(), true));
						}

						string description = attribute.RegistryValue["description"];
						if (string.IsNullOrEmpty(description) == false)
						{
							sb.Clear();
							sb.Append(@"<para>");
							sb.Append(ConvertToDOSLineEnding(description));
							sb.Append(@"</para>");
							attr.Comments.Add(
								new CodeCommentStatement(sb.ToString(), true));
						}

						attr.Comments.Add(
							new CodeCommentStatement(@"</summary>", true));

						// create the get statement
						attr.GetStatements.Add(
							new CodeMethodReturnStatement(
								new CodeMethodReferenceExpression(
									new CodeCastExpression(
										"global::GME.MGA.MgaFCO",
										new CodeVariableReferenceExpression("Impl")),
									"BoolAttrByName[\"" + attribute.Name + "\"]")));

						// create the set statement
						attr.SetStatements.Add(
							new CodeAssignStatement(
								new CodeMethodReferenceExpression(
									new CodeCastExpression(
										"global::GME.MGA.MgaFCO",
										new CodeVariableReferenceExpression("Impl")),
									"BoolAttrByName[\"" + attribute.Name + "\"]"),
									new CodePropertySetValueReferenceExpression()));

						newAttrClass.Members.Add(attr);
					}
					else if (attribute.Meta.Name == "EnumAttribute")
					{
						// add enum type
						CodeTypeDeclaration enumAttr = new CodeTypeDeclaration(attribute.Name + "_enum")
							{
								Attributes = MemberAttributes.Public,
								IsEnum = true,
							};

                        enumAttr.Comments.Add(
                            new CodeCommentStatement(Configuration.Comments.Empty, true));

						List<string> enumKeys = new List<string>();

						// TODO: default item???
						int idx = 0;
						foreach (var key in attribute.StrAttrByName["MenuItems"].Split('\n'))
						{
							if (String.IsNullOrEmpty(key))
							{
								// skip if it is empty
								continue;
							}

							int i;
							string prefix = "";
							if (int.TryParse(key[0].ToString(), out i))
							{
								prefix = Configuration.Generator.EnumPrefix;
							}


							var codeProperty = new CodeMemberField
							{
								Name = prefix + key.Split(',').FirstOrDefault().ReplaceNames(),
							};

							codeProperty.Comments.Add(new CodeCommentStatement(@"<summary>", true));
                            codeProperty.Comments.Add(new CodeCommentStatement(new System.Xml.Linq.XText(key).ToString(), true));
							codeProperty.Comments.Add(new CodeCommentStatement(@"</summary>", true));

							if (enumAttr.Members.Cast<CodeMemberField>().Any(x => x.Name == codeProperty.Name))
							{
								// the enum already contains this key
								StringBuilder sbError = new StringBuilder();
								sbError.AppendLine("Enum attribute names must be unique.");

								sbError.AppendFormat(
									"Item: {0} attibute name: {1}{2}",
									attribute.Name,
									codeProperty.Name,
									Environment.NewLine);

								sbError.AppendFormat(
									"Object path: {0}",
									attribute.AbsPath);

								throw new Exception(sbError.ToString());
							}
							else
							{
								// put in the enum because it is unique
								codeProperty.InitExpression = new CodePrimitiveExpression(idx);
								enumAttr.Members.Add(codeProperty);
							}
							idx++;
						}

						newAttrClass.Members.Add(enumAttr);


						CodeMemberProperty attr = new CodeMemberProperty()
						{
							Attributes = MemberAttributes.Public | MemberAttributes.Final,
							HasGet = true,
							HasSet = true,
							Name = attribute.Name,
							Type = new CodeTypeReference(attribute.Name + "_enum"),
						};

						attr.Comments.Add(
	new CodeCommentStatement(@"<summary>", true));

						StringBuilder sb = new StringBuilder();
						sb.Append(@"<para>");
						sb.AppendFormat("{0} attribute", attribute.Name);
						sb.Append(@"</para>");
						attr.Comments.Add(
							new CodeCommentStatement(sb.ToString(), true));

						string help = attribute.StrAttrByName["Help"];
						if (string.IsNullOrEmpty(help) == false)
						{
							sb.Clear();
							sb.Append(@"<para>");
							sb.AppendFormat("{0}", help);
							sb.Append(@"</para>");
							attr.Comments.Add(
								new CodeCommentStatement(sb.ToString(), true));
						}

						string description = attribute.RegistryValue["description"];
						if (string.IsNullOrEmpty(description) == false)
						{
							sb.Clear();
							sb.Append(@"<para>");
							sb.Append(ConvertToDOSLineEnding(description));
							sb.Append(@"</para>");
							attr.Comments.Add(
								new CodeCommentStatement(sb.ToString(), true));
						}

						attr.Comments.Add(
							new CodeCommentStatement(@"</summary>", true));

						// create the get statement
						attr.GetStatements.Add(
							new CodeSnippetExpression("global::System.Array a = global::System.Enum.GetValues(typeof(" +
								Configuration.GetClassName(Subject) + ".AttributesClass." + attribute.Name  + "_enum))"));

						attr.GetStatements.Add(
							new CodeSnippetExpression(
								"int number = " + 
								typeof(Utils).FullName + 
								".GetEnumItemNumber(Impl as global::GME.MGA.MgaFCO, \"" + 
								attribute.Name + "\")"));

						attr.GetStatements.Add(
							new CodeSnippetExpression("return (" +
								Configuration.GetClassName(Subject) + ".AttributesClass." + attribute.Name + "_enum)a.GetValue(number)"));

						// create the set statement
						attr.SetStatements.Add(
							new CodeSnippetExpression(typeof(Utils).FullName + ".SetEnumItem(Impl as global::GME.MGA.MgaFCO, \"" + 
								attribute.Name + "\", (int)value);"));

						newAttrClass.Members.Add(attr);
					}
					else if (attribute.Meta.Name == "FieldAttribute")
					{
						if (attribute.StrAttrByName["DataType"] == "integer")
						{
							CodeMemberProperty attr = new CodeMemberProperty()
							{
								Attributes = MemberAttributes.Public | MemberAttributes.Final,
								HasGet = true,
								HasSet = true,
								Name = attribute.Name,
								Type = new CodeTypeReference(typeof(int)),
							};

							attr.Comments.Add(
								new CodeCommentStatement(@"<summary>", true));

							StringBuilder sb = new StringBuilder();
							sb.Append(@"<para>");
							sb.AppendFormat("{0} attribute", attribute.Name);
							sb.Append(@"</para>");
							attr.Comments.Add(
								new CodeCommentStatement(sb.ToString(), true));

							string help = attribute.StrAttrByName["Help"];
							if (string.IsNullOrEmpty(help) == false)
							{
								sb.Clear();
								sb.Append(@"<para>");
								sb.AppendFormat("{0}", help);
								sb.Append(@"</para>");
								attr.Comments.Add(
									new CodeCommentStatement(sb.ToString(), true));
							}

							string description = attribute.RegistryValue["description"];
							if (string.IsNullOrEmpty(description) == false)
							{
								sb.Clear();
								sb.Append(@"<para>");
								sb.Append(ConvertToDOSLineEnding(description));
								sb.Append(@"</para>");
								attr.Comments.Add(
									new CodeCommentStatement(sb.ToString(), true));
							}

							attr.Comments.Add(
								new CodeCommentStatement(@"</summary>", true));

							// create the get statement
							attr.GetStatements.Add(
								new CodeMethodReturnStatement(
									new CodeMethodReferenceExpression(
										new CodeCastExpression(
											"global::GME.MGA.MgaFCO",
											new CodeVariableReferenceExpression("Impl")),
										"IntAttrByName[\"" + attribute.Name + "\"]")));

							// create the set statement
							attr.SetStatements.Add(
								new CodeAssignStatement(
									new CodeMethodReferenceExpression(
										new CodeCastExpression(
											"global::GME.MGA.MgaFCO",
											new CodeVariableReferenceExpression("Impl")),
										"IntAttrByName[\"" + attribute.Name + "\"]"),
										new CodePropertySetValueReferenceExpression()));

							newAttrClass.Members.Add(attr);
						}
						else if (attribute.StrAttrByName["DataType"] == "string")
						{
							CodeMemberProperty attr = new CodeMemberProperty()
							{
								Attributes = MemberAttributes.Public | MemberAttributes.Final,
								HasGet = true,
								HasSet = true,
								Name = attribute.Name,
								Type = new CodeTypeReference(typeof(string)),
							};

							attr.Comments.Add(
								new CodeCommentStatement(@"<summary>", true));

							StringBuilder sb = new StringBuilder();
							sb.Append(@"<para>");
							sb.AppendFormat("{0} attribute", attribute.Name);
							sb.Append(@"</para>");
							attr.Comments.Add(
								new CodeCommentStatement(sb.ToString(), true));

							string help = attribute.StrAttrByName["Help"];
							if (string.IsNullOrEmpty(help) == false)
							{
								sb.Clear();
								sb.Append(@"<para>");
								sb.AppendFormat("{0}", help);
								sb.Append(@"</para>");
								attr.Comments.Add(
									new CodeCommentStatement(sb.ToString(), true));
							}

							string description = attribute.RegistryValue["description"];
							if (string.IsNullOrEmpty(description) == false)
							{
								sb.Clear();
								sb.Append(@"<para>");
								sb.Append(ConvertToDOSLineEnding(description));
								sb.Append(@"</para>");
								attr.Comments.Add(
									new CodeCommentStatement(sb.ToString(), true));
							}

							attr.Comments.Add(
								new CodeCommentStatement(@"</summary>", true));

							// create the get statement
							attr.GetStatements.Add(
								new CodeMethodReturnStatement(
									new CodeMethodReferenceExpression(
										new CodeCastExpression(
											"global::GME.MGA.MgaFCO",
											new CodeVariableReferenceExpression("Impl")),
										"StrAttrByName[\"" + attribute.Name + "\"]")));

							// create the set statement
							attr.SetStatements.Add(
								new CodeAssignStatement(
									new CodeMethodReferenceExpression(
										new CodeCastExpression(
											"global::GME.MGA.MgaFCO",
											new CodeVariableReferenceExpression("Impl")),
										"StrAttrByName[\"" + attribute.Name + "\"]"),
										new CodePropertySetValueReferenceExpression()));

							newAttrClass.Members.Add(attr);
						}
						else if (attribute.StrAttrByName["DataType"] == "double")
						{
							CodeMemberProperty attr = new CodeMemberProperty()
							{
								Attributes = MemberAttributes.Public | MemberAttributes.Final,
								HasGet = true,
								HasSet = true,
								Name = attribute.Name,
								Type = new CodeTypeReference(typeof(double)),
							};

							// TODO: create a function for these summaries
							attr.Comments.Add(
								new CodeCommentStatement(@"<summary>", true));

							StringBuilder sb = new StringBuilder();
							sb.Append(@"<para>");
							sb.AppendFormat("{0} attribute", attribute.Name);
							sb.Append(@"</para>");
							attr.Comments.Add(
								new CodeCommentStatement(sb.ToString(), true));

							string help = attribute.StrAttrByName["Help"];
							if (string.IsNullOrEmpty(help) == false)
							{
								sb.Clear();
								sb.Append(@"<para>");
								sb.AppendFormat("{0}", help);
								sb.Append(@"</para>");
								attr.Comments.Add(
									new CodeCommentStatement(sb.ToString(), true));
							}

							string description = attribute.RegistryValue["description"];
							if (string.IsNullOrEmpty(description) == false)
							{
								sb.Clear();
								sb.Append(@"<para>");
								sb.Append(ConvertToDOSLineEnding(description));
								sb.Append(@"</para>");
								attr.Comments.Add(
									new CodeCommentStatement(sb.ToString(), true));
							}

							attr.Comments.Add(
								new CodeCommentStatement(@"</summary>", true));

							// create the get statement
							attr.GetStatements.Add(
								new CodeMethodReturnStatement(
									new CodeMethodReferenceExpression(
										new CodeCastExpression(
											"global::GME.MGA.MgaFCO",
											new CodeVariableReferenceExpression("Impl")),
										"FloatAttrByName[\"" + attribute.Name + "\"]")));

							// create the set statement
							attr.SetStatements.Add(
								new CodeAssignStatement(
									new CodeMethodReferenceExpression(
										new CodeCastExpression(
											"global::GME.MGA.MgaFCO",
											new CodeVariableReferenceExpression("Impl")),
										"FloatAttrByName[\"" + attribute.Name + "\"]"),
										new CodePropertySetValueReferenceExpression()));

							newAttrClass.Members.Add(attr);
						}
					}
					else
					{
						throw new Exception(
							String.Format("Unrecognized attribute type: {0}", attribute.Meta.Name));
					}
				}

				GeneratedClass.Types[0].Members.Add(newAttrClass);

				foreach (MgaFCO fco in baseClasses)
				{
					CodeMemberProperty newAttrProperty = new CodeMemberProperty()
						{
							Attributes = MemberAttributes.Final,
							HasGet = true,
							Name = Configuration.GetInterfaceName(fco as MgaObject) + ".Attributes",
							Type = new CodeTypeReference(Configuration.GetClassName(fco as MgaObject) + "." + newAttrClass.Name),
						};

					newAttrProperty.Comments.Add(
						new CodeCommentStatement(Configuration.Comments.Attributes, true));

					newAttrProperty.GetStatements.Add(
						new CodeMethodReturnStatement(
							new CodeObjectCreateExpression(Configuration.GetClassName(fco as MgaObject) + "." + newAttrClass.Name,
								new CodeFieldReferenceExpression(
									new CodeThisReferenceExpression(),
									"Impl"))));

					GeneratedClass.Types[0].Members.Add(newAttrProperty);
				}
			}
			#endregion
		}

		void InterfaceCodeAttributes()
		{
			#region Domain Specific Attributes
			if (Subject.MetaBase.Name != "Folder" &&
					Subject.MetaBase.Name != "RootFolder")
			{
				CodeMemberProperty newAttrProperty = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "Attributes",
					Type = new CodeTypeReference(Configuration.GetClassName(Subject) + ".AttributesClass"),
				};

                if (baseClassesWoObject.Count > 0)
                {
					newAttrProperty.Attributes =
						newAttrProperty.Attributes | MemberAttributes.New;
				}

				newAttrProperty.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.Attributes, true));

				GeneratedInterface.Types[0].Members.Add(newAttrProperty);
			}
			#endregion
		}
	}
}
