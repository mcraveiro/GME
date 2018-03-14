using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using GME.CSharp;
using GME;
using GME.MGA;
using GME.MGA.Core;
using System.Linq;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Diagnostics.Contracts;
using System.Windows.Forms;

// common library
using ISIS.GME.Common;

using System.Reflection;
using System.Linq.Expressions;
using System.Drawing;
using GME.Util;

namespace CSharpDSMLGenerator
{
    /// <summary>
    /// This class implements the necessary COM interfaces for a GME interpreter
    /// component.
    /// </summary>
    [Guid(ComponentConfig.guid),
    ProgId(ComponentConfig.progID),
    ClassInterface(ClassInterfaceType.AutoDual)]
    [ComVisible(true)]
    public class CSharpDSMLGeneratorInterpreter : IMgaComponentEx, IGMEVersionInfo
    {
        /// <summary>
        /// Contains information about the GUI event that initiated the invocation.
        /// </summary>
        [ComVisible(false)]
        public enum ComponentStartMode
        {
            GME_MAIN_START = 0, 		// Not used by GME
            GME_BROWSER_START = 1,      // Right click in the GME Tree Browser window
            GME_CONTEXT_START = 2,		// Using the context menu by right clicking a model element in the GME modeling window
            GME_EMBEDDED_START = 3,		// Not used by GME
            GME_MENU_START = 16,		// Clicking on the toolbar icon, or using the main menu
            GME_BGCONTEXT_START = 18,	// Using the context menu by right clicking the background of the GME modeling window
            GME_ICON_START = 32,		// Not used by GME
            GME_SILENT_MODE = 128 		// Not used by GME, available to testers not using GME
        }

        /// <summary>
        /// This function is called for each interpreter invocation before Main.
        /// Don't perform MGA operations here unless you open a transaction.
        /// </summary>
        /// <param name="project">The handle of the project opened in GME, for 
        /// which the interpreter was called.</param>
        public void Initialize(MgaProject project)
        {
            // TODO: Add your initialization code here...            
        }

        /// <summary>
        /// The main entry point of the interpreter. A transaction is already open,
        /// GMEConsole is avaliable. A general try-catch block catches all the exceptions
        /// coming from this function, you don't need to add it. For more information,
        /// see InvokeEx.
        /// </summary>
        /// <param name="project">The handle of the project opened in GME, for which 
        /// the interpreter was called.</param>
        /// <param name="currentobj">The model open in the active tab in GME. Its 
        /// value is null if no model is open (no GME modeling windows open). </param>
        /// <param name="selectedobjs">
        /// A collection for the selected  model elements. It is never null.
        /// If the interpreter is invoked by the context menu of the GME Tree Browser, 
        /// then the selected items in the tree browser. Folders
        /// are never passed (they are not FCOs).
        /// If the interpreter is invoked by clicking on the toolbar icon or the 
        /// context menu of the modeling window, then the selected items 
        /// in the active GME modeling window. If nothing is selected, the 
        /// collection is empty (contains zero elements).
        /// </param>
        /// <param name="startMode">Contains information about the GUI event 
        /// that initiated the invocation.</param>
        [ComVisible(false)]
        public void Main(
            MgaProject project,
            MgaFCO currentobj,
            MgaFCOs selectedobjs,
            ComponentStartMode startMode)
        {
            System.Diagnostics.Stopwatch sw = System.Diagnostics.Stopwatch.StartNew();

            GMEConsole.Out.WriteLine("Running interpreter...");

            if (project.RootMeta.Name == "MetaGME")
            {
                GMEConsole.Out.WriteLine("Generating .NET API code");
                GenerateDotNetCode(project, currentobj, selectedobjs, startMode);
            }

            GMEConsole.Out.WriteLine("Elapsed time: {0}", sw.Elapsed.ToString("c"));
        }


        #region Code generator functions
        private void GenerateDotNetCode(
            MgaProject project,
            MgaFCO currentobj,
            MgaFCOs selectedobjs,
            ComponentStartMode startMode)
        {
            // speed up the generation process
            CSharpDSMLGenerator.Generator.FCO.derivedClassCache.Clear();

            string projectDir = Path.GetDirectoryName(project.ProjectConnStr.Substring("MGA=".Length));
            string paradigmXmpFile = Path.Combine(projectDir, project.RootFolder.Name);
            paradigmXmpFile = Path.ChangeExtension(paradigmXmpFile, "xmp");
            if (!File.Exists(paradigmXmpFile))
            {
                string error = String.Format("Paradigm file '{0}' does not exist.", paradigmXmpFile);
                if (startMode == ComponentStartMode.GME_SILENT_MODE)
                    throw new Exception(error);
                DialogResult yesno = MessageBox.Show(error + "\n\nDo you want to run the Meta Interpreter?",
                    "Paradigm file missing", MessageBoxButtons.YesNo);
                if (yesno != DialogResult.Yes)
                    throw new Exception(error);
                MgaGateway.AbortTransaction();
                IMgaComponentEx metaInterpreter = (IMgaComponentEx)Activator.CreateInstance(Type.GetTypeFromProgID("MGA.Interpreter.MetaInterpreter"));
                metaInterpreter.InvokeEx(project, null, null, (int)ComponentStartMode.GME_SILENT_MODE);
                MgaGateway.BeginTransaction();
            }

            string outputDir = ".";
            if (project.ProjectConnStr.StartsWith("MGA="))
            {
                outputDir = Path.GetDirectoryName(project.ProjectConnStr.Substring(4));
            }
            GeneratorMode mode;
            if (startMode != ComponentStartMode.GME_SILENT_MODE)
            {
                ApiGenerator apiForm = new ApiGenerator();
                apiForm.txtOutputDir.Text =
                    Path.GetDirectoryName(project.ProjectConnStr.Substring("MGA=".Length));

                DialogResult dr = apiForm.ShowDialog();
                if (dr != DialogResult.OK)
                {
                    return;
                }

                mode = ((ApiGenerator.ModeItem)apiForm.cbMode.SelectedItem).Mode;
            }
            else
            {
                mode = GeneratorMode.Namespaces;
            }
            var compileUnit = GenerateDotNetCode(project, paradigmXmpFile, outputDir, mode);
            CompileDll(outputDir, compileUnit);
        }

        [ComVisible(false)]
        public CodeCompileUnit GenerateDotNetCode(MgaProject project, string paradigmXmpFile, string outputDir, GeneratorMode mode)
        {
            //paradigm = MgaMeta.DsmlModel.GetParadigm(paradigmXmpFile);
            Generator.Configuration.DsmlModel = new MgaMeta.DsmlModel(paradigmXmpFile);
            // Important step
            //Generator.Configuration.LocalFactory.Clear();
            Generator.Configuration.DsmlName = project.RootFolder.Name;
            List<MgaObject> all = new List<MgaObject>();

            // TODO: add version
            //var fileVersion = new CodeAttributeDeclaration() {
            //    Name = "AssemblyFileVersionAttribute",
            //};

            //var version = new CodeAttributeDeclaration()
            //{
            //    Name = "AssemblyVersion"
            //};
            //fileVersion.Arguments.Add(new CodeAttributeArgument("AssemblyFileVersionAttribute", new CodeSnippetExpression("1.0.0.*")));
            //version.Arguments.Add(new CodeAttributeArgument(new CodeSnippetExpression("1.0.0.*")));

            //compileunit.AssemblyCustomAttributes.Add(fileVersion);
            //compileunit.AssemblyCustomAttributes.Add(version);

            all.AddRange(
                FlattenMga<IMgaObject>(project.RootFolder, x => x.ChildObjects.Cast<MgaObject>()).
                Cast<MgaObject>());

            int countImplIntInheritance = all.
                Where(x =>
                    x.MetaBase.Name == "ImplementationInheritance" ||
                    x.MetaBase.Name == "InterfaceInheritance").Count();

            if (countImplIntInheritance > 0)
            {
                throw new NotSupportedException("ImplementationInheritance and InterfaceInheritance elements are not supported.");
            }

            List<string> l = new List<string>();
            l.Add("Atom");
            l.Add("Model");
            l.Add("Reference");
            l.Add("FCO");
            l.Add("Connection");
            l.Add("Set");
            l.Add("Folder");
            l.Add("RootFolder");

            Func<MgaObject, bool> condition =
                new Func<MgaObject, bool>(x => l.Contains(x.MetaBase.Name));

            GMEConsole.Info.WriteLine("Processing graph...");

            CodeCompileUnit compileunit = new CodeCompileUnit();
            foreach (var obj in all.Where(condition))
            {
                if (obj.MetaBase.Name == "RootFolder" &&
                    string.IsNullOrEmpty((obj as MgaFolder).LibraryName) == false)
                {
                    //throw new NotSupportedException("Attached libraries are not supported yet.");
                    continue;
                }

                GMEConsole.Info.WriteLine("{0} {1} {2}", obj.Name, obj.MetaBase.Name, obj.ObjType);
                Generator.FCO b = new Generator.FCO(
                    obj,
                    new CodeTypeReferenceCollection(),
                    new CodeTypeReferenceCollection());

                // assumption we have two namespaces only
                //  (this is the main global namespace for classes/interfaces , other 
                //  namespaces, which are defined in the project will be under this namespace.)
                // - one for the classes 
                // - one for the interfaces
                CodeNamespace nsClass = compileunit.Namespaces.
                    Cast<CodeNamespace>().
                    FirstOrDefault(x => x.Name == b.GeneratedClass.Name);

                if (nsClass == null)
                {
                    compileunit.Namespaces.Add(b.GeneratedClass);
                }
                else
                {
                    nsClass.Types.AddRange(b.GeneratedClass.Types);
                }

                CodeNamespace nsInterface = compileunit.Namespaces.
                    Cast<CodeNamespace>().
                    FirstOrDefault(x => x.Name == b.GeneratedInterface.Name);

                if (nsClass == null)
                {
                    compileunit.Namespaces.Add(b.GeneratedInterface);
                }
                else
                {
                    nsInterface.Types.AddRange(b.GeneratedInterface.Types);
                }
            }


            // TODO: a lot of parameters must be set based on the user's choice.
            CodeDomGenerateCode(
                CodeDomProvider.CreateProvider(language),
                compileunit,
                Path.Combine(outputDir, project.Name),
                mode);

            GMEConsole.Info.WriteLine("API has been generated.");
            return compileunit;
        }

        private string language = "c#";

        [ComVisible(false)]
        public bool CompileDll(string outputDir, CodeCompileUnit compileunit)
        {

            string dllFile = Path.Combine(outputDir, Generator.Configuration.ProjectNamespace + ".dll");

            // Configure a CompilerParameters that links System.dll
            // and produces the specified dll file.
            string[] referenceAssemblies =
				{ "System.dll",
					"System.Core.dll",
					Assembly.GetAssembly(typeof(GME.MGA.Meta.MgaMetaBase)).Location,
					Assembly.GetAssembly(typeof(GME.MGA.MgaObjectClass)).Location,
					Assembly.GetAssembly(typeof(ISIS.GME.Common.Utils)).Location,
				};

            CompilerParameters cp = new CompilerParameters(
                referenceAssemblies,
                dllFile,
                false);

            // Generate a DLL file.
            cp.GenerateExecutable = false;

            using (FileStream fs = File.Create(Path.Combine(outputDir, "AssemblySignature.snk")))
            {
                fs.Write(
                    CSharpDSMLGenerator.Properties.Resources.AssemblySignature,
                    0,
                    CSharpDSMLGenerator.Properties.Resources.AssemblySignature.Length);
            }

            cp.CompilerOptions += " /debug /pdb:\"" + Path.Combine(outputDir, Generator.Configuration.ProjectNamespace) + "\"";
            cp.CompilerOptions += " /doc:\"" + Path.Combine(outputDir, Generator.Configuration.ProjectNamespace) + ".xml\"";
            cp.CompilerOptions += " /keyfile:\"" + Path.Combine(outputDir, "AssemblySignature.snk") + "\"";
            //cp.CompilerOptions += " /optimize";

            // Invoke compilation.
            //CompilerResults cr = CodeDomProvider.CreateProvider(language).
            //CompileAssemblyFromFile(cp, sourceFile.ToArray());
            CompilerResults cr = CodeDomProvider.CreateProvider(language).CompileAssemblyFromDom(cp, compileunit);

            GMEConsole.Info.WriteLine("Compiling source code.");

            cr.Errors.Cast<CompilerError>().ToList().
                ForEach(x => System.Diagnostics.Debug.WriteLine(x.ErrorText));
            var errors = cr.Errors.Cast<CompilerError>().ToList();

            // errors last on the console
            errors.Sort((x, y) => y.IsWarning.CompareTo(x.IsWarning));

            errors.
                ForEach(x =>
                {
                    var msg = string.Format("{0} ({1}) {2} ", x.FileName, x.Line, x.ErrorText);

                    if (x.IsWarning)
                    {
                        GMEConsole.Warning.WriteLine(msg);
                    }
                    else
                    {
                        GMEConsole.Error.WriteLine(msg);
                    }
                });

            if (cr.Errors.Count == 0)
            {
                GMEConsole.Info.WriteLine(
                    "{0} was generated and is ready to use.",
                    Path.GetFullPath(cr.PathToAssembly));
                return true;
            }
            return false;

        }

        [ComVisible(false)]
        public static IEnumerable<T> FlattenMga<T>(T item, Func<T, IEnumerable<T>> next)
            where T : IMgaObject
        {
            yield return item;
            if (item is MgaModel ||
                item is MgaFolder)
            {
                foreach (T child in next(item))
                {
                    foreach (T flattenedChild in FlattenMga(child, next))
                    {
                        yield return flattenedChild;
                    }
                }
            }
        }

        [ComVisible(false)]
        public enum GeneratorMode
        {
            /// <summary>
            /// Generates one file, which contains class and
            /// interface definitions.
            /// </summary>
            OneFile,

            /// <summary>
            /// <para>Generates separate files for namespaces
            /// /in general two files/.</para>
            /// <para>1. Classes</para>
            /// <para>2. Interfaces</para>
            /// </summary>
            Namespaces,

            /// <summary>
            /// <para>Generates separate files for each class and
            /// each interface.</para>
            /// </summary>
            Many,
        }

        [ComVisible(false)]
        public List<string> CodeDomGenerateCode(
            CodeDomProvider provider,
            CodeCompileUnit compileunit,
            string outputFileName = "",
            GeneratorMode mode = GeneratorMode.OneFile)
        {
            GMEConsole.Info.WriteLine("Generating source code.");
            string outputDir = Path.GetDirectoryName(outputFileName);

            List<string> sourceFiles = new List<string>();

            CodeGeneratorOptions options = new CodeGeneratorOptions();
            options.BlankLinesBetweenMembers = true;
            options.BracingStyle = "C";
            options.IndentString = "\t";

            if (mode == GeneratorMode.OneFile)
            {
                string fileName = "TestAPI";
                if (string.IsNullOrEmpty(outputFileName) == false)
                {
                    fileName = Path.GetFileNameWithoutExtension(outputFileName);
                }

                // Build the source file name with the appropriate
                // language extension.
                String sourceFile;
                if (provider.FileExtension[0] == '.')
                {
                    sourceFile = fileName + provider.FileExtension;
                }
                else
                {
                    sourceFile = fileName + "." + provider.FileExtension;
                }

                // Create an IndentedTextWriter, constructed with
                // a StreamWriter to the source file.
                IndentedTextWriter tw = new IndentedTextWriter(
                    new StreamWriter(sourceFile, false));
                sourceFiles.Add(sourceFile);

                // Generate source code using the code generator.
                provider.GenerateCodeFromCompileUnit(compileunit, tw, options);

                GMEConsole.Info.WriteLine("- {0}", Path.GetFullPath(sourceFile));
                // Close the output file.
                tw.Close();
            }
            else if (mode == GeneratorMode.Namespaces)
            {
                foreach (CodeNamespace ns in compileunit.Namespaces)
                {
                    // Build the source file name with the appropriate
                    // language extension.
                    String sourceFile;
                    if (provider.FileExtension[0] == '.')
                    {
                        sourceFile = ns.Name + provider.FileExtension;
                    }
                    else
                    {
                        sourceFile = ns.Name + "." + provider.FileExtension;
                    }
                    sourceFile = Path.Combine(outputDir, sourceFile);

                    CodeCompileUnit ccu = new CodeCompileUnit();
                    ccu.Namespaces.Add(ns);

                    // Create an IndentedTextWriter, constructed with
                    // a StreamWriter to the source file.
                    IndentedTextWriter tw = new IndentedTextWriter(
                        new StreamWriter(sourceFile, false));
                    sourceFiles.Add(sourceFile);

                    // Generate source code using the code generator.
                    provider.GenerateCodeFromCompileUnit(ccu, tw, options);
                    GMEConsole.Info.WriteLine("- {0}", Path.GetFullPath(sourceFile));
                    // Close the output file.
                    tw.Close();
                }
            }
            else if (mode == GeneratorMode.Many)
            {
                foreach (CodeNamespace ns in compileunit.Namespaces)
                {
                    foreach (CodeTypeDeclaration ctd in ns.Types)
                    {
                        CodeNamespace nsNew = new CodeNamespace()
                        {
                            Name = ns.Name,
                        };
                        nsNew.Imports.AddRange(ns.Imports.Cast<CodeNamespaceImport>().ToArray());
                        nsNew.Comments.AddRange(ns.Comments.Cast<CodeCommentStatement>().ToArray());

                        // only one type (class or interface)
                        nsNew.Types.Add(ctd);

                        // Build the source file name with the appropriate
                        // language extension.
                        String sourceFile;
                        if (provider.FileExtension[0] == '.')
                        {
                            sourceFile = nsNew.Name + "." + ctd.Name + provider.FileExtension;
                        }
                        else
                        {
                            sourceFile = nsNew.Name + "." + ctd.Name + "." + provider.FileExtension;
                        }
                        sourceFile = Path.Combine(outputDir, sourceFile);

                        CodeCompileUnit ccu = new CodeCompileUnit();
                        ccu.Namespaces.Add(nsNew);

                        // Create an IndentedTextWriter, constructed with
                        // a StreamWriter to the source file.
                        IndentedTextWriter tw = new IndentedTextWriter(
                            new StreamWriter(sourceFile, false));
                        sourceFiles.Add(sourceFile);

                        // Generate source code using the code generator.
                        provider.GenerateCodeFromCompileUnit(ccu, tw, options);
                        GMEConsole.Info.WriteLine("- {0}", Path.GetFullPath(sourceFile));
                        // Close the output file.
                        tw.Close();
                    }
                }
            }
            return sourceFiles;
        }

        #endregion


        #region test code for callback functions
        public class Process
        {
            [My("Atom1")]
            public Expression<Func<int, int>> FuncAtom = x => x * x;

            [My("Atom2")]
            public Func<int, bool> FuncAtom2;

            [My("Atom3")]
            public bool FuncAtom3(int x)
            {
                if (x > 2)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

            public delegate int MyNewFunc(int x);

            [My("Atom4")]
            public int MyNewFunc2(MyNewFunc func, int x)
            {
                return func(x);
            }
            //public static int MyNewFunc2(int x)
            //{
            //  return x * x;
            //}

        }

        public void ProcessGraph()
        {
            foreach (MethodInfo item in typeof(Process).GetMethods())
            {
                MyAttribute attr = (MyAttribute)Attribute.GetCustomAttribute(item, typeof(MyAttribute));
                if (attr != null)
                {
                    //object o = typeof(Process).InvokeMember(item.Name, BindingFlags.InvokeMethod | BindingFlags.Static, null, null, null);
                }
            }
            object o = Activator.CreateInstance(typeof(Process));
            foreach (FieldInfo item in typeof(Process).GetFields())
            {
                MyAttribute attr = (MyAttribute)Attribute.GetCustomAttribute(item, typeof(MyAttribute));
                if (attr != null)
                {
                    if (attr.PositionalString == "Atom1")
                    {
                        MethodInfo mi = item.FieldType.GetMethods().FirstOrDefault(x => x.Name == "Compile");
                        object[] param = new object[1];
                        param[0] = 5;
                        int x2 = (int)mi.Invoke(o, param);
                    }
                    //ParameterInfo[] pis = mi.GetParameters();
                    //int i = (int) mi.Invoke(System.Linq.Expressions.Expression.Bind(
                }
            }
        }


        [AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = true)]
        sealed class MyAttribute : Attribute
        {
            // See the attribute guidelines at 
            //  http://go.microsoft.com/fwlink/?LinkId=85236
            readonly string positionalString;

            // This is a positional argument
            public MyAttribute(string positionalString)
            {
                this.positionalString = positionalString;

                // TODO: Implement code here
                //throw new NotImplementedException();
            }

            public string PositionalString
            {
                get { return positionalString; }
            }

            // This is a named argument
            public int NamedInt { get; set; }
        }

        #endregion


        #region IMgaComponentEx Members

        internal MgaGateway MgaGateway { get; set; }
        internal GMEConsole GMEConsole { get; set; }

        public void InvokeEx(MgaProject project, MgaFCO currentobj, MgaFCOs selectedobjs, int param)
        {
            if (!enabled)
            {
                return;
            }

            try
            {
                GMEConsole = GMEConsole.CreateFromProject(project);
                MgaGateway = new MgaGateway(project);
                project.CreateTerritoryWithoutSink(out MgaGateway.territory);

                MgaGateway.PerformInTransaction(delegate
                {
                    Main(project, currentobj, selectedobjs, Convert(param));
                },
                transactiontype_enum.TRANSACTION_NON_NESTED);
            }
            finally
            {
                if (MgaGateway.territory != null)
                {
                    MgaGateway.territory.Destroy();
                }
                //ISIS.GME.Common.Utils.ObjectCache.Clear();


                MgaGateway = null;
                project = null;
                currentobj = null;
                selectedobjs = null;
                GMEConsole = null;
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();
            }
        }

        private ComponentStartMode Convert(int param)
        {
            switch (param)
            {
                case (int)ComponentStartMode.GME_BGCONTEXT_START:
                    return ComponentStartMode.GME_BGCONTEXT_START;
                case (int)ComponentStartMode.GME_BROWSER_START:
                    return ComponentStartMode.GME_BROWSER_START;

                case (int)ComponentStartMode.GME_CONTEXT_START:
                    return ComponentStartMode.GME_CONTEXT_START;

                case (int)ComponentStartMode.GME_EMBEDDED_START:
                    return ComponentStartMode.GME_EMBEDDED_START;

                case (int)ComponentStartMode.GME_ICON_START:
                    return ComponentStartMode.GME_ICON_START;

                case (int)ComponentStartMode.GME_MAIN_START:
                    return ComponentStartMode.GME_MAIN_START;

                case (int)ComponentStartMode.GME_MENU_START:
                    return ComponentStartMode.GME_MENU_START;
                case (int)ComponentStartMode.GME_SILENT_MODE:
                    return ComponentStartMode.GME_SILENT_MODE;
            }

            return ComponentStartMode.GME_SILENT_MODE;
        }

        #region Component Information
        public string ComponentName
        {
            get { return GetType().Name; }
        }

        public string ComponentProgID
        {
            get
            {
                return ComponentConfig.progID;
            }
        }

        public componenttype_enum ComponentType
        {
            get { return ComponentConfig.componentType; }
        }
        public string Paradigm
        {
            get { return ComponentConfig.paradigmName; }
        }
        #endregion

        #region Enabling
        bool enabled = true;
        public void Enable(bool newval)
        {
            enabled = newval;
        }
        #endregion

        #region Interactive Mode
        protected bool interactiveMode = true;
        public bool InteractiveMode
        {
            get
            {
                return interactiveMode;
            }
            set
            {
                interactiveMode = value;
            }
        }
        #endregion

        #region Custom Parameters
        SortedDictionary<string, object> componentParameters = null;

        public object get_ComponentParameter(string Name)
        {
            if (Name == "type")
                return "csharp";

            if (Name == "path")
                return GetType().Assembly.Location;

            if (Name == "fullname")
                return GetType().FullName;

            object value;
            if (componentParameters != null && componentParameters.TryGetValue(Name, out value))
            {
                return value;
            }

            return null;
        }

        public void set_ComponentParameter(string Name, object pVal)
        {
            if (componentParameters == null)
            {
                componentParameters = new SortedDictionary<string, object>();
            }

            componentParameters[Name] = pVal;
        }
        #endregion

        #region Unused Methods
        // Old interface, it is never called for MgaComponentEx interfaces
        public void Invoke(MgaProject Project, MgaFCOs selectedobjs, int param)
        {
            throw new NotImplementedException();
        }

        // Not used by GME
        public void ObjectsInvokeEx(MgaProject Project, MgaObject currentobj, MgaObjects selectedobjs, int param)
        {
            throw new NotImplementedException();
        }

        #endregion

        #endregion

        #region IMgaVersionInfo Members

        public GMEInterfaceVersion_enum version
        {
            get { return GMEInterfaceVersion_enum.GMEInterfaceVersion_Current; }
        }

        #endregion

        #region Registration Helpers

        [ComRegisterFunctionAttribute]
        public static void GMERegister(Type t)
        {
            Registrar.RegisterComponentsInGMERegistry();

        }

        [ComUnregisterFunctionAttribute]
        public static void GMEUnRegister(Type t)
        {
            Registrar.UnregisterComponentsInGMERegistry();
        }

        #endregion


    }
}
