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
		void ClassCodeSrcDstEnd()
		{
			#region Connection
			#region SrcEnd
			if (Subject.MetaBase.Name == "Connection")
			{
				List<MgaFCO> SrcEndPoints = new List<MgaFCO>();

				List<MgaFCO> _srcEndWProxies = new List<MgaFCO>();

				List<MgaFCO> connectionBase = baseClasses.
					Where(x => x.Meta.Name == "Connection" || x.Meta.Name == "ConnectionProxy").ToList();

				connectionBase.ForEach(x => _srcEndWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));
				_srcEndWProxies.AddRange(connectionBase);

				if (_srcEndWProxies.Count > 0)
				{
					foreach (MgaFCO item in _srcEndWProxies)
					{
						SrcEndPoints.AddRange(GetSrcEnd(item, true));
					}
				}

				CodeTypeDeclaration newSrcConn = new CodeTypeDeclaration("SrcEndsClass")
				{
					Attributes = MemberAttributes.Public,
					IsClass = true,
					TypeAttributes = System.Reflection.TypeAttributes.NestedPublic,
				};

                newSrcConn.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				CodeMemberField impl = new CodeMemberField("global::GME.MGA.IMgaObject", "Impl");
				newSrcConn.Members.Add(impl);

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

				newSrcConn.Members.Add(ctor);

				foreach (MgaFCO item in SrcEndPoints.OfType<MgaAtom>().Distinct())
				{
					CodeMemberProperty newConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Public,
						HasGet = true,
						Name = item.Name,
						Type = new CodeTypeReference(
							Configuration.GetInterfaceName(item as MgaObject)),
					};

					List<MgaFCO> derivedClasses = new List<MgaFCO>();
					derivedClasses.Add(item);
					derivedClasses.AddRange(GetDerivedClasses(item));

					newConnections.Comments.Add(new CodeCommentStatement("<summary>", true));
					newConnections.Comments.Add(
						new CodeCommentStatement("Src ends. NOTE: Does not contain derived classes. Type must exactly match otherwise it is null.", true));

					StringBuilder sb = new StringBuilder();
					sb.AppendLine("<para>");
					sb.AppendLine("-----------------------------------------");
					sb.AppendLine("</para>");
					sb.AppendLine("<para>");
					sb.AppendLine("Type could be:");
					sb.AppendLine("</para>");
					foreach (var derived in derivedClasses)
					{
						sb.AppendLine("<para>");
						sb.AppendFormat("- {0}{1}", derived.Name, Environment.NewLine);
						sb.AppendLine("</para>");
					}
					newConnections.Comments.Add(new CodeCommentStatement(sb.ToString(), true));
					newConnections.Comments.Add(new CodeCommentStatement("</summary>", true));

					newConnections.GetStatements.Add(
						new CodeSnippetStatement(
							Configuration.GetInterfaceName(item as MgaObject) + " result = null;"));

					foreach (var derived in derivedClasses)
					{
						newConnections.GetStatements.Add(
							new CodeSnippetStatement(
								"result = " +
								typeof(ISIS.GME.Common.Utils).FullName + ".CastSrcEnd<" +
								Configuration.GetClassName(derived as MgaObject) +
								", global::" + typeof(MgaFCO).FullName + ">(Impl as global::" +
								typeof(MgaFCO).FullName + ", \"" +
								Configuration.GetKindName(derived as MgaObject) + "\");"));

						newConnections.GetStatements.Add(
							new CodeSnippetExpression(
								"if (result != null) { return result; }"));
					}

					newConnections.GetStatements.Add(
						new CodeMethodReturnStatement(
							new CodeVariableReferenceExpression("result")));

					newSrcConn.Members.Add(newConnections);
				}

				GeneratedClass.Types[0].Members.Add(newSrcConn);

				CodeMemberProperty newAllSrcConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Override,
					HasGet = true,
					Name = "SrcEnd",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.FCO).FullName),
				};

				newAllSrcConnections.Comments.Add(new CodeCommentStatement("", true));

				if (SrcEndPoints.Count() == 0)
				{
					newAllSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("null")));
				}
				else
				{
					StringBuilder sb = new StringBuilder();
					foreach (var childFco in SrcEndPoints)
					{
						sb.Append("((" +
								Configuration.GetInterfaceName(Subject) +
								")(this)).SrcEnds." + childFco.Name + ", " + Environment.NewLine);
					}

					newAllSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(
                            new CodeSnippetExpression("(new " +
                                typeof(ISIS.GME.Common.Interfaces.FCO).FullName +
                                "[] { " + sb.ToString() +
								"}).FirstOrDefault(x => x != null)")));
				}
				GeneratedClass.Types[0].Members.Add(newAllSrcConnections);

				foreach (var item in connectionBase)
				{
					CodeMemberProperty newSrcConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Final,
						HasGet = true,
						Name = Configuration.GetInterfaceName(item as MgaObject) + ".SrcEnds",
						Type = new CodeTypeReference(item.Name + ".SrcEndsClass"),
					};

					newSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("new " + item.Name + ".SrcEndsClass(Impl)")));

					GeneratedClass.Types[0].Members.Add(newSrcConnections);
				}

			}
			#endregion
			#region DstEnd
			if (Subject.MetaBase.Name == "Connection")
			{
				List<MgaFCO> DstEndPoints = new List<MgaFCO>();

				List<MgaFCO> _dstEndWProxies = new List<MgaFCO>();

				List<MgaFCO> connectionBase = baseClasses.
					Where(x => x.Meta.Name == "Connection" || x.Meta.Name == "ConnectionProxy").ToList();

				connectionBase.ForEach(x => _dstEndWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));
				_dstEndWProxies.AddRange(connectionBase);

				if (_dstEndWProxies.Count > 0)
				{
					foreach (MgaFCO item in _dstEndWProxies)
					{
						DstEndPoints.AddRange(GetDstEnd(item, true));
					}
				}

				CodeTypeDeclaration newDstConn = new CodeTypeDeclaration("DstEndsClass")
				{
					Attributes = MemberAttributes.Public,
					IsClass = true,
					TypeAttributes = System.Reflection.TypeAttributes.NestedPublic,
				};

                newDstConn.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.Empty, true));

				CodeMemberField impl = new CodeMemberField("global::GME.MGA.IMgaObject", "Impl");
				newDstConn.Members.Add(impl);

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

				newDstConn.Members.Add(ctor);

				foreach (MgaFCO item in DstEndPoints.OfType<MgaAtom>().Distinct())
				{
					CodeMemberProperty newConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Public,
						HasGet = true,
						Name = item.Name,
						Type = new CodeTypeReference(
							Configuration.GetInterfaceName(item as MgaObject)),
					};

					List<MgaFCO> derivedClasses = new List<MgaFCO>();
					derivedClasses.Add(item);
					derivedClasses.AddRange(GetDerivedClasses(item));

					newConnections.Comments.Add(new CodeCommentStatement("<summary>", true));
					newConnections.Comments.Add(
						new CodeCommentStatement("Dst ends. NOTE: Does not contain derived classes. Type must exactly match otherwise it is null.", true));

					StringBuilder sb = new StringBuilder();
					sb.AppendLine("<para>");
					sb.AppendLine("-----------------------------------------");
					sb.AppendLine("</para>");
					sb.AppendLine("<para>");
					sb.AppendLine("Type could be:");
					sb.AppendLine("</para>");
					foreach (var derived in derivedClasses)
					{
						sb.AppendLine("<para>");
						sb.AppendFormat("- {0}{1}", derived.Name, Environment.NewLine);
						sb.AppendLine("</para>");
					}
					newConnections.Comments.Add(new CodeCommentStatement(sb.ToString(), true));
					newConnections.Comments.Add(new CodeCommentStatement("</summary>", true));

					newConnections.GetStatements.Add(
						new CodeSnippetStatement(
							Configuration.GetInterfaceName(item as MgaObject) + " result = null;"));

					foreach (var derived in derivedClasses)
					{
						newConnections.GetStatements.Add(
							new CodeSnippetStatement(
								"result = " +
								typeof(ISIS.GME.Common.Utils).FullName + ".CastDstEnd<" +
								Configuration.GetClassName(derived as MgaObject) +
								", global::" + typeof(MgaFCO).FullName + ">(Impl as global::" +
								typeof(MgaFCO).FullName + ", \"" +
								Configuration.GetKindName(derived as MgaObject) + "\");"));

						newConnections.GetStatements.Add(
							new CodeSnippetExpression(
								"if (result != null) { return result; }"));
					}

					newConnections.GetStatements.Add(
						new CodeMethodReturnStatement(
							new CodeVariableReferenceExpression("result")));

					newDstConn.Members.Add(newConnections);
				}

				GeneratedClass.Types[0].Members.Add(newDstConn);

				CodeMemberProperty newAllDstConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Override,
					HasGet = true,
					Name = "DstEnd",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.FCO).FullName),
				};

				newAllDstConnections.Comments.Add(new CodeCommentStatement("", true));

				if (DstEndPoints.Count() == 0)
				{
					newAllDstConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("null")));
				}
				else
				{
					StringBuilder sb = new StringBuilder();
					foreach (var childFco in DstEndPoints)
					{
						sb.Append("((" +
								Configuration.GetInterfaceName(Subject) +
								")(this)).DstEnds." + childFco.Name + ", " + Environment.NewLine);
					}

					newAllDstConnections.GetStatements.Add(
						new CodeMethodReturnStatement(
							new CodeSnippetExpression("(new " +
                                typeof(ISIS.GME.Common.Interfaces.FCO).FullName
                                + "[] { " + sb.ToString() +
								"}).FirstOrDefault(x => x != null)")));
				}
				GeneratedClass.Types[0].Members.Add(newAllDstConnections);


				foreach (var item in connectionBase)
				{
					CodeMemberProperty newDstConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Final,
						HasGet = true,
						Name = Configuration.GetInterfaceName(item as MgaObject) + ".DstEnds",
						Type = new CodeTypeReference(item.Name + ".DstEndsClass"),
					};

					newDstConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("new " + item.Name + ".DstEndsClass(Impl)")));

					GeneratedClass.Types[0].Members.Add(newDstConnections);
				}

			}
			#endregion
			#endregion
		}

		void IntefaceCodeSrcDstEnd()
		{

			#region SrcEnds
			if (Subject.MetaBase.Name == "Connection")
			{
				CodeMemberProperty newAllSrcConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "SrcEnd",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.FCO).FullName),
				};

                if (baseClassesWoObject.Any(x => x.MetaBase.MetaRef == Subject.MetaBase.MetaRef))
                {
					newAllSrcConnections.Attributes = newAllSrcConnections.Attributes | MemberAttributes.New;
				}

                newAllSrcConnections.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.AllSrcConnections, true));

				GeneratedInterface.Types[0].Members.Add(newAllSrcConnections);

				CodeMemberProperty newSrcConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "SrcEnds",
					Type = new CodeTypeReference(
						Configuration.GetClassName(Subject) + ".SrcEndsClass"),
				};

                newSrcConnections.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.SrcConnectionEnd, true));


                if (baseClassesWoObject.Any(x => x.MetaBase.MetaRef == Subject.MetaBase.MetaRef))
                {
					newSrcConnections.Attributes = newSrcConnections.Attributes | MemberAttributes.New;
				}

				GeneratedInterface.Types[0].Members.Add(newSrcConnections);
			}
			#endregion
			#region DstEnds
			if (Subject.MetaBase.Name == "Connection")
			{
				CodeMemberProperty newAllDstConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "DstEnd",
					Type = new CodeTypeReference(typeof(ISIS.GME.Common.Interfaces.FCO).FullName),
				};

                if (baseClassesWoObject.Any(x => x.MetaBase.MetaRef == Subject.MetaBase.MetaRef))
                {
					newAllDstConnections.Attributes = newAllDstConnections.Attributes | MemberAttributes.New;
				}

                newAllDstConnections.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.AllDstConnections, true));

				GeneratedInterface.Types[0].Members.Add(newAllDstConnections);

				CodeMemberProperty newDstConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "DstEnds",
					Type = new CodeTypeReference(
						Configuration.GetClassName(Subject) + ".DstEndsClass"),
				};

                newDstConnections.Comments.Add(
                    new CodeCommentStatement(Configuration.Comments.DstConnectionEnd, true));

                if (baseClassesWoObject.Any(x => x.MetaBase.MetaRef == Subject.MetaBase.MetaRef))
                {
					newDstConnections.Attributes = newDstConnections.Attributes | MemberAttributes.New;
				}

				GeneratedInterface.Types[0].Members.Add(newDstConnections);
			}
			#endregion
		}

	}
}
