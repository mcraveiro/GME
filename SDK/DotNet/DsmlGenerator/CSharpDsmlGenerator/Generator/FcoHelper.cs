using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;
using System.Diagnostics.Contracts;

namespace CSharpDSMLGenerator.Generator
{
	public partial class FCO
	{
		#region Meta Model Helper Functions

		public static IEnumerable<MgaFCO> GetReferred(
			MgaFCO mgaFCO,
			bool includeDerived = false)
		{
			Contract.Requires(mgaFCO != null);
			Contract.Requires(mgaFCO.MetaBase.Name == "Reference");

			List<MgaFCO> result = new List<MgaFCO>();

			List<MgaFCO> baseReferences = GetBaseClasses(mgaFCO).
				Where(x => x.MetaBase.Name == "Reference").ToList();

			baseReferences.Add(mgaFCO);
			List<MgaFCO> baseReferencesWProxies = baseReferences.ToList();
			baseReferences.ForEach(x => baseReferencesWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));

			foreach (MgaFCO item in baseReferences)
			{
				foreach (MgaConnPoint cp in item.PartOfConns)
				{
					MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
					if (simple.Src == item &&
						simple.MetaBase.Name == "ReferTo")
					{
						if (simple.Dst is MgaReference)
						{
							result.Add((simple.Dst as MgaReference).Referred);
							if (includeDerived)
							{
								if (includeDerived)
								{
									foreach (var derived in GetDerivedClasses((simple.Dst as MgaReference).Referred))
									{
										result.Add(derived);
									}
								}
							}
						}
						else
						{
							result.Add(simple.Dst);
							if (includeDerived)
							{
								foreach (var derived in GetDerivedClasses(simple.Dst))
								{
									result.Add(derived);
								}
							}
						}
					}
				}
			}
			return result.Distinct();
		}

		public static IEnumerable<MgaFCO> GetReferencedBy(
			MgaFCO mgaFCO,
			bool includeDerived = false)
		{
			Contract.Requires(mgaFCO != null);

			List<MgaFCO> baseReferences = GetBaseClasses(mgaFCO).ToList();

			baseReferences.Add(mgaFCO);
			List<MgaFCO> baseReferencesWProxies = baseReferences.ToList();
			baseReferences.ForEach(x => baseReferencesWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));

			foreach (MgaFCO item in baseReferences)
			{
				foreach (MgaConnPoint cp in item.PartOfConns)
				{
					MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
					if (simple.Dst == item &&
						simple.MetaBase.Name == "ReferTo")
					{
						if (simple.Src is MgaReference)
						{
							yield return (simple.Src as MgaReference).Referred;
							if (includeDerived)
							{
								if (includeDerived)
								{
									foreach (var derived in GetDerivedClasses((simple.Src as MgaReference).Referred))
									{
										yield return derived;
									}
								}
							}
						}
						else
						{
							yield return simple.Src;
							if (includeDerived)
							{
								foreach (var derived in GetDerivedClasses(simple.Src))
								{
									yield return derived;
								}
							}
						}
					}
				}
			}
		}

		public static IEnumerable<MgaFCO> GetSetMembers(
	MgaFCO mgaFCO,
	bool includeDerived = false)
		{
			Contract.Requires(mgaFCO != null);
			Contract.Requires(mgaFCO.MetaBase.Name == "Set");

			List<MgaFCO> baseReferences = GetBaseClasses(mgaFCO).
				Where(x => x.MetaBase.Name == "Set").ToList();

			baseReferences.Add(mgaFCO);
			List<MgaFCO> baseReferencesWProxies = baseReferences.ToList();
			baseReferences.ForEach(x => baseReferencesWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));

			foreach (MgaFCO item in baseReferences)
			{
				foreach (MgaConnPoint cp in item.PartOfConns)
				{
					MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
					if (simple.Dst == item &&
						simple.MetaBase.Name == "SetMembership")
					{
						if (simple.Src is MgaReference)
						{
							yield return (simple.Src as MgaReference).Referred;
							if (includeDerived)
							{
								if (includeDerived)
								{
									foreach (var derived in GetDerivedClasses((simple.Src as MgaReference).Referred))
									{
										yield return derived;
									}
								}
							}
						}
						else
						{
							yield return simple.Src;
							if (includeDerived)
							{
								foreach (var derived in GetDerivedClasses(simple.Src))
								{
									yield return derived;
								}
							}
						}
					}
				}
			}
		}

		public static IEnumerable<MgaFCO> GetMembersOfSets(
			MgaFCO mgaFCO,
			bool includeDerived = false)
		{
			Contract.Requires(mgaFCO != null);

			List<MgaFCO> baseReferences = GetBaseClasses(mgaFCO).ToList();

			baseReferences.Add(mgaFCO);
			List<MgaFCO> baseReferencesWProxies = baseReferences.ToList();
			baseReferences.ForEach(x => baseReferencesWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));

			foreach (MgaFCO item in baseReferences)
			{
				foreach (MgaConnPoint cp in item.PartOfConns)
				{
					MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
					if (simple.Src == item &&
						simple.MetaBase.Name == "SetMembership")
					{
						if (simple.Dst is MgaReference)
						{
							yield return (simple.Dst as MgaReference).Referred;
							if (includeDerived)
							{
								if (includeDerived)
								{
									foreach (var derived in GetDerivedClasses((simple.Dst as MgaReference).Referred))
									{
										yield return derived;
									}
								}
							}
						}
						else
						{
							yield return simple.Dst;
							if (includeDerived)
							{
								foreach (var derived in GetDerivedClasses(simple.Dst))
								{
									yield return derived;
								}
							}
						}
					}
				}
			}
		}


		// retrives with the object and the defined roles in the given container
		public static Dictionary<MgaFCO, List<string>> GetChildRoles(MgaFCO mgaFCO)
		{
			Contract.Requires(mgaFCO != null);
			Contract.Requires(mgaFCO.MetaBase.Name == "Model");

			// contains the element and the set of roles names (at least one)
			Dictionary<MgaFCO, List<string>> result = new Dictionary<MgaFCO, List<string>>();

			// (1) get base classes (models)
			List<MgaFCO> baseModels = GetBaseClasses(mgaFCO).Where(x => x.MetaBase.Name == "Model").ToList();
			baseModels.Add(mgaFCO);
			List<MgaFCO> baseModelsWProxies = baseModels.ToList();
			baseModels.ForEach(x => baseModelsWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));

			List<MgaFCO> containedItems = new List<MgaFCO>();

			List<string> resultItem = null;
			List<MgaFCO> derivedClasses = null;
			string role = "";
			bool self = false;

			// (2) get base classes self roles
			foreach (MgaFCO item in baseModelsWProxies)
			{
				foreach (MgaConnPoint cp in item.PartOfConns)
				{
					if (cp.ConnRole == "src")
					{
						continue;
					}
					MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
					if (simple.MetaRole.Name == "Containment")
					{
						containedItems.Clear();
						derivedClasses = GetDerivedClasses(item).ToList();
						derivedClasses.Add(item);

						role = simple.StrAttrByName["Rolename"];

						self = false;

						// self containments
						if (simple.Src is MgaReference == false &&
								simple.Dst is MgaReference == false)
						{
							if (simple.Src == item &&
									simple.Dst == item)
							{
								self = true;
								foreach (var derived in derivedClasses)
								{
									// model self containment
									if (result.TryGetValue(derived, out resultItem))
									{
										resultItem.Add(derived.Name + role);
									}
									else
									{
										resultItem = new List<string>();
										resultItem.Add(derived.Name + role);
										result.Add(derived, resultItem);
									}
								}
							}
						}
						else if (simple.Src is MgaReference &&
										 simple.Dst is MgaReference == false)
						{
							if ((simple.Src as MgaReference).Referred == item &&
									simple.Dst == item)
							{
								self = true;
								// model self containment
								foreach (var derived in derivedClasses)
								{
									// model self containment
									if (result.TryGetValue(derived, out resultItem))
									{
										resultItem.Add(derived.Name + role);
									}
									else
									{
										resultItem = new List<string>();
										resultItem.Add(derived.Name + role);
										result.Add(derived, resultItem);
									}
								}
							}
						}
						else if (simple.Src is MgaReference == false &&
										 simple.Dst is MgaReference)
						{
							if (simple.Src == item &&
									(simple.Dst as MgaReference).Referred == item)
							{
								self = true;
								// model self containment
								foreach (var derived in derivedClasses)
								{
									// model self containment
									if (result.TryGetValue(derived, out resultItem))
									{
										resultItem.Add(derived.Name + role);
									}
									else
									{
										resultItem = new List<string>();
										resultItem.Add(derived.Name + role);
										result.Add(derived, resultItem);
									}
								}
							}
						}
						else if (simple.Src is MgaReference &&
										 simple.Dst is MgaReference)
						{
							if ((simple.Src as MgaReference).Referred == item &&
									(simple.Dst as MgaReference).Referred == item)
							{
								self = true;
								// model self containment
								foreach (var derived in derivedClasses)
								{
									// model self containment
									if (result.TryGetValue(derived, out resultItem))
									{
										resultItem.Add(derived.Name + role);
									}
									else
									{
										resultItem = new List<string>();
										resultItem.Add(derived.Name + role);
										result.Add(derived, resultItem);
									}
								}
							}
						}

						if (self == false &&
							simple.Dst == item)
						{
							if (simple.Src is MgaReference)
							{
								derivedClasses = GetDerivedClasses((simple.Src as MgaReference).Referred).ToList();
								derivedClasses.Add((simple.Src as MgaReference).Referred);
							}
							else
							{
								derivedClasses = GetDerivedClasses(simple.Src).ToList();
								derivedClasses.Add(simple.Src);
							}
							if (derivedClasses.Count == 1)
							{
								// if there is no derived classes
								if (result.TryGetValue(derivedClasses.FirstOrDefault(), out resultItem))
								{
									if (string.IsNullOrEmpty(role))
									{
										resultItem.Add(derivedClasses.FirstOrDefault().Name);
									}
									else
									{
										resultItem.Add(role);
									}
								}
								else
								{
									resultItem = new List<string>();
									if (string.IsNullOrEmpty(role))
									{
										resultItem.Add(derivedClasses.FirstOrDefault().Name);
									}
									else
									{
										resultItem.Add(role);
									}
									result.Add(derivedClasses.FirstOrDefault(), resultItem);
								}
							}
							else
							{
								// if there is at least one derived class
								foreach (var derived in derivedClasses)
								{
									// model self containment
									if (result.TryGetValue(derived, out resultItem))
									{
										resultItem.Add(derived.Name + role);
									}
									else
									{
										resultItem = new List<string>();
										resultItem.Add(derived.Name + role);
										result.Add(derived, resultItem);
									}
								}
							}
						}

					}
				}
			}

			// (3) exclude abstract elements
			// (4) get all containment type of connections (except self)

			// (5) get base classes child elements w roles
			// (6) get this class's child elements w roles

			return result;
		}


		public static IEnumerable<string> GetRoles(MgaFCO mgaFCO)
		{
			IEnumerable<MgaSimpleConnection> simples = GetRolesConnections(mgaFCO).Distinct();
			bool hasDerived = GetDerivedClasses(mgaFCO).Where(x => x != mgaFCO).Count() > 0;
			string compositionName = "";

			foreach (var item in simples)
			{
				if (item.MetaBase.Name != "Containment")
				{
					throw new Exception();
				}

				compositionName = item.StrAttrByName["Rolename"];

				MgaFCO Src = item.Src;
				if (item.Src is MgaReference)
				{
					Src = (item.Src as MgaReference).Referred;
				}
				if (Src == mgaFCO)
				{
					// role defined directly
					if (hasDerived)
					{
						// role name = this name + composition name
						yield return mgaFCO.Name + compositionName;
					}
					else
					{
						if (string.IsNullOrEmpty(compositionName))
						{
							// role name = this name
							yield return mgaFCO.Name;
						}
						else
						{
							// role name = composition name
							yield return compositionName;
						}
					}
				}
				else if (item.Dst != mgaFCO)
				{
					// this is a derived class
					// role name = this name + composition name
					yield return mgaFCO.Name + compositionName;
				}
			}
		}


		public static IEnumerable<MgaSimpleConnection> GetRolesConnections(MgaFCO mgaFCO)
		{
			List<MgaFCO> baseClasses = GetBaseClasses(mgaFCO).ToList();
			baseClasses.Add(mgaFCO);


			List<MgaFCO> baseClassesWProxies = new List<MgaFCO>();


			baseClasses.ForEach(x => baseClassesWProxies.AddRange(x.ReferencedBy.Cast<MgaFCO>()));
			baseClassesWProxies.AddRange(baseClasses);

			//baseClasses.AddRange(GetBaseClasses(mgaFCO).Select(x => x.ReferencedBy).Cast<MgaFCO>());
			foreach (MgaFCO item in baseClassesWProxies)
			{
				foreach (MgaConnPoint cp in item.PartOfConns)
				{
					MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
					if (simple.MetaBase.Name == "Containment")
					{
						yield return simple;
					}
				}
			}
		}


		public static IEnumerable<Tuple<MgaFCO, string>> GetSrcConnections(MgaFCO mgaFCO)
		{
			//Contract.Requires<ArgumentNullException>(mgaFCO != null);

			foreach (MgaConnPoint cp in mgaFCO.PartOfConns)
			{
				MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
				if (simple != null)
				{
                    MgaFCO connector = null;
                    if (simple.MetaBase.Name == "ConnectorToDestination")
                    {
                        connector = simple.Src;
                    }
                    if (simple.MetaBase.Name == "SourceToConnector" &&
                        string.IsNullOrEmpty(simple.StrAttrByName["srcRolename"])) // bidirectional connection
                    {
                        connector = simple.Dst;
                    }
                    if (connector != null)
					{
						foreach (MgaConnPoint cpConnector in connector.PartOfConns)
						{
							MgaSimpleConnection simpleConn = cpConnector.Owner as MgaSimpleConnection;
							if (simpleConn != null)
							{
								if (simpleConn.MetaBase.Name == "AssociationClass")
								{
                                    MgaFCO target = simpleConn.Src == connector ? simpleConn.Dst : simpleConn.Src;
                                    if (target is MgaReference)
                                    {
                                        target = (target as MgaReference).Referred;
                                    }
                                    if (simple.Src == connector)
									{
                                        yield return new Tuple<MgaFCO, string>(target, "Src");
									}
									else // bidirectional connection
									{
                                        yield return new Tuple<MgaFCO, string>(target, "Dst");
                                    }
								}
							}
						}
					}
				}
			}
		}

		public static IEnumerable<Tuple<MgaFCO, string>> GetDstConnections(MgaFCO mgaFCO)
		{
			//Contract.Requires<ArgumentNullException>(mgaFCO != null);

			foreach (MgaConnPoint cp in mgaFCO.PartOfConns)
			{
				MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
				if (simple != null)
				{
                    MgaFCO connector = null;
                    if (simple.MetaBase.Name == "SourceToConnector")
                    {
                        connector = simple.Dst;
                    }
                    if (simple.MetaBase.Name == "ConnectorToDestination" &&
                        string.IsNullOrEmpty(simple.StrAttrByName["dstRolename"])) // bidirectional connection
                    {
                        connector = simple.Src;
                    }
                    if (connector != null)
					{
						foreach (MgaConnPoint cpConnector in connector.PartOfConns)
						{
							MgaSimpleConnection simpleConn = cpConnector.Owner as MgaSimpleConnection;
							if (simpleConn != null)
							{
								if (simpleConn.MetaBase.Name == "AssociationClass")
								{
                                    MgaFCO target = simpleConn.Src == connector ? simpleConn.Dst : simpleConn.Src;
									if (target is MgaReference)
									{
                                        target = (target as MgaReference).Referred;
                                    }
                                    if (simple.Dst == connector)
                                    {
                                        yield return new Tuple<MgaFCO, string>(target, "Dst");
									}
									else // bidirectional connection
									{
                                        yield return new Tuple<MgaFCO, string>(target, "Src");
                                    }
								}
							}
						}
					}
				}
			}
		}


		public static IEnumerable<MgaFCO> GetSrcEnd(
			MgaFCO mgaFCO,
			bool includeDerived = false)
		{
			//Contract.Requires(mgaFCO != null);

			foreach (MgaConnPoint cp in mgaFCO.PartOfConns)
			{
				MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
				if (simple != null)
				{
					if (simple.MetaBase.Name == "AssociationClass")
					{
						MgaFCO connector = null;
						if (simple.Src.MetaBase.Name == "Connector")
						{
							connector = simple.Src;
						}
						else
						{
							connector = simple.Dst;
						}
						foreach (MgaConnPoint cpConnector in connector.PartOfConns)
						{
							MgaSimpleConnection simpleConn = cpConnector.Owner as MgaSimpleConnection;
							if (simpleConn != null)
							{
                                if (simpleConn.MetaBase.Name == "SourceToConnector")
								{
									if (simpleConn.Src is MgaReference)
									{
										yield return (simpleConn.Src as MgaReference).Referred;

										if (includeDerived)
										{
											foreach (MgaFCO derivedClass in GetDerivedClasses((simpleConn.Src as MgaReference).Referred))
											{
												yield return derivedClass;
											}
										}
									}
									else
									{
										yield return simpleConn.Src;

										if (includeDerived)
										{
											foreach (MgaFCO derivedClass in GetDerivedClasses(simpleConn.Src))
											{
												yield return derivedClass;
											}
										}
									}
								}
                                else if (simpleConn.MetaBase.Name == "ConnectorToDestination" &&
                                         string.IsNullOrEmpty(simpleConn.StrAttrByName["dstRolename"]))
                                {
                                    if (simpleConn.Dst is MgaReference)
                                    {
                                        yield return (simpleConn.Dst as MgaReference).Referred;

                                        if (includeDerived)
                                        {
                                            foreach (MgaFCO derivedClass in GetDerivedClasses((simpleConn.Dst as MgaReference).Referred))
                                            {
                                                yield return derivedClass;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        yield return simpleConn.Dst;

                                        if (includeDerived)
                                        {
                                            foreach (MgaFCO derivedClass in GetDerivedClasses(simpleConn.Dst))
                                            {
                                                yield return derivedClass;
                                            }
                                        }
                                    }
                                }
							}
						}
					}
				}
			}
		}

		public static IEnumerable<MgaFCO> GetDstEnd(MgaFCO mgaFCO, bool includeDerived = false)
		{
			//Contract.Requires(mgaFCO != null);

			foreach (MgaConnPoint cp in mgaFCO.PartOfConns)
			{
				MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
				if (simple != null)
				{
					if (simple.MetaBase.Name == "AssociationClass")
					{
						MgaFCO connector = null;
						if (simple.Src.MetaBase.Name == "Connector")
						{
							connector = simple.Src;
						}
						else
						{
							connector = simple.Dst;
						}
						foreach (MgaConnPoint cpConnector in connector.PartOfConns)
						{
							MgaSimpleConnection simpleConn = cpConnector.Owner as MgaSimpleConnection;
							if (simpleConn != null)
							{
                                if (simpleConn.MetaBase.Name == "ConnectorToDestination")
								{
									if (simpleConn.Dst is MgaReference)
									{
										yield return (simpleConn.Dst as MgaReference).Referred;

										if (includeDerived)
										{
											foreach (MgaFCO derivedClass in GetDerivedClasses((simpleConn.Dst as MgaReference).Referred))
											{
												yield return derivedClass;
											}
										}
									}
									else
									{
										yield return simpleConn.Dst;

										if (includeDerived)
										{
											foreach (MgaFCO derivedClass in GetDerivedClasses(simpleConn.Dst))
											{
												yield return derivedClass;
											}
										}
									}
								}
                                else if (simpleConn.MetaBase.Name == "SourceToConnector" &&
                                         string.IsNullOrEmpty(simpleConn.StrAttrByName["srcRolename"]))
                                {
                                    if (simpleConn.Src is MgaReference)
                                    {
                                        yield return (simpleConn.Src as MgaReference).Referred;

                                        if (includeDerived)
                                        {
                                            foreach (MgaFCO derivedClass in GetDerivedClasses((simpleConn.Src as MgaReference).Referred))
                                            {
                                                yield return derivedClass;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        yield return simpleConn.Src;

                                        if (includeDerived)
                                        {
                                            foreach (MgaFCO derivedClass in GetDerivedClasses(simpleConn.Src))
                                            {
                                                yield return derivedClass;
                                            }
                                        }
                                    }
                                }
							}
						}
					}
				}
			}
		}

		public static IEnumerable<MgaFCO> GetParents(MgaFCO mgaFCO, bool includeDerived = false)
		{
			List<MgaFCO> baseClasses = GetBaseClasses(mgaFCO).ToList();
			baseClasses.Add(mgaFCO);

			List<MgaFCO> parentDerivedClasses = new List<MgaFCO>();

			foreach (MgaFCO baseFco in baseClasses)
			{
				foreach (MgaFCO fco in baseFco.ReferencedBy)
				{
					foreach (MgaConnPoint cp in fco.PartOfConns)
					{
						MgaSimpleConnection conn = cp.Owner as MgaSimpleConnection;
						if (conn.Meta.Name == "Containment" ||
							conn.Meta.Name == "FolderContainment")
						{
							if (conn.Src == fco)
							{
								if (conn.Dst is MgaReference)
								{
									yield return (conn.Dst as MgaReference).Referred;

									if (includeDerived)
									{
										parentDerivedClasses = GetDerivedClasses((conn.Dst as MgaReference).Referred).ToList();
										foreach (var item in parentDerivedClasses)
										{
											yield return item;
										}
									}
								}
								else
								{
									yield return conn.Dst;

									if (includeDerived)
									{
										parentDerivedClasses = GetDerivedClasses(conn.Dst).ToList();
										foreach (var item in parentDerivedClasses)
										{
											yield return item;
										}
									}
								}
							}
						}
					}
				}

				foreach (MgaConnPoint cp in baseFco.PartOfConns)
				{
					MgaSimpleConnection conn = cp.Owner as MgaSimpleConnection;
					if (conn.Meta.Name == "Containment" ||
						conn.Meta.Name == "FolderContainment")
					{
						if (conn.Src == baseFco)
						{
							if (conn.Dst is MgaReference)
							{
								yield return (conn.Dst as MgaReference).Referred;

								if (includeDerived)
								{
									parentDerivedClasses = GetDerivedClasses((conn.Dst as MgaReference).Referred).ToList();
									foreach (var item in parentDerivedClasses)
									{
										yield return item;
									}
								}
							}
							else
							{
								yield return conn.Dst;

								if (includeDerived)
								{
									parentDerivedClasses = GetDerivedClasses(conn.Dst).ToList();
									foreach (var item in parentDerivedClasses)
									{
										yield return item;
									}
								}
							}
						}
					}
				}
			}
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="mgaFCO"></param>
		/// <returns></returns>
		public static bool IsInRootFolder(MgaFCO mgaFCO)
		{
			// TODO reimplement this function, what happens if the base class has the inrootfolder = true
			if (mgaFCO.Attributes.Cast<MgaAttribute>().FirstOrDefault(x => x.Meta.Name == "InRootFolder") != null)
			{
				return mgaFCO.BoolAttrByName["InRootFolder"];
			}
			else
			{
				return false;
			}
		}

		/// <summary>
		/// Does not contain the root folder as parent
		/// </summary>
		/// <param name="mgaObject"></param>
		/// <returns></returns>
		public static IEnumerable<MgaFCO> GetParentFolders(MgaFCO mgaObject)
		{
			return GetParents(mgaObject, true).Where(x => x.MetaBase.Name == "Folder");
		}

		public static IEnumerable<MgaFCO> GetParentModels(MgaFCO mgaObject)
		{
			return GetParents(mgaObject, true).Where(x => x.MetaBase.Name == "Model");
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="mgaFCO">Do NOT pass proxy as an input.</param>
		/// <returns>Does NOT contain the passed object.</returns>
		public static IEnumerable<MgaFCO> GetBaseClasses(MgaFCO mgaFCO)
		{
			Contract.Requires(mgaFCO != null);
			// TODO: try to avoid infinite loop (obj -> obj2 -> ref to obj)
			List<MgaFCO> result = new List<MgaFCO>();

			foreach (MgaFCO refs in mgaFCO.ReferencedBy)
			{
				result.AddRange(GetBaseClasses(refs));
			}

			foreach (MgaConnPoint cp in mgaFCO.PartOfConns)
			{
				MgaSimpleConnection conn = cp.Owner as MgaSimpleConnection;
				if (conn.Meta.Name == "DerivedInheritance")
				{
					foreach (MgaConnPoint baseCp in conn.Src.PartOfConns)
					{
						MgaSimpleConnection baseConn = baseCp.Owner as MgaSimpleConnection;
						if (baseConn.Meta.Name == "BaseInheritance")
						{

							if (baseConn.Src is MgaReference)
							{
								result.Add((baseConn.Src as MgaReference).Referred);
								result.AddRange(GetBaseClasses((baseConn.Src as MgaReference).Referred));
							}
							else
							{
								result.Add(baseConn.Src);
								result.AddRange(GetBaseClasses(baseConn.Src));
							}
						}
					}
				}
			}
			// eliminate proxies
			return result.Where(x => (x is MgaReference) == false).Distinct();
		}

		public static Dictionary<MgaFCO, List<MgaFCO>> derivedClassCache =
			new Dictionary<MgaFCO,List<MgaFCO>>();

		public static int fromCache = 0;
		public static int fromNotCache = 0;

		public static IEnumerable<MgaFCO> GetDerivedClasses(MgaFCO mgaFCO)
		{
			// TODO: try to avoid infinite loop (obj -> obj2 -> ref to obj)
			List<MgaFCO> result = null;

			if (derivedClassCache.TryGetValue(mgaFCO, out result))
			{
				fromCache++;
				return result;
			}
			else
			{
				fromNotCache++;
				result = new List<MgaFCO>();
			}

			foreach (MgaFCO refs in mgaFCO.ReferencedBy)
			{
				result.AddRange(GetDerivedClasses(refs));
			}

			foreach (MgaConnPoint cp in mgaFCO.PartOfConns)
			{
				MgaSimpleConnection conn = cp.Owner as MgaSimpleConnection;
				if (conn.Meta.Name == "BaseInheritance")
				{
					foreach (MgaConnPoint baseCp in conn.Dst.PartOfConns)
					{
						MgaSimpleConnection baseConn = baseCp.Owner as MgaSimpleConnection;
						if (baseConn.Meta.Name == "DerivedInheritance")
						{

							if (baseConn.Dst is MgaReference)
							{
								result.Add((baseConn.Dst as MgaReference).Referred);
								result.AddRange(GetDerivedClasses((baseConn.Dst as MgaReference).Referred));
							}
							else
							{
								result.Add(baseConn.Dst);
								result.AddRange(GetDerivedClasses(baseConn.Dst));
							}
						}
					}
				}
			}

			derivedClassCache.Add(mgaFCO, result.Where(x => (x is MgaReference) == false).ToList());

			// exclude proxies
			return result.Where(x => (x is MgaReference) == false);
		}

		public IEnumerable<MgaFCO> GetChildren(MgaObject subject)
		{
			Contract.Requires((subject is MgaReference) == false);

			List<MgaFCO> children = new List<MgaFCO>();

			if (subject.MetaBase.Name == "RootFolder")
			{
				// get root folder objects
				MgaFilter f = subject.Project.CreateFilter();

				foreach (MgaFCO fco in subject.Project.AllFCOs(f))
				{
					foreach (MgaAttribute attr in fco.Attributes)
					{
						if (attr.Meta.Name == "InRootFolder")
						{
							if (fco.BoolAttrByName["InRootFolder"] == true)
							{
								children.Add(fco);
							}
						}
					}
				}
			}
			else
			{
				List<MgaFCO> baseClasses = GetBaseClasses(subject as MgaFCO).ToList();
				baseClasses.Add(subject as MgaFCO);

				// folder or model
				foreach (MgaFCO baseFco in baseClasses)
				{
					foreach (MgaFCO fco in baseFco.ReferencedBy)
					{
						foreach (MgaConnPoint cp in fco.PartOfConns)
						{
							MgaSimpleConnection conn = cp.Owner as MgaSimpleConnection;
							// compare the dst with the proxy
							if (conn.Dst == fco)
							{
								if (conn.Meta.Name == "Containment" ||
									conn.Meta.Name == "FolderContainment")
								{
									if (conn.Src is MgaReference)
									{
										children.Add((conn.Src as MgaReference).Referred);
									}
									else
									{
										children.Add(conn.Src);
									}
								}
							}
						}
					}

					foreach (MgaConnPoint cp in baseFco.PartOfConns)
					{
						MgaSimpleConnection conn = cp.Owner as MgaSimpleConnection;
						if (conn.Meta.Name == "Containment" ||
							conn.Meta.Name == "FolderContainment")
						{
							if (conn.Dst == baseFco)
							{
								if (conn.Src is MgaReference)
								{
									children.Add((conn.Src as MgaReference).Referred);
								}
								else
								{
									children.Add(conn.Src);
								}
							}
						}
					}
				}
			}
			return children.Distinct();
		}

		#endregion
	}
}
