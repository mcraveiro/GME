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

		void ClassCodeSetMembers()
		{
			if (Subject.MetaBase.Name == "Set")
			{
				List<MgaFCO> Referred = new List<MgaFCO>();

				List<MgaFCO> _ReferredWProxies = new List<MgaFCO>();

				List<MgaFCO> referenceBase = baseClasses.
					Where(x => x.Meta.Name == "Set" || x.Meta.Name == "SetProxy").ToList();

				referenceBase.ForEach(x => _ReferredWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));
				_ReferredWProxies.AddRange(referenceBase);

				if (_ReferredWProxies.Count > 0)
				{
					foreach (MgaFCO item in _ReferredWProxies)
					{
						Referred.AddRange(GetSetMembers(item, true));
					}
				}

				CodeTypeDeclaration newRefClass = new CodeTypeDeclaration("SetMembersClass")
				{
					Attributes = MemberAttributes.Public,
					IsClass = true,
					TypeAttributes = System.Reflection.TypeAttributes.NestedPublic,
				};

                newRefClass.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				CodeMemberField impl = new CodeMemberField("global::GME.MGA.IMgaObject", "Impl");
				newRefClass.Members.Add(impl);

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

				newRefClass.Members.Add(ctor);

				foreach (MgaFCO item in Referred.OfType<MgaAtom>().Distinct())
				{
					CodeMemberProperty newReferred = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Public,
						HasGet = true,
						HasSet = true,
						Name = item.Name,
						Type = new CodeTypeReference(
                            "global::System.Collections.Generic.IEnumerable<" + Configuration.GetInterfaceName(item as MgaObject) + ">"),
					};

					List<MgaFCO> derivedClasses = new List<MgaFCO>();
					derivedClasses.Add(item);
					derivedClasses.AddRange(GetDerivedClasses(item));
					derivedClasses = derivedClasses.Distinct().ToList();

					newReferred.Comments.Add(new CodeCommentStatement("<summary>", true));
					newReferred.Comments.Add(
						new CodeCommentStatement("SetMember objects", true));

					StringBuilder sb = new StringBuilder();
					sb.Append("<para>");
					sb.Append("-----------------------------------------");
					sb.AppendLine("</para>");
					sb.Append("<para>");
					sb.Append("Type could be:");
					sb.AppendLine("</para>");
					foreach (var derived in derivedClasses)
					{
						sb.Append("<para>");
						sb.AppendFormat("- {0}", derived.Name);
						sb.AppendLine("</para>");
					}
					newReferred.Comments.Add(new CodeCommentStatement(sb.ToString(), true));
					newReferred.Comments.Add(new CodeCommentStatement("</summary>", true));

					newReferred.GetStatements.Add(
						new CodeSnippetStatement(
							"global::System.Collections.Generic.IEnumerable<" + Configuration.GetInterfaceName(item as MgaObject) + "> result = null;"));

					sb.Clear();
					sb.AppendLine("result = ISIS.GME.Common.Utils.CastSetMembers(Impl as global::GME.MGA.MgaSet, new global::System.Collections.Generic.Dictionary<int, global::System.Type>() {");

					List<int> metaRefs = new List<int>();

					foreach (var derived in derivedClasses)
					{
						int metaRef = Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(derived as MgaObject));
						if (metaRefs.Contains(metaRef) ||
								metaRef == 0)
						{
							continue;
						}
						else
						{
							sb.Append("{ ");
							sb.Append(metaRef);
							sb.Append(", typeof(");
							sb.Append(Configuration.GetClassName(derived as MgaObject));
							sb.Append(") },");
							sb.AppendLine();
						}
					}
					sb.Append("}).Cast<");
					sb.Append(Configuration.GetInterfaceName(item as MgaObject));
					sb.Append(">()");
					newReferred.GetStatements.Add(
						new CodeSnippetExpression(sb.ToString()));

					newReferred.GetStatements.Add(
						new CodeMethodReturnStatement(
							new CodeVariableReferenceExpression("result")));

					newRefClass.Members.Add(newReferred);
				}

				GeneratedClass.Types[0].Members.Add(newRefClass);

				CodeMemberProperty newAllReferred = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.New,
					HasGet = true,
					Name = "AllSetMembers",
					Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable <" + typeof(ISIS.GME.Common.Interfaces.FCO).FullName + ">"),
				};

				newAllReferred.Comments.Add(new CodeCommentStatement("", true));

				newAllReferred.GetStatements.Add(
					new CodeSnippetStatement(
						"global::System.Collections.Generic.IEnumerable <" + typeof(ISIS.GME.Common.Interfaces.FCO).FullName + "> result = null;"));

				StringBuilder sb2 = new StringBuilder();
				sb2.AppendLine("result = ISIS.GME.Common.Utils.CastSetMembers(Impl as global::GME.MGA.MgaSet, new global::System.Collections.Generic.Dictionary<int, global::System.Type>() {");

				List<int> metaRefs2 = new List<int>();

				foreach (MgaFCO item in Referred.OfType<MgaAtom>().Distinct())
				{
					List<MgaFCO> derivedClasses = new List<MgaFCO>();
					derivedClasses.Add(item);
					derivedClasses.AddRange(GetDerivedClasses(item));
					derivedClasses = derivedClasses.Distinct().ToList();

					foreach (var derived in derivedClasses)
					{
						int metaRef = Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(derived as MgaObject));
						if (metaRefs2.Contains(metaRef) ||
								metaRef == 0)
						{
							continue;
						}
						else
						{
							metaRefs2.Add(metaRef);
							sb2.Append("{ ");
							sb2.Append(metaRef);
							sb2.Append(", typeof(");
							sb2.Append(Configuration.GetClassName(derived as MgaObject));
							sb2.Append(") },");
						}
						sb2.AppendLine();
					}
				}
				sb2.Append("})");

				newAllReferred.GetStatements.Add(
					new CodeSnippetExpression(sb2.ToString()));

				newAllReferred.GetStatements.Add(
					new CodeMethodReturnStatement(
						new CodeVariableReferenceExpression("result")));


				GeneratedClass.Types[0].Members.Add(newAllReferred);

				foreach (var item in referenceBase)
				{
					CodeMemberProperty newSrcConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Final,
						HasGet = true,
						Name = Configuration.GetInterfaceName(item as MgaObject) + ".SetMembers",
						Type = new CodeTypeReference(Configuration.GetClassName(item) + ".SetMembersClass"),
					};

					newSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("new " + Configuration.GetClassName(item) + ".SetMembersClass(Impl)")));

					GeneratedClass.Types[0].Members.Add(newSrcConnections);
				}

			}
		}

		void InterfaceCodeSetMembers()
		{
			if (Subject.MetaBase.Name == "Set")
			{
				CodeMemberProperty newAllReferred = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "AllSetMembers",
					Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable <" + typeof(ISIS.GME.Common.Interfaces.FCO).FullName + ">"),
				};

				if (baseClassesWoObject.Count > 0)
				{
					newAllReferred.Attributes = newAllReferred.Attributes | MemberAttributes.New;
				}

				newAllReferred.Comments.Add(
					new CodeCommentStatement("Contains the domain specific ....", true));

				GeneratedInterface.Types[0].Members.Add(newAllReferred);

				CodeMemberProperty newReferred = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "SetMembers",
					Type = new CodeTypeReference(
						Configuration.GetClassName(Subject) + ".SetMembersClass"),
				};

				if (baseClassesWoObject.Count > 0)
				{
					newReferred.Attributes = newReferred.Attributes | MemberAttributes.New;
				}

				GeneratedInterface.Types[0].Members.Add(newReferred);
			}
		}

		void ClassCodeMemberOfSets()
		{
			if (Subject.MetaBase.Name == "FCO" ||
				Subject.MetaBase.Name == "Reference" ||
				Subject.MetaBase.Name == "Set" ||
				Subject.MetaBase.Name == "Atom" ||
				Subject.MetaBase.Name == "Connection" ||
				Subject.MetaBase.Name == "Model")
			{
				List<MgaFCO> Referred = new List<MgaFCO>();

				List<MgaFCO> _ReferredWProxies = new List<MgaFCO>();

				List<MgaFCO> referenceBase = baseClasses.ToList();

				referenceBase.ForEach(x => _ReferredWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));
				_ReferredWProxies.AddRange(referenceBase);

				if (_ReferredWProxies.Count > 0)
				{
					foreach (MgaFCO item in _ReferredWProxies)
					{
						Referred.AddRange(GetMembersOfSets(item, true));
					}
				}

				CodeTypeDeclaration newRefClass = new CodeTypeDeclaration("MembersOfSetClass")
				{
					Attributes = MemberAttributes.Public,
					IsClass = true,
					TypeAttributes = System.Reflection.TypeAttributes.NestedPublic,
				};

                newRefClass.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				CodeMemberField impl = new CodeMemberField("global::GME.MGA.IMgaObject", "Impl");
				newRefClass.Members.Add(impl);

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

				newRefClass.Members.Add(ctor);

				foreach (MgaFCO item in Referred.OfType<MgaAtom>().Distinct())
				{
					CodeMemberProperty newReferred = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Public,
						HasGet = true,
						Name = item.Name,
						Type = new CodeTypeReference(
							"global::System.Collections.Generic.IEnumerable<" + Configuration.GetInterfaceName(item as MgaObject) + ">"),
					};

					List<MgaFCO> derivedClasses = new List<MgaFCO>();
					derivedClasses.Add(item);
					derivedClasses.AddRange(GetDerivedClasses(item));
					derivedClasses = derivedClasses.Distinct().ToList();

					newReferred.Comments.Add(new CodeCommentStatement("<summary>", true));
					newReferred.Comments.Add(
						new CodeCommentStatement("MembersOfSet objects", true));

					StringBuilder sb = new StringBuilder();
					sb.Append("<para>");
					sb.Append("-----------------------------------------");
					sb.AppendLine("</para>");
					sb.Append("<para>");
					sb.Append("Type could be:");
					sb.AppendLine("</para>");
					foreach (var derived in derivedClasses)
					{
						sb.Append("<para>");
						sb.AppendFormat("- {0}", derived.Name);
						sb.AppendLine("</para>");
					}
					newReferred.Comments.Add(new CodeCommentStatement(sb.ToString(), true));
					newReferred.Comments.Add(new CodeCommentStatement("</summary>", true));

					//newReferred.GetStatements.Add(
					//  new CodeSnippetStatement(
					//    Configuration.GetInterfaceName(item as MgaObject) + " result = null;"));

					sb.Clear();
					sb.AppendLine("ISIS.GME.Common.Utils.CastMembersOfSet(Impl as global::GME.MGA.MgaFCO, new global::System.Collections.Generic.Dictionary<int, global::System.Type>() {");

					List<int> metaRefs = new List<int>();

					foreach (var derived in derivedClasses)
					{
						int metaRef = Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(derived as MgaObject));
						if (metaRefs.Contains(metaRef) ||
								metaRef == 0)
						{
							continue;
						}
						else
						{
							sb.Append("{ ");
							sb.Append(metaRef);
							sb.Append(", typeof(");
							sb.Append(Configuration.GetClassName(derived as MgaObject));
							sb.Append(") },");
							sb.AppendLine();
						}
					}
					sb.Append("}).Cast<");
					sb.Append(Configuration.GetInterfaceName(item as MgaObject));
					sb.Append(">()");

					newReferred.GetStatements.Add(
						new CodeMethodReturnStatement(
							new CodeVariableReferenceExpression(sb.ToString())));

					newRefClass.Members.Add(newReferred);
				}

				GeneratedClass.Types[0].Members.Add(newRefClass);

				CodeMemberProperty newAllReferred = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "AllMembersOfSet",
					Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.FCO).FullName + ">"),
				};

				newAllReferred.Comments.Add(new CodeCommentStatement("", true));

				StringBuilder sb2 = new StringBuilder();
				sb2.AppendLine("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.FCO).FullName + "> result = ISIS.GME.Common.Utils.CastMembersOfSet(Impl as global::GME.MGA.MgaFCO, new global::System.Collections.Generic.Dictionary<int, global::System.Type>() {");

				List<int> metaRefs2 = new List<int>();

				foreach (MgaFCO item in Referred.OfType<MgaAtom>().Distinct())
				{
					List<MgaFCO> derivedClasses = new List<MgaFCO>();
					derivedClasses.Add(item);
					derivedClasses.AddRange(GetDerivedClasses(item));
					derivedClasses = derivedClasses.Distinct().ToList();

					foreach (var derived in derivedClasses)
					{
						int metaRef = Configuration.DsmlModel.GetMetaRef(Configuration.GetKindName(derived as MgaObject));
						if (metaRefs2.Contains(metaRef) ||
								metaRef == 0)
						{
							continue;
						}
						else
						{
							metaRefs2.Add(metaRef);
							sb2.Append("{ ");
							sb2.Append(metaRef);
							sb2.Append(", typeof(");
							sb2.Append(Configuration.GetClassName(derived as MgaObject));
							sb2.Append(") },");
						}
						sb2.AppendLine();
					}
				}
				sb2.Append("})");

				newAllReferred.GetStatements.Add(
					new CodeSnippetExpression(sb2.ToString()));

				newAllReferred.GetStatements.Add(
					new CodeMethodReturnStatement(
						new CodeVariableReferenceExpression("result")));


				GeneratedClass.Types[0].Members.Add(newAllReferred);

				foreach (var item in referenceBase)
				{
					CodeMemberProperty newSrcConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Final,
						HasGet = true,
						Name = Configuration.GetInterfaceName(item as MgaObject) + ".MembersOfSet",
						Type = new CodeTypeReference(Configuration.GetClassName(item) + ".MembersOfSetClass"),
					};

					newSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("new " + Configuration.GetClassName(item) + ".MembersOfSetClass(Impl)")));

					GeneratedClass.Types[0].Members.Add(newSrcConnections);
				}

			}
		}

		void InterfaceCodeMemberOfSets()
		{
			if (Subject.MetaBase.Name == "FCO" ||
				Subject.MetaBase.Name == "Reference" ||
				Subject.MetaBase.Name == "Set" ||
				Subject.MetaBase.Name == "Atom" ||
				Subject.MetaBase.Name == "Connection" ||
				Subject.MetaBase.Name == "Model")
			{
				CodeMemberProperty newAllReferencedBy = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "AllMembersOfSet",
					Type = new CodeTypeReference(
						"global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.FCO).FullName + ">"),
				};

				if (baseClassesWoObject.Count > 0)
				{
					newAllReferencedBy.Attributes = newAllReferencedBy.Attributes | MemberAttributes.New;
				}

                newAllReferencedBy.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				GeneratedInterface.Types[0].Members.Add(newAllReferencedBy);

				CodeMemberProperty newReferencedBy = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "MembersOfSet",
					Type = new CodeTypeReference(
						Configuration.GetClassName(Subject) + ".MembersOfSetClass"),
				};

                newReferencedBy.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				if (baseClassesWoObject.Count > 0)
				{
					newReferencedBy.Attributes = newReferencedBy.Attributes | MemberAttributes.New;
				}

				GeneratedInterface.Types[0].Members.Add(newReferencedBy);
			}
		}
	}
}
