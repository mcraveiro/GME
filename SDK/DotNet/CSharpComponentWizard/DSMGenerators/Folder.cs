using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace DSM.Generators
{
    public class Folder : Object
    {
        new public class Template
        {
            #region Container
            public static readonly string ContainerFolderInner =
@"
                        if (this.mgaObject.ParentFolder.MetaBase.Name == ""{0}"")
                            return new {0}(this.mgaObject.ParentFolder);
";
            public static readonly string Container =
@"
        public IContainer Container
        {{
            get
            {{
                if (this.mgaObject.ParentFolder != null)
                {{
                    if (this.mgaObject.ParentFolder ==MgaGateway.project.RootFolder)
                    {{
                        return new RootFolder(MgaGateway.project as GME.MGA.IMgaFolder);
                    }}
                    else
                    {{
                        {0}
                    }}
                }}

                return null;
            }}
        }}
";
            #endregion
            #region Containment
            public static readonly string Containment =
@"
        public IEnumerable<{0}> Contained{0}s
        {{
            get
            {{
                MgaObjects children = mgaObject.ChildObjects;
                foreach (MgaObject o in children)
                {{
                    if (o.MetaBase.Name == ""{0}"")
                        yield return new {0}(o as {1});
                }}
            }}
        }}
";
            #endregion
            #region General
            public static readonly string Class =
@"
namespace {0}
{{
    public class {1} : {2}
    {{
        internal {3} mgaObject;
        internal static {6} mgaMetaObject = MgaGateway.GetMetaByName(""{7}"") as {6};
        public {1}({3} mgaObject)
        {{
            this.mgaObject = mgaObject;
        }}

{8}
    
        #region Common
{4}
        #endregion

        #region Containment
{5}
        #endregion
    }}
}}
";
            #endregion
        }

        public Folder(GME.MGA.IMgaAtom mgaObject)
        {            
            baseInterfaceName = "IFolder";

            memberType = "IMgaFolder";
            namespaceName = Generator.NamespaceName;

            if (mgaObject != null)
            {
                className = mgaObject.Name;

                //rootfolder is initialized with mgaObject == null
                this.mgaObject = mgaObject;

                if (Object.ElementsByName.ContainsKey(mgaObject.Name))
                {
                    //TODO
                    //throw new Exception(string.Format("Element name is not unique: {0}", mgaObject.Name));
                    DSM.GeneratorFacade.Errors.Add(string.Format("Element name is not unique: {0}", mgaObject.Name));
                }
                else
                {
                    Object.ElementsByName.Add(mgaObject.Name, this);
                }

                //if (Object.ElementsByName.ContainsKey(mgaObject.Name))
                //{
                //    throw new Exception(string.Format("Element name is not unique: {0}", mgaObject.Name));
                //}

                //Object.ElementsByName.Add(mgaObject.Name, this);

                if (InRootFolder)
                    RootFolder.RootObject.Add(this);
            }
        }

        #region Containment
        protected virtual IEnumerable<Object> Contained
        {
            get
            {
                foreach (GME.MGA.IMgaObject mgaObject in this.MgaObjects)
                {
                    GME.MGA.IMgaFCO fco = mgaObject as GME.MGA.IMgaFCO;
                    foreach (GME.MGA.IMgaConnPoint conn in fco.PartOfConns)
                    {
                        if (conn.Owner.Meta.Name == "FolderContainment" && conn.ConnRole == "dst")
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
                                            yield return Object.ElementsByName[connOther.Target.Name] as Object;
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

        public string GenerateContainments()
        {
            StringBuilder sb = new StringBuilder();

            sb.Append(generateOwnContainments());

            return sb.ToString();
        }
        private string generateOwnContainments()
        {
            StringBuilder sb = new StringBuilder();
            foreach (Object cont in Contained)
            {
                if (cont is Folder)
                    sb.Append(generateContainment(cont.className, cont.memberType));
                else
                    sb.Append(generateContainment(cont as FCO));
            }
            return sb.ToString();
        }
        private string generateContainment(string typename, string type)
        {
            //addCheckConnectionName(typename);

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat(Folder.Template.Containment, typename, type);

            return sb.ToString();
        }
        private string generateContainment(FCO current)
        {
            StringBuilder inner = new StringBuilder();

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
        #endregion

        #region Container
        public string GenerateContainer()
        {
            StringBuilder sb = new StringBuilder();

            StringBuilder folder = new StringBuilder();

            List<string> foldernames = new List<string>();

            generateContainerInner(ref folder, ref foldernames);

            sb.AppendFormat(Folder.Template.Container, folder.ToString(), Generator.ClassName);

            return sb.ToString();
        }
        private void generateContainerInner(ref StringBuilder folder, ref List<string> folderNames)
        {
            generateOwnContainerInnerFolder(ref folder, ref folderNames);
            
        }
        private void generateOwnContainerInnerFolder(ref StringBuilder sb, ref List<string> folderNames)
        {
            //PossibleContainers that are folders
            foreach (Object container in this.PossibleContainers)
            {
                Folder folder = container as Folder;
                if (folder != null)
                {
                    if (!folderNames.Contains(folder.className))
                    {
                        //this container is a model
                        sb.AppendFormat(Folder.Template.ContainerFolderInner, folder.className, folder.className);
                        folderNames.Add(folder.className);
                    }
                }
            }
        }

        #endregion

        public override string GenerateClass()
        {
            List<string> crnews = new List<string>();

            StringBuilder sb = new StringBuilder();
            sb.Append(Init());

            sb.AppendFormat(
                Folder.Template.Class,
                namespaceName,
                className,
                baseInterfaceName,
                memberType,
                GenerateCommon()+GenerateContainer(),
                GenerateContainments(),
                "IMgaMeta" + memberType.Substring(4),
                className,
                GenerateCreateNews(ref crnews, this));

            return sb.ToString();
        }

        public override void Save()
        {
            Directory.CreateDirectory(Path.Combine(Generator.Path, Generator.ClassName));

            string pathName = Path.Combine(Generator.Path, Generator.ClassName) + @"\" + this.Name + ".cs";
            GeneratorFacade.generatedFiles.Add(pathName);
            using (TextWriter tw = new StreamWriter(pathName))
            {
                tw.WriteLine(GenerateClass());
            }
        }
    }
}
