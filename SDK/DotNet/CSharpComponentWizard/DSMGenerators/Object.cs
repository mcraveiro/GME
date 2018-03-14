using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace DSM.Generators
{
    public abstract class Object
    {
        public class Template
        {

            #region CreateNew
            public static readonly string CreateNewModelParent =
@"
        public static {0} CreateNew({1} parent)
        {{
            foreach (MgaMetaRole role in (parent.mgaObject.MetaBase as IMgaMetaModel).Roles)
            {{
                if (role.kind == mgaMetaObject)
                {{
                    return new {0}(parent.mgaObject.CreateChildObject(role) as {2});
                }}
            }}
            throw new ArgumentException();
        }}
";
            public static readonly string CreateNewFolderParent =
@"
        public static {0} CreateNew({1} parent)
        {{
            foreach (MgaMetaFCO rootObj in (parent.mgaObject.MetaBase as IMgaMetaFolder).LegalRootObjects)
            {{
                if (rootObj == mgaMetaObject)
                {{
                    return new {0}(parent.mgaObject.CreateRootObject(rootObj) as {2});
                }}
            }}
            throw new ArgumentException();
        }}
";
            public static readonly string CreateNewModelParentConnection =
@"
        public static {0} CreateNew({1} parent)
        {{
            foreach (MgaMetaRole role in (parent.mgaObject.MetaBase as IMgaMetaModel).Roles)
            {{
                if (role.kind == mgaMetaObject)
                {{
                    return new {0}(parent.mgaObject.CreateSimpleConn(role, null, null, null, null) as IMgaConnection);
                }}
            }}
            throw new ArgumentException();
        }}
";
            #endregion
            #region General
            public static readonly string Init =
@"
using System;
using System.Collections.Generic;
using System.Text;

using GME.MGA;
using GME.MGA.Meta;
";

            public static readonly string Object =
@"
namespace {0}
{{
    public class {1} : {2}
    {{
        internal {{3}} mgaObject;
        
        public {1}({3} mgaObject)
        {{
            this.mgaObject = mgaObject;
        }}
    }}

    //Common
    {4}
}}
";
            public static readonly string General =
@"
        #region IObject Members

        public string Name
        {{
            get
            {{
                return mgaObject.Name;
            }}

            set
            {{
                mgaObject.Name = value;
            }}
        }}
        public string ID
        {{
            get
            {{
                return mgaObject.ID;
            }}
        }}

        public IMgaObject MgaObject
        {{
            get {{ return mgaObject; }}
        }}
        #endregion
        
        public override bool Equals(object obj)
        {{
            if (obj == null)
                return false;

            if (obj is IObject)
                return this.ID == (obj as IObject).ID;
            
            return false;
        }}
        public override int GetHashCode()
        {{
            return this.ID.GetHashCode();
        }}
        public static bool operator==({0} o1, IObject o2)
        {{
            return (o1.ID == o2.ID);
        }}
        public static bool operator!=({0} o1, IObject o2)
        {{
            return (o1.ID != o2.ID);
        }}
        public void Delete()
        {{
            this.mgaObject.DestroyObject();
        }}
";
            #endregion
        }

        public static Dictionary<string, string> ProxyCache = new Dictionary<string, string>();
        public static Dictionary<string, GME.MGA.IMgaReference> ProxyObjects = new Dictionary<string, GME.MGA.IMgaReference>();
        public static Dictionary<string, Object> ElementsByName = new Dictionary<string, Object>();

        protected GME.MGA.IMgaAtom mgaObject;

        protected string namespaceName = "GME.CSharp.SOMENAME";
        
        internal string className = "ObjectName";
        
        protected string baseInterfaceName = "IObject";

        internal string memberType = "IMgaObject";

        public string Name
        {
            get 
            {
                return mgaObject.Name;
            }
        }

        protected bool InRootFolder
        {
            get 
            {
                return this.mgaObject.get_BoolAttrByName("InRootFolder");
            }
        }

        #region Parent - Child
        public class DerivedWithKind
        {
            public enum InhType
            {
                General, Implementation, Interface
            }

            public FCO Rel;
            public InhType Type;
        }
        internal virtual IEnumerable<DerivedWithKind> Parents
        {
            get { return new List<DerivedWithKind>(); }
        }

        internal virtual bool HasChildren
        {
            get { return false; }
        }
        #endregion

        internal IEnumerable<GME.MGA.IMgaObject> MgaObjects
        {
            get
            {
                yield return mgaObject;

                foreach (KeyValuePair<string, string> kvp in Object.ProxyCache)
                {
                    if (kvp.Value == this.Name)
                    {
                        string proxyname = kvp.Key;
                        yield return Object.ProxyObjects[proxyname];
                    }
                }
            }
        }

        #region CreateNew
        protected IEnumerable<Object> PossibleContainers
        {
            get
            {
                foreach (GME.MGA.IMgaConnPoint conn in mgaObject.PartOfConns)
                {
                    if ((conn.Owner.Meta.Name == "Containment" || conn.Owner.Meta.Name == "FolderContainment") && 
                        conn.ConnRole == "src")
                    {
                        foreach (GME.MGA.IMgaConnPoint connOther in conn.Owner.ConnPoints)
                        {
                            if (connOther.ConnRole == "dst")
                            {
                                //connOther.target: container
                                if (connOther.Target.MetaBase.Name.Contains("Proxy"))
                                {
                                    if (Object.ProxyCache.ContainsKey(connOther.Target.Name))
                                        yield return Object.ElementsByName[Object.ProxyCache[connOther.Target.Name]];
                                    else
                                        DSM.GeneratorFacade.Errors.Add("Proxy '" + connOther.Target.Name + "' is not found");
                                }
                                else
                                {
                                    if (Object.ElementsByName.ContainsKey(connOther.Target.Name))
                                        yield return Object.ElementsByName[connOther.Target.Name];
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
        public string GenerateCreateNews(ref List<string> containers, Object realCurrentObject)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(generateOwnCreateNews(ref containers, realCurrentObject));

            //genarate parents' attributes:
            foreach (DerivedWithKind parent in this.Parents)
            {
                if (parent.Rel is FCO)
                {
                    if (parent.Type == DerivedWithKind.InhType.General ||
                        parent.Type == DerivedWithKind.InhType.Interface)
                        sb.Append((parent.Rel as FCO).GenerateCreateNews(ref containers, realCurrentObject));
                }
            }

            return sb.ToString();
        }
        private string generateOwnCreateNews(ref List<string> containers, Object realCurrentObject)
        {
            StringBuilder sb = new StringBuilder();
            if (this.InRootFolder && !containers.Contains("RootFolder"))
            {                
                containers.Add("RootFolder");
                sb.AppendFormat(Object.Template.CreateNewFolderParent,
                    ((realCurrentObject.HasChildren) ? realCurrentObject.className + "Impl" : realCurrentObject.className), 
                    "RootFolder", 
                    this.memberType);
            }
            
            foreach (Object cont in PossibleContainers)
            {
                if (!containers.Contains(cont.className))
                {
                    sb.Append(generateCreateNew(cont, realCurrentObject));
                    containers.Add(cont.className);
                }
            }
            return sb.ToString();
        }
        private string generateCreateNew(Object current, Object realCurrentObject)
        {
            StringBuilder inner = new StringBuilder();

            if (realCurrentObject is Connection) //in this case the container can only be a model
                inner.AppendFormat(Object.Template.CreateNewModelParentConnection,
                        ((realCurrentObject.HasChildren) ? realCurrentObject.className + "Impl" : realCurrentObject.className),
                        ((current.HasChildren) ? current.className + "Impl" : current.className));
            else if (current is FCO)
                inner.AppendFormat(Object.Template.CreateNewModelParent,
                    ((realCurrentObject.HasChildren) ? realCurrentObject.className + "Impl" : realCurrentObject.className),
                    ((current.HasChildren) ? current.className + "Impl" : current.className),
                    realCurrentObject.memberType);
            else if (current is Folder)
                inner.AppendFormat(Object.Template.CreateNewFolderParent,
                    ((realCurrentObject.HasChildren) ? realCurrentObject.className + "Impl" : realCurrentObject.className),
                    ((current.HasChildren) ? current.className + "Impl" : current.className),
                    realCurrentObject.memberType);

            return inner.ToString();
        }
        #endregion


        public string GenerateCommon()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Object.Template.General, (this.HasChildren) ? className + "Impl" : className);
            return sb.ToString();
        }

        public virtual string GenerateClass()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(Init());

            sb.AppendFormat(Object.Template.Object, namespaceName, className, baseInterfaceName, memberType, GenerateCommon());

            return sb.ToString();
        }

        protected string Init()
        {
            return Object.Template.Init;
        }

        public abstract void Save();
    }
}
