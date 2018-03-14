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
		void ClassCodeSrcDstConnections()
		{
			#region Connections
			#region Src Connections
			if (Subject.MetaBase.Name != "RootFolder" &&
				Subject.MetaBase.Name != "Folder")
			{
				// find the connection objects

                List<Tuple<MgaFCO, string>> connections = new List<Tuple<MgaFCO, string>>();

				List<MgaFCO> _listWProxies = new List<MgaFCO>();

				baseClasses.ForEach(x => _listWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));
				_listWProxies.AddRange(baseClasses);

				if (_listWProxies.Count > 0)
				{
					foreach (MgaFCO item in _listWProxies)
					{
						connections.AddRange(GetSrcConnections(item));
					}
				}

				CodeTypeDeclaration newSrcConn = new CodeTypeDeclaration("SrcConnectionsClass")
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

                var uniqueNames = Configuration.GetUniqueNames(connections.Select(x => x.Item1), connections.Count).ToDictionary(x => x.Item1, x => x.Item2);
				foreach (IGrouping<MgaFCO, string> group in connections.GroupBy(x => x.Item1, x => x.Item2))
				{
                    MgaFCO item = group.Key;
                    if (!(item is MgaAtom))
                    {
                        continue;
                    }
					CodeMemberProperty newConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Public,
						HasGet = true,
                        Name = uniqueNames[item] + "Collection",
						Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + Configuration.GetInterfaceName(item as MgaObject) + ">"),
					};

					newConnections.Comments.Add(
						new CodeCommentStatement(Configuration.Comments.SrcConnections, true));

                    if (group.Count() == 1)
                    {
					    newConnections.GetStatements.Add(
						    new CodeMethodReturnStatement(
							    new CodeSnippetExpression(typeof(ISIS.GME.Common.Utils).FullName + ".CastSrcConnections<" + Configuration.GetClassName(item as MgaObject) + ", global::" + typeof(MgaFCO).FullName + ">(Impl as global::" + typeof(MgaFCO).FullName + ", \"" + Configuration.GetKindName(item as MgaObject) + "\")")));
                    }
                    else
                    {
					    newConnections.GetStatements.Add(
						    new CodeMethodReturnStatement(
							    new CodeSnippetExpression(
                                    
                                    
                                    typeof(ISIS.GME.Common.Utils).FullName + ".CastSrcConnections<" + Configuration.GetClassName(item as MgaObject) + ", global::" + typeof(MgaFCO).FullName + ">(Impl as global::" + typeof(MgaFCO).FullName + ", \"" + Configuration.GetKindName(item as MgaObject) + "\")"
                                    + ".Concat(" +
                                    typeof(ISIS.GME.Common.Utils).FullName + ".CastDstConnections<" + Configuration.GetClassName(item as MgaObject) + ", global::" + typeof(MgaFCO).FullName + ">(Impl as global::" + typeof(MgaFCO).FullName + ", \"" + Configuration.GetKindName(item as MgaObject) + "\")"
                                    + ")"
                                    )));
                    }

					newSrcConn.Members.Add(newConnections);
				}

				GeneratedClass.Types[0].Members.Add(newSrcConn);

				CodeMemberProperty newAllSrcConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Override,
					HasGet = true,
					Name = "AllSrcConnections",
					Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Connection).FullName + ">"),
				};

				newAllSrcConnections.Comments.Add(new CodeCommentStatement("", true));

				if (connections.Count() == 0)
				{
					newAllSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(
							new CodeSnippetExpression("new " + typeof(ISIS.GME.Common.Interfaces.Connection).FullName + "[] {}")));
				}
				else
				{
                    var allConnectionClasses = connections.Select(x => x.Item1).Distinct();
					newAllSrcConnections.GetStatements.Add(
							new CodeSnippetExpression(
								"global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Connection).FullName + "> result = ((" +
								Configuration.GetInterfaceName(Subject) +
                                ")(this)).SrcConnections." + allConnectionClasses.FirstOrDefault().Name + "Collection.Cast<" +
								typeof(ISIS.GME.Common.Interfaces.Connection).FullName + ">()"));

                    foreach (var childFco in allConnectionClasses.Skip(1))
					{
						newAllSrcConnections.GetStatements.Add(
							new CodeSnippetExpression(
								"result = result.Concat(((" +
								Configuration.GetInterfaceName(Subject) +
								")(this)).SrcConnections." + childFco.Name + "Collection.Cast<" +
								typeof(ISIS.GME.Common.Interfaces.Connection).FullName + ">())"));
					}

					newAllSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("result")));
				}
				GeneratedClass.Types[0].Members.Add(newAllSrcConnections);

				foreach (var item in baseClasses)
				{
					CodeMemberProperty newSrcConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Final,
						HasGet = true,
						Name = Configuration.GetInterfaceName(item as MgaObject) + ".SrcConnections",
						Type = new CodeTypeReference(
							Configuration.GetClassName(item as MgaObject) + ".SrcConnectionsClass"),
					};

					newSrcConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("new " + Configuration.GetClassName(item as MgaObject) + ".SrcConnectionsClass(Impl)")));

					GeneratedClass.Types[0].Members.Add(newSrcConnections);
				}
			}
			#endregion

			#region Dst Connections
			if (Subject.MetaBase.Name != "RootFolder" &&
				Subject.MetaBase.Name != "Folder")
			{
				// find the connection objects

				List<Tuple<MgaFCO, string>> connections = new List<Tuple<MgaFCO, string>>();

				List<MgaFCO> _listWProxies = new List<MgaFCO>();

				baseClasses.ForEach(x => _listWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));
				_listWProxies.AddRange(baseClasses);

				if (_listWProxies.Count > 0)
				{
					foreach (MgaFCO item in _listWProxies)
					{
						connections.AddRange(GetDstConnections(item));
					}
				}

				CodeTypeDeclaration newDstConn = new CodeTypeDeclaration("DstConnectionsClass")
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

                var uniqueNames = Configuration.GetUniqueNames(connections.Select(x => x.Item1), connections.Count).ToDictionary(x => x.Item1, x => x.Item2);
				foreach (IGrouping<MgaFCO, string> group in connections.GroupBy(x => x.Item1, x => x.Item2))
				{
                    MgaFCO item = group.Key;
                    if (!(item is MgaAtom))
                    {
                        continue;
                    }
					CodeMemberProperty newConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Public,
						HasGet = true,
						Name = uniqueNames[item] + "Collection",
						Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + Configuration.GetInterfaceName(item as MgaObject) + ">"),
					};

					newConnections.Comments.Add(
						new CodeCommentStatement(Configuration.Comments.DstConnections, true));

                    if (group.Count() == 1)
                    {
                        newConnections.GetStatements.Add(
                            new CodeMethodReturnStatement(
                                new CodeSnippetExpression(typeof(ISIS.GME.Common.Utils).FullName + ".CastDstConnections<" + Configuration.GetClassName(item as MgaObject) + ", global::" + typeof(MgaFCO).FullName + ">(Impl as global::" + typeof(MgaFCO).FullName + ", \"" + Configuration.GetKindName(item as MgaObject) + "\")")));
                    }
                    else
                    {
                        newConnections.GetStatements.Add(
                            new CodeMethodReturnStatement(
                                new CodeSnippetExpression(
                                    typeof(ISIS.GME.Common.Utils).FullName + ".CastSrcConnections<" + Configuration.GetClassName(item as MgaObject) + ", global::" + typeof(MgaFCO).FullName + ">(Impl as global::" + typeof(MgaFCO).FullName + ", \"" + Configuration.GetKindName(item as MgaObject) + "\")"
                                    + ".Concat(" +
                                    typeof(ISIS.GME.Common.Utils).FullName + ".CastDstConnections<" + Configuration.GetClassName(item as MgaObject) + ", global::" + typeof(MgaFCO).FullName + ">(Impl as global::" + typeof(MgaFCO).FullName + ", \"" + Configuration.GetKindName(item as MgaObject) + "\")"
                                    + ")"
                                    )));
                    }

					newDstConn.Members.Add(newConnections);
				}

				GeneratedClass.Types[0].Members.Add(newDstConn);

				CodeMemberProperty newAllDstConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Override,
					HasGet = true,
					Name = "AllDstConnections",
					Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Connection).FullName + ">"),
				};

				newAllDstConnections.Comments.Add(new CodeCommentStatement("", true));

				if (connections.Count() == 0)
				{
					newAllDstConnections.GetStatements.Add(
						new CodeMethodReturnStatement(
							new CodeSnippetExpression("new " + typeof(ISIS.GME.Common.Interfaces.Connection).FullName + "[] {}")));
				}
				else
				{
                    var allConnectionClasses = connections.Select(x => x.Item1).Distinct();
					newAllDstConnections.GetStatements.Add(
							new CodeSnippetExpression(
								"global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Connection).FullName + "> result = ((" +
								Configuration.GetInterfaceName(Subject) +
                                ")(this)).DstConnections." + allConnectionClasses.FirstOrDefault().Name + "Collection.Cast<" +
								typeof(ISIS.GME.Common.Interfaces.Connection).FullName + ">()"));

                    foreach (var childFco in allConnectionClasses.Skip(1))
					{
						newAllDstConnections.GetStatements.Add(
							new CodeSnippetExpression(
								"result = result.Concat(((" +
								Configuration.GetInterfaceName(Subject) +
								")(this)).DstConnections." + childFco.Name + "Collection.Cast<" +
								typeof(ISIS.GME.Common.Interfaces.Connection).FullName + ">())"));
					}

					newAllDstConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("result")));
				}
				GeneratedClass.Types[0].Members.Add(newAllDstConnections);

				foreach (var item in baseClasses)
				{
					CodeMemberProperty newDstConnections = new CodeMemberProperty()
					{
						Attributes = MemberAttributes.Final,
						HasGet = true,
						Name = Configuration.GetInterfaceName(item as MgaObject) + ".DstConnections",
						Type = new CodeTypeReference(
							Configuration.GetClassName(item as MgaObject) + ".DstConnectionsClass"),
					};

					newDstConnections.GetStatements.Add(
						new CodeMethodReturnStatement(new CodeSnippetExpression("new " + Configuration.GetClassName(item as MgaObject) + ".DstConnectionsClass(Impl)")));

					GeneratedClass.Types[0].Members.Add(newDstConnections);
				}
			}

			#endregion
			#endregion
		}

		void InterfaceCodeSrcDstConnections()
		{
			#region Src Connections
			if (Subject.MetaBase.Name != "RootFolder" &&
				Subject.MetaBase.Name != "Folder")
			{
				CodeMemberProperty newAllSrcConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Override,
					HasGet = true,
					Name = "AllSrcConnections",
					Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Connection).FullName + ">"),
				};

                if (baseClasses.Count > 1)
                {
					newAllSrcConnections.Attributes = 
						newAllSrcConnections.Attributes | MemberAttributes.New;
				}

				newAllSrcConnections.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.AllSrcConnections, true));

				GeneratedInterface.Types[0].Members.Add(newAllSrcConnections);

				CodeMemberProperty newSrcConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "SrcConnections",
					Type = new CodeTypeReference(
						Configuration.GetClassName(Subject) + ".SrcConnectionsClass"),
				};

                if (baseClasses.Count > 1)
                {
					newSrcConnections.Attributes =
						newSrcConnections.Attributes | MemberAttributes.New;
				}

				newSrcConnections.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.SrcConnections, true));

				GeneratedInterface.Types[0].Members.Add(newSrcConnections);

			}
			#endregion
			#region Dst Connections
			if (Subject.MetaBase.Name != "RootFolder" &&
				Subject.MetaBase.Name != "Folder")
			{
				CodeMemberProperty newAllDstConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public | MemberAttributes.Override,
					HasGet = true,
					Name = "AllDstConnections",
					Type = new CodeTypeReference("global::System.Collections.Generic.IEnumerable<" + typeof(ISIS.GME.Common.Interfaces.Connection).FullName + ">"),
				};

                if (baseClasses.Count > 1)
                {
					newAllDstConnections.Attributes =
						newAllDstConnections.Attributes | MemberAttributes.New;
				}

				newAllDstConnections.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.AllDstConnections, true));

				GeneratedInterface.Types[0].Members.Add(newAllDstConnections);

				CodeMemberProperty newDstConnections = new CodeMemberProperty()
				{
					Attributes = MemberAttributes.Public,
					HasGet = true,
					Name = "DstConnections",
					Type = new CodeTypeReference(
						Configuration.GetClassName(Subject) + ".DstConnectionsClass"),
				};

                if (baseClasses.Count > 1)
                {
					newDstConnections.Attributes =
						newDstConnections.Attributes | MemberAttributes.New;
				}

				newDstConnections.Comments.Add(
					new CodeCommentStatement(Configuration.Comments.DstConnections, true));

				GeneratedInterface.Types[0].Members.Add(newDstConnections);
			}
			#endregion
		}
	}
}
