using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace DSM.Generators
{
    public class Model : FCO
    {
        new public class Template
        {
            public static readonly string Containment =
@"
        public IEnumerable<{0}> Contained{0}s
        {{
            get
            {{
                MgaObjects children = mgaObject.ChildObjects;
                foreach (MgaObject o in children)
                {{
{1}
                }}
            }}
        }}
";

            public static readonly string ContainmentInner =
@"
                    if (o.MetaBase.Name == ""{0}"")
                        yield return new {1}(o as {2});
";

            public static readonly string ContainmentInterface =
@"
        IEnumerable<{0}> Contained{0}s {{ get; }}
";
        }

        public Model(GME.MGA.IMgaAtom mgaObject)
            : base(mgaObject)
        {
            className = mgaObject.Name;
            baseInterfaceName = "IModel";

            memberType = "IMgaModel";
        }


        #region Containment
        protected IEnumerable<FCO> Contained
        {
            get
            {
                foreach (GME.MGA.IMgaObject mgaObject in this.MgaObjects)
                {
                    GME.MGA.IMgaFCO fco = mgaObject as GME.MGA.IMgaFCO;
                    foreach (GME.MGA.IMgaConnPoint conn in fco.PartOfConns)
                    {
                        if (conn.Owner.Meta.Name == "Containment" && conn.ConnRole == "dst")
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

        public string GenerateContainments(ref List<string> names, ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(generateOwnContainments(ref names, ref forInterface));

            //genarate parents' attributes:
            foreach (DerivedWithKind parent in this.Parents)
            {
                if (parent.Rel is Model)
                {
                    if (parent.Type == DerivedWithKind.InhType.General ||
                        parent.Type == DerivedWithKind.InhType.Implementation)
                        sb.Append((parent.Rel as Model).GenerateContainments(ref names, ref forInterface));
                }
            }

            return sb.ToString();
        }
        private string generateOwnContainments(ref List<string> names, ref StringBuilder forInterface)
        {
            StringBuilder sb = new StringBuilder();
            foreach (FCO cont in Contained)
            {
                if (!names.Contains(cont.className))
                {
                    sb.Append(generateContainment(cont));
                    if (this.HasChildren)
                    {
                        forInterface.Append(generateContainmentForInterface(cont));
                    }
                    names.Add(cont.className);
                }
            }
            return sb.ToString();
        }
        private string generateContainment(FCO current)
        {
            StringBuilder inner = new StringBuilder();

            //FCO current = Object.ElementsByName[typename] as FCO;

            inner.AppendFormat(Model.Template.ContainmentInner, current.className, current.ProperClassName, current.memberType);
            if (current.HasChildren)
            {
                //and add all of the children
                foreach (DerivedWithKind child in current.ChildrenRecursive)
                {
                    if (child.Type == DerivedWithKind.InhType.General ||
                        child.Type == DerivedWithKind.InhType.Interface)
                        inner.AppendFormat(Model.Template.ContainmentInner, child.Rel.className, child.Rel.ProperClassName, child.Rel.memberType);
                }
            }

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Model.Template.Containment, current.className, inner.ToString());

            return sb.ToString();
        }
        private string generateContainmentForInterface(FCO current)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Model.Template.ContainmentInterface, current.className);

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
            List<string> conts = new List<string>();
            List<string> crnews = new List<string>();

            StringBuilder sbAttrib = new StringBuilder();
            StringBuilder sbContain = new StringBuilder();
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
                GenerateContainments(ref conts, ref sbContain),
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
                sbContain.ToString());
            }

            return sb.ToString();
        }
    }
}
        