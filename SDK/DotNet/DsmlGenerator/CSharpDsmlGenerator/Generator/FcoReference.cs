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

		void ClassCodeReferred()
		{
			if (Subject.MetaBase.Name == "Reference")
			{
				List<MgaFCO> Referred = new List<MgaFCO>();

				List<MgaFCO> _ReferredWProxies = new List<MgaFCO>();

				List<MgaFCO> referenceBase = baseClasses.
					Where(x => x.Meta.Name == "Reference" || x.Meta.Name == "ReferenceProxy").ToList();

				referenceBase.ForEach(x => _ReferredWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));
				_ReferredWProxies.AddRange(referenceBase);

				if (_ReferredWProxies.Count > 0)
				{
					foreach (MgaFCO item in _ReferredWProxies)
					{
						Referred.AddRange(GetReferred(item, true));
					}
				}

				CodeTypeDeclaration newRefClass = new CodeTypeDeclaration("ReferredClass")
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
							Configuration.GetInterfaceName(item as MgaObject)),
					};

					List<MgaFCO> derivedClasses = new List<MgaFCO>();
					derivedClasses.Add(item);
					derivedClasses.AddRange(GetDerivedClasses(item));
					derivedClasses = derivedClasses.Distinct().ToList();

					newReferred.Comments.Add(new CodeCommentStatement("<summary>", true));
					newReferred.Comments.Add(
						new CodeCommentStatement("Referred objects", true));

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
							Configuration.GetInterfaceName(item as MgaObject) + " result = null;"));

					sb.Clear();
					sb.AppendLine("result = ISIS.GME.Common.Utils.CastReferred(Impl as global::GME.MGA.MgaReference, new global::System.Collections.Generic.Dictionary<int, global::System.Type>() {");

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
					sb.AppendFormat("}}, \"{0}\") as ", item.Name);
					sb.Append(Configuration.GetInterfaceName(item as MgaObject));

					newReferred.GetStatements.Add(
						new CodeSnippetExpression(sb.ToString()));

					newReferred.GetStatements.Add(
						new CodeMethodReturnStatement(
							new CodeVariableReferenceExpression("result")));

					sb.Clear();
					sb.Append(typeof(ISIS.GME.Common.Utils).FullName);
					sb.Append(".");
					sb.Append("SetReferred(Impl as global::");
					sb.Append(typeof(MgaReference).FullName);
					sb.Append(", value)");
					newReferred.SetStatements.Add(
						new CodeSnippetExpression(sb.ToString()));

					newRefClass.Members.Add(newReferred);
				}

				GeneratedClass.Types[0].Members.Add(newRefClass);

				CodeMemberProperty newAllReferred = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "AllReferred",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.FCO).FullName),
				};

				newAllReferred.Comments.Add(new CodeCommentStatement("", true));

				newAllReferred.GetStatements.Add(
					new CodeSnippetStatement(
						typeof(ISIS.GME.Common.Interfaces.FCO).FullName + " result = null;"));

				StringBuilder sb2 = new StringBuilder();
				sb2.AppendLine("result = ISIS.GME.Common.Utils.CastReferred(Impl as global::GME.MGA.MgaReference, new global::System.Collections.Generic.Dictionary<int, global::System.Type>() {");

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
				sb2.Append("}) as ");
				sb2.Append(typeof(ISIS.GME.Common.Interfaces.FCO).FullName);

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
						Name = Configuration.GetInterfaceName(item as MgaObject) + ".Referred",
						Type = new CodeTypeReference(Configuration.GetClassName(item) + ".ReferredClass"),
					};

					newSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("new " + Configuration.GetClassName(item) + ".ReferredClass(Impl)")));

					GeneratedClass.Types[0].Members.Add(newSrcConnections);
				}

			}
		}

		void InterfaceCodeReferred()
		{
			if (Subject.MetaBase.Name == "Reference")
			{
				CodeMemberProperty newAllReferred = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "AllReferred",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.FCO).FullName),
				};

                if (baseClassesWoObject.Any(x => x.MetaBase.MetaRef == Subject.MetaBase.MetaRef))
                {
					newAllReferred.Attributes =
						newAllReferred.Attributes | MemberAttributes.New;
				}

				newAllReferred.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.AllReferred, true));

				GeneratedInterface.Types[0].Members.Add(newAllReferred);

				CodeMemberProperty newReferred = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "Referred",
					Type = new CodeTypeReference(
						Configuration.GetClassName(Subject) + ".ReferredClass"),
				};

                if (baseClassesWoObject.Any(x => x.MetaBase.MetaRef == Subject.MetaBase.MetaRef))
                {
					newReferred.Attributes =
						newReferred.Attributes | MemberAttributes.New;
				}

				newReferred.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.Referred, true));

				GeneratedInterface.Types[0].Members.Add(newReferred);
			}
		}

		void ClassCodeReferencedBy()
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
						Referred.AddRange(GetReferencedBy(item, true));
					}
				}

				CodeTypeDeclaration newRefClass = new CodeTypeDeclaration("ReferencedByClass")
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
						new CodeCommentStatement("ReferencedBy objects", true));

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
                    sb.AppendLine("ISIS.GME.Common.Utils.CastReferencedBy(Impl as global::GME.MGA.MgaFCO, new global::System.Collections.Generic.Dictionary<int, global::System.Type>() {");

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
					Name = "AllReferencedBy",
                    Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.FCO).FullName + ">"),
				};

				newAllReferred.Comments.Add(new CodeCommentStatement("", true));

				StringBuilder sb2 = new StringBuilder();
                sb2.AppendLine("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.FCO).FullName + "> result = ISIS.GME.Common.Utils.CastReferencedBy(Impl as global::GME.MGA.MgaFCO, new global::System.Collections.Generic.Dictionary<int, global::System.Type>() {");

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
						Name = Configuration.GetInterfaceName(item as MgaObject) + ".ReferencedBy",
						Type = new CodeTypeReference(Configuration.GetClassName(item) + ".ReferencedByClass"),
					};

					newSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("new " + Configuration.GetClassName(item) + ".ReferencedByClass(Impl)")));

					GeneratedClass.Types[0].Members.Add(newSrcConnections);
				}

			}
		}

		void InterfaceCodeReferencedBy()
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
					Name = "AllReferencedBy",
					Type = new CodeTypeReference(
                        "global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.FCO).FullName + ">"),
				};

                if (baseClassesWoObject.Count > 0)
				{
					newAllReferencedBy.Attributes =
						newAllReferencedBy.Attributes | MemberAttributes.New;
				}

				newAllReferencedBy.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.AllReferencedBy, true));

				GeneratedInterface.Types[0].Members.Add(newAllReferencedBy);

				CodeMemberProperty newReferencedBy = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "ReferencedBy",
					Type = new CodeTypeReference(
						Configuration.GetClassName(Subject) + ".ReferencedByClass"),
				};

				if (baseClassesWoObject.Count > 0)
				{
					newReferencedBy.Attributes =
						newReferencedBy.Attributes | MemberAttributes.New;
				}

				newReferencedBy.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.ReferencedBy, true));

				GeneratedInterface.Types[0].Members.Add(newReferencedBy);
			}
		}
	}
}
