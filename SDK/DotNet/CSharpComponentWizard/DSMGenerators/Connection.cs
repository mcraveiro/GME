using System;
using System.Collections.Generic;
using System.Text;

namespace DSM.Generators
{
    public class Connection : FCO
    {
        new public class Template
        {
            public static readonly string Connect =
@"
	    public void Connect({0} obj1, {1} obj2)
        {{
            if (this.mgaObject.ParentModel != null)
            {{
                IMgaModel parent = this.mgaObject.ParentModel;
                foreach (MgaMetaRole role in (parent.MetaBase as IMgaMetaModel).Roles)
                {{
                    if (role.kind == mgaMetaObject)
                    {{
                        this.mgaObject.DestroyObject();
                        this.mgaObject = parent.CreateSimpleConn(role, obj1.MgaObject as MgaFCO, obj2.MgaObject as MgaFCO, null, null) as IMgaConnection;
                        return;
                    }}
                }}
            }}
        }}
";
            public static readonly string ConnectInterface =
@"
	    void Connect({0} obj1, {1} obj2);
";
            #region Endpoint
            public static readonly string EndPointInterface =
@"
	    public {0} {1} { get; };
";
            public static readonly string EndPointInner =
@"
                   if (target.Meta.Name == ""{0}"")
                      return new {2}(target as {3});
";
            public static readonly string EndPoint =
@"
        public {0} {1}
        {{
            get
            {{
                IMgaFCO target = (mgaObject as IMgaSimpleConnection).{3};
{2}

                return null;
            }}
        }}
";
            #endregion
        }

        public Connection(GME.MGA.IMgaAtom mgaObject)
            : base(mgaObject)
        {
            className = mgaObject.Name;
            baseInterfaceName = "IConnection";

            memberType = "IMgaConnection";
        }

        public override string GenerateClass()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(Init());

            List<string> rels = new List<string>();
            List<string> attrs = new List<string>();
            List<string> conns = new List<string>();
            List<string> crnews = new List<string>();
            List<ConnEndPoints> cons = new List<ConnEndPoints>();

            StringBuilder sbAttrib = new StringBuilder();
            StringBuilder sbConns = new StringBuilder();
            StringBuilder sbRels = new StringBuilder();
            StringBuilder sbCons = new StringBuilder();
            StringBuilder sbEnds = new StringBuilder();

            string baseInterfaces = (this.HasChildren) ? className : baseInterfaceName;

            foreach (DerivedWithKind parent in this.Parents)
            {
                if (parent.Type != DerivedWithKind.InhType.Implementation)
                    baseInterfaces = baseInterfaces + ", " + parent.Rel.Name;
            } 

            sb.AppendFormat(
                FCO.Template.Class,
                namespaceName,
                (this.HasChildren) ? className + "Impl" : className,
                baseInterfaces,
                memberType,
                GenerateCommon() + GenerateContainer(),
                GenerateAttributes(ref attrs, ref sbAttrib),
                GenerateConnections(ref conns, ref sbConns),
                GenerateRelationships(ref rels, ref sbRels),
                GenerateConnects(ref cons, ref sbCons),
                "IMgaMeta" + memberType.Substring(4),
                className,
                GenerateCreateNews(ref crnews,this) + GenerateEndPoints(ref sbEnds));

            baseInterfaces = baseInterfaceName;

            foreach (DerivedWithKind parent in this.Parents)
            {
                if (parent.Type != DerivedWithKind.InhType.Implementation)
                    baseInterfaces = baseInterfaces + ", " + parent.Rel.Name;
            } 

            if (this.HasChildren)
            {
                //have to generate interface as well
                if (this.HasChildren)
                {
                    //have to generate interface as well
                    sb.AppendFormat(
                    FCO.Template.Interface,
                    namespaceName,
                    className,
                    baseInterfaces,
                    memberType,
                    GenerateCommon(),
                    sbAttrib.ToString(),
                    sbConns.ToString(),
                    sbRels.ToString(),
                    sbCons.ToString()+sbEnds.ToString());
                }
            }

            return sb.ToString();
        }

        //public string GenerateEndpoints()
        //{
        //    StringBuilder sb = new StringBuilder();
        //    //sb.Append(getSrc());
        //    //sb.Append(getDst());
        //    return sb.ToString();
        //}

        public string GenerateEndPoints(ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(generateOwnEndPoints(ref forInterface));

            //genarate parents' attributes:
            //foreach (DerivedWithKind parent in this.Parents)
            //{
            //    if (parent.Rel is Model)
            //    {
            //        if (parent.Type == DerivedWithKind.InhType.General ||
            //            parent.Type == DerivedWithKind.InhType.Implementation)
            //            sb.Append((parent.Rel as Model).GenerateContainments(ref names, ref forInterface));
            //    }
            //}

            return sb.ToString();
        }
        private string generateOwnEndPoints(ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            foreach (ConnEndPoints connends in Ends)
            {
                    sb.Append(generateCurrentEndPoints(connends));
                    if (this.HasChildren)
                    {
                        forInterface.Append(generateCurrentEndPointsForInterface(connends));
                    }
                
                break;
            }
            return sb.ToString();
        }
        private string generateCurrentEndPoints(ConnEndPoints current)
        {
            
            StringBuilder sb = new StringBuilder();
            {
                StringBuilder inner = new StringBuilder();
                inner.AppendFormat(Connection.Template.EndPointInner, current.src.className, "src"/*Is it okay?*/, current.src.ProperClassName, current.src.memberType);
                if (current.src.HasChildren)
                {
                    //and add all of the children
                    foreach (DerivedWithKind child in current.src.ChildrenRecursive)
                    {
                        if (child.Type == DerivedWithKind.InhType.General)
                            inner.AppendFormat(Connection.Template.EndPointInner, child.Rel.className, "src", child.Rel.ProperClassName, child.Rel.memberType);
                    }
                }

                sb.AppendFormat(Connection.Template.EndPoint, current.src.className, "Src", inner.ToString(), "src");
            }
            {
                StringBuilder inner = new StringBuilder();
                inner.AppendFormat(Connection.Template.EndPointInner, current.dst.className, "dst"/*Is it okay?*/, current.dst.ProperClassName, current.dst.memberType);
                if (current.dst.HasChildren)
                {
                    //and add all of the children
                    foreach (DerivedWithKind child in current.dst.ChildrenRecursive)
                    {
                        if (child.Type == DerivedWithKind.InhType.General)
                            inner.AppendFormat(Connection.Template.EndPointInner, child.Rel.className, "dst", child.Rel.ProperClassName, child.Rel.memberType);
                    }
                }
                sb.AppendFormat(Connection.Template.EndPoint, current.dst.className, "Dst", inner.ToString(), "dst");
            }
                        
            return sb.ToString();
        }
        private string generateCurrentEndPointsForInterface(ConnEndPoints current)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Connection.Template.EndPointInterface, current.src.className, "Src");
            sb.AppendFormat(Connection.Template.EndPointInterface, current.dst.className, "Dst");

            return sb.ToString();
        }
        
        #region Connect
        public class ConnEndPoints
        {
            public FCO src = null;
            public FCO dst = null;

            public ConnEndPoints()
            {
            }

            public ConnEndPoints(FCO src, FCO dst)
            {
                this.src = src;
                this.dst = dst;
            }

            public override bool Equals(object obj)
            {
                if (obj == null) return false;

                if (this.GetType() != obj.GetType()) return false;

                ConnEndPoints other = (ConnEndPoints)obj;

                if (!object.Equals(src, other.src)) return false;

                if (!object.Equals(dst, other.dst)) return false;

                return true;
            }

            public override int GetHashCode()
            {
                return src.GetHashCode() + dst.GetHashCode();
            }
        }

        private IEnumerable<ConnEndPoints> Ends
        {
            get
            {
                foreach (GME.MGA.IMgaConnPoint conn in mgaObject.PartOfConns)
                {
                    if (conn.Owner.Meta.Name == "AssociationClass")
                    {
                        foreach (GME.MGA.IMgaConnPoint connOther in conn.Owner.ConnPoints)
                        {
                            if (connOther.Target.ID != mgaObject.ID)
                            {
                                //connOther.target: Connector
                                ConnEndPoints ends = new ConnEndPoints();
                                foreach (GME.MGA.IMgaConnPoint conn2 in connOther.Target.PartOfConns)
                                {   
                                    if (conn2.Owner.Meta.Name == "SourceToConnector")
                                    {
                                        foreach (GME.MGA.IMgaConnPoint connOther2 in conn2.Owner.ConnPoints)
                                        {
                                            if (connOther2.Target.ID != connOther.Target.ID)
                                            {
                                                //connOther2.target: Connection
                                                if (connOther2.Target.MetaBase.Name.Contains("Proxy"))
                                                {
                                                    if (Object.ProxyCache.ContainsKey(connOther2.Target.Name))
                                                        ends.src = Object.ElementsByName[Object.ProxyCache[connOther2.Target.Name]] as FCO;                                                            
                                                    else
                                                        DSM.GeneratorFacade.Errors.Add("Proxy '" + connOther2.Target.Name + "' is not found");
                                                }
                                                else
                                                {
                                                    if (Object.ElementsByName.ContainsKey(connOther2.Target.Name))
                                                    {
                                                        ends.src = Object.ElementsByName[connOther2.Target.Name] as FCO;
                                                    }
                                                    else
                                                        DSM.GeneratorFacade.Errors.Add(connOther2.Target.Name + " is not found");
                                                }
                                            }
                                        }
                                    }
                                    else if (conn2.Owner.Meta.Name == "ConnectorToDestination")
                                    {
                                        foreach (GME.MGA.IMgaConnPoint connOther2 in conn2.Owner.ConnPoints)
                                        {
                                            if (connOther2.Target.ID != connOther.Target.ID)
                                            {
                                                //connOther2.Target: Connection
                                                if (connOther2.Target.MetaBase.Name.Contains("Proxy"))
                                                {
                                                    if (Object.ProxyCache.ContainsKey(connOther2.Target.Name))
                                                        ends.dst = Object.ElementsByName[Object.ProxyCache[connOther2.Target.Name]] as FCO;
                                                    else
                                                        DSM.GeneratorFacade.Errors.Add("Proxy '" + connOther2.Target.Name + "' is not found");
                                                }
                                                else
                                                {
                                                    if (Object.ElementsByName.ContainsKey(connOther2.Target.Name))
                                                    {
                                                        ends.dst = Object.ElementsByName[connOther2.Target.Name] as FCO;
                                                    }
                                                    else
                                                        DSM.GeneratorFacade.Errors.Add(connOther2.Target.Name + " is not found");
                                                }
                                            }
                                        }
                                    }                                    
                                }
                                if (ends.src != null && ends.dst != null)
                                    yield return ends;
                            }
                        }
                    }
                }
            }
        }
        public string GenerateConnects(ref List<ConnEndPoints> alreadyGenerated, ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(generateOwnConnects(ref alreadyGenerated, ref forInterface));

            //genarate parents' stuff:
            foreach (DerivedWithKind parent in this.Parents)
            {
                if (parent.Rel is Connection)
                {
                    if (parent.Type == DerivedWithKind.InhType.General ||
                        parent.Type == DerivedWithKind.InhType.Interface)
                        sb.Append((parent.Rel as Connection).GenerateConnects(ref alreadyGenerated, ref forInterface));
                }
            }

            return sb.ToString();
        }
        private string generateOwnConnects(ref List<ConnEndPoints> alreadyGenerated, ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            foreach (ConnEndPoints end in Ends)
            {
                if (!alreadyGenerated.Contains(end))
                {
                    sb.Append(generateConnect(end));
                    if (this.HasChildren)
                    {
                        forInterface.Append(generateConnectForInterface(end));
                    }
                    alreadyGenerated.Add(end);
                }
            }
            return sb.ToString();
        }
        private string generateConnect(ConnEndPoints current)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Connection.Template.Connect,
                current.src.Name,
                current.dst.Name);

            return sb.ToString();
        }
        private string generateConnectForInterface(ConnEndPoints current)
        {
            StringBuilder sb = new StringBuilder();
            //sb.AppendFormat(Connection.Template.ConnectInterface, 
            //    current.src.Name, 
            //    current.dst.Name);

            return sb.ToString();
        }
        #endregion
        //private string getSrc()
        //{
        //    FCO source = null;
        //    string rolename = "";
        //    foreach (MGALib.IMgaConnPoint conn in mgaObject.PartOfConns)
        //    {
        //        if (conn.Owner.Meta.Name == "AssociationClass")//"ConnectorToDestination" || conn.Owner.Meta.Name == "SourceToConnector")
        //        {
        //            foreach (MGALib.IMgaConnPoint connOther in conn.Owner.ConnPoints)
        //            {
        //                if (connOther.Target.ID != mgaObject.ID)
        //                {
        //                    //connOther.Target: Connector
        //                    foreach (MGALib.IMgaConnPoint conn2 in connOther.Target.PartOfConns)
        //                    {
        //                        if (conn2.Owner.Meta.Name == "SourceToConnector")
        //                        {
        //                            foreach (MGALib.IMgaConnPoint connOther2 in conn2.Owner.ConnPoints)
        //                            {
        //                                if (connOther2.Target.ID != connOther.Target.ID)
        //                                {
        //                                    //connOther2.Target: Connection
        //                                    if (Object.ElementsByName.ContainsKey(connOther2.Target.Name))
        //                                    {
        //                                        rolename = conn2.Owner.get_StrAttrByName("srcRolename");
        //                                        source = Object.ElementsByName[connOther2.Target.Name] as FCO;
        //                                    }
        //                                    else
        //                                        throw new Exception(connOther2.Target.Name + " is not cached");
        //                                }
        //                            }
        //                        }
        //                    }
        //                }
        //            }
        //        }
        //    }

        //    if (source == null)
        //        return "";

        //    if (rolename == "")
        //        rolename = "Src";

        //    StringBuilder sb = new StringBuilder();
        //    sb.AppendFormat(Connection.Template.EndPoint, source.className, rolename, source.memberType);
        //    return sb.ToString();
        //}
        //private string getDst()
        //{
        //    FCO dest = null;
        //    string rolename = "";
        //    foreach (MGALib.IMgaConnPoint conn in mgaObject.PartOfConns)
        //    {
        //        if (conn.Owner.Meta.Name == "AssociationClass")//"ConnectorToDestination" || conn.Owner.Meta.Name == "SourceToConnector")
        //        {
        //            foreach (MGALib.IMgaConnPoint connOther in conn.Owner.ConnPoints)
        //            {
        //                if (connOther.Target.ID != mgaObject.ID)
        //                {
        //                    //connOther.Target: Connector
        //                    foreach (MGALib.IMgaConnPoint conn2 in connOther.Target.PartOfConns)
        //                    {
        //                        if (conn2.Owner.Meta.Name == "ConnectorToDestination")
        //                        {
        //                            foreach (MGALib.IMgaConnPoint connOther2 in conn2.Owner.ConnPoints)
        //                            {
        //                                if (connOther2.Target.ID != connOther.Target.ID)
        //                                {
        //                                    //connOther2.Target: Connection
        //                                    if (Object.ElementsByName.ContainsKey(connOther2.Target.Name))
        //                                    {
        //                                        rolename = conn2.Owner.get_StrAttrByName("dstRolename");
        //                                        dest = Object.ElementsByName[connOther2.Target.Name] as FCO;
        //                                    }
        //                                    else
        //                                        throw new Exception(connOther2.Target.Name + " is not cached");
        //                                }
        //                            }
        //                        }
        //                    }
        //                }
        //            }
        //        }
        //    }

        //    if (dest == null)
        //        return "";

        //    if (rolename == "")
        //        rolename = "Dst";

        //    StringBuilder sb = new StringBuilder();
        //    sb.AppendFormat(Connection.Template.EndPoint, dest.className, rolename, dest.memberType);
        //    return sb.ToString();
        //}
    }
}
