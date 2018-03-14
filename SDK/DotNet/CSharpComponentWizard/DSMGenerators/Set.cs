using System;
using System.Collections.Generic;
using System.Text;

namespace DSM.Generators
{
    public class Set : FCO
    {
        new public class Template
        {
            public static readonly string SetMember =
@"
        public void AddMember{0}({0} member)
        {{
            this.mgaObject.AddMember(member.MgaObject as MgaFCO);
        }}

        public IEnumerable<{0}> Member{0}s
        {{
            get
            {{
                MgaFCOs children = mgaObject.Members;
                foreach (MgaFCO o in children)
                {{
{1}
                }}
            }}
        }}
";
            public static readonly string SetMemberInner =
@"
                    if (o.MetaBase.Name == ""{0}"")
                        yield return new {1}(o as {2});
";
            public static readonly string SetMemberInterface =
@"
        void AddMember{0}s({0} member);
        IEnumerable<{0}> Member{0}s {{ get; }}
";
        }

        public Set(GME.MGA.IMgaAtom mgaObject)
            : base(mgaObject)
        {
            className = mgaObject.Name;
            baseInterfaceName = "ISet";

            memberType = "IMgaSet";
        }


        #region SetMembers
        protected IEnumerable<FCO> Members
        {
            get
            {
                foreach (GME.MGA.IMgaObject mgaObject in this.MgaObjects)
                {
                    GME.MGA.IMgaFCO fco = mgaObject as GME.MGA.IMgaFCO;
                    foreach (GME.MGA.IMgaConnPoint conn in fco.PartOfConns)
                    {
                        if (conn.Owner.Meta.Name == "SetMembership" && conn.ConnRole == "dst")
                        {
                            foreach (GME.MGA.IMgaConnPoint connOther in conn.Owner.ConnPoints)
                            {
                                if (connOther.ConnRole == "src")
                                {
                                    //connOther.target: contained
                                    if (connOther.Target.MetaBase.Name.Contains("Proxy"))
                                    {
                                        if (Object.ProxyCache.ContainsKey(connOther.Target.Name))
                                            yield return Object.ElementsByName[Object.ProxyCache[connOther.Target.Name]] as FCO;
                                        else
                                            DSM.GeneratorFacade.Errors.Add("Proxy '" + connOther.Target.Name + "' is not found");
                                    }
                                    else
                                    {
                                        if (Object.ElementsByName.ContainsKey(connOther.Target.Name))
                                            yield return Object.ElementsByName[connOther.Target.Name] as FCO;
                                        else
                                        {
                                            //todo
                                            DSM.GeneratorFacade.Errors.Add(connOther.Target.Name + " is not found");
                                            //throw new Exception(connOther.target.Name + " is not cached");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        public string GenerateMembers(ref List<string> names, ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(generateOwnMembers(ref names, ref forInterface));

            //genarate parents' attributes:
            foreach (DerivedWithKind parent in this.Parents)
            {
                if (parent.Rel is Set)
                {
                    if (parent.Type == DerivedWithKind.InhType.General ||
                        parent.Type == DerivedWithKind.InhType.Implementation)
                        sb.Append((parent.Rel as Set).GenerateMembers(ref names, ref forInterface));
                }
            }

            return sb.ToString();
        }
        private string generateOwnMembers(ref List<string> names, ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            foreach (FCO cont in Members)
            {
                if (!names.Contains(cont.className))
                {
                    sb.Append(generateMember(cont));//cont.className, cont.memberType));
                    if (this.HasChildren)
                    {
                        forInterface.Append(generateMemberForInterface(cont));//.className));
                    }
                    names.Add(cont.className);
                }
            }
            return sb.ToString();
        }
        private string generateMember(FCO current)
        {
            StringBuilder inner = new StringBuilder();

            //FCO current = Object.ElementsByName[typename] as FCO;

            inner.AppendFormat(Set.Template.SetMemberInner, current.className, current.ProperClassName, current.memberType);
            if (current.HasChildren)
            {
                //and add all of the children
                foreach (DerivedWithKind child in current.ChildrenRecursive)
                {
                    if (child.Type == DerivedWithKind.InhType.General ||
                        child.Type == DerivedWithKind.InhType.Interface)
                        inner.AppendFormat(Set.Template.SetMemberInner, child.Rel.className, child.Rel.ProperClassName, child.Rel.memberType);
                }
            }

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Set.Template.SetMember, current.className, inner.ToString());

            return sb.ToString();
        }
        private string generateMemberForInterface(FCO current)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Set.Template.SetMemberInterface, current.className);

            return sb.ToString();
        }
        #endregion

        public override string GenerateClass()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(Init());

            List<string> rels = new List<string>();
            List<string> attrs = new List<string>();
            List<string> conns = new List<string>();
            List<string> membs = new List<string>();
            List<string> crnews = new List<string>();

            StringBuilder sbAttrib = new StringBuilder();
            StringBuilder sbMemb = new StringBuilder();
            StringBuilder sbConns = new StringBuilder();
            StringBuilder sbRels = new StringBuilder();

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
                GenerateMembers(ref membs, ref sbMemb),
                "IMgaMeta" + memberType.Substring(4),
                className,
                GenerateCreateNews(ref crnews, this));

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
                    sbMemb.ToString());
                }
            }

            return sb.ToString();
        }
    }
}
