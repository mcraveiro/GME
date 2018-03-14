using System;
using System.Collections.Generic;
using System.Text;

namespace DSM.Generators
{
    public class Reference : FCO
    {
        new public class Template
        {
            public static readonly string Reference =
@"
        public {0} Referenced{0}
        {{
            get
            {{
{1}
                return null;
            }}
            set
            {{
                mgaObject.Referred = value.mgaObject as MgaFCO;
            }}
        }}
";

            public static readonly string ReferenceInner =
@"
                if (mgaObject.Referred.MetaBase.Name == ""{0}"")
                    return new {1}(mgaObject.Referred as {2});
";

            public static readonly string ReferenceInterface =
@"
        {0} Referenced{0} {{ get; set; }}
";
        }


        public Reference(GME.MGA.IMgaAtom mgaObject)
            : base(mgaObject)
        {
            className = mgaObject.Name;
            baseInterfaceName = "IReference";

            memberType = "IMgaReference";
        }

        #region Reference
        protected IEnumerable<FCO> References
        {
            get
            {
                foreach (GME.MGA.IMgaObject mgaObject in this.MgaObjects)
                {
                    GME.MGA.IMgaFCO fco = mgaObject as GME.MGA.IMgaFCO;
                    foreach (GME.MGA.IMgaConnPoint conn in fco.PartOfConns)
                    {
                        if (conn.Owner.Meta.Name == "ReferTo" && conn.ConnRole == "src")
                        {
                            foreach (GME.MGA.IMgaConnPoint connOther in conn.Owner.ConnPoints)
                            {
                                if (connOther.ConnRole == "dst")
                                {
                                    //connOther.target: ref
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

        public string GenerateReferences(ref List<string> names, ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(generateOwnReferences(ref names, ref forInterface));

            //genarate parents' References:
            foreach (DerivedWithKind parent in this.Parents)
            {
                if (parent.Rel is Reference)
                {
                    if (parent.Type == DerivedWithKind.InhType.General ||
                        parent.Type == DerivedWithKind.InhType.Implementation)
                        sb.Append((parent.Rel as Reference).GenerateReferences(ref names, ref forInterface));
                }
            }

            return sb.ToString();
        }
        private string generateOwnReferences(ref List<string> names, ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            foreach (FCO ref1 in References)
            {
                if (!names.Contains(ref1.className))
                {
                    sb.Append(generateReference(ref1));
                    if (this.HasChildren)
                    {
                        forInterface.Append(generateReferenceForInterface(ref1.className));
                    }
                    names.Add(ref1.className);
                }
            }
            return sb.ToString();
        }
        private string generateReferenceForInterface(string typename)
        {
            //addCheckConnectionName(typename);

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Reference.Template.ReferenceInterface, typename);

            return sb.ToString();
        }
        private string generateReference(FCO current)
        {
            StringBuilder inner = new StringBuilder();

            //FCO current = Object.ElementsByName[typename] as FCO;

            inner.AppendFormat(Reference.Template.ReferenceInner, current.className, current.ProperClassName, current.memberType);
            if (current.HasChildren)
            {
                //and add all of the children
                foreach (DerivedWithKind child in current.ChildrenRecursive)
                {
                    if (child.Type == DerivedWithKind.InhType.General ||
                        child.Type == DerivedWithKind.InhType.Interface)
                        inner.AppendFormat(Reference.Template.ReferenceInner, child.Rel.className, child.Rel.ProperClassName, child.Rel.memberType);
                }
            }

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Reference.Template.Reference, current.className, inner.ToString());

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
            List<string> refs = new List<string>();
            List<string> crnews = new List<string>();

            StringBuilder sbAttrib = new StringBuilder();
            StringBuilder sbRefs = new StringBuilder();
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
                GenerateReferences(ref refs, ref sbRefs),
                "IMgaMeta" + memberType.Substring(4),
                className,
                GenerateCreateNews(ref crnews,this));

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
                    sbRefs.ToString());
                }
            }

            return sb.ToString();
        }
    }
}
