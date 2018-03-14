using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.IO;
using Microsoft.Win32;
using EnvDTE80;
using EnvDTE100;
using DSM;
using System.Runtime.Versioning;

namespace CSharpComponentWizard
{
    public enum CompType
    {
        Addon,
        Interpreter
    }

    public enum Registration
    {
        Systemwide,
        User,
        Both
    }

    public enum ComponentInterface
    {
        Dependent,
        Independent
    }

    public static class SolutionGenerator
    {
        public const string ENTRYPOINTCODE_REPLACESTRING = "$GET_ROOTFOLDER_CODE$";

        public static MainWindow mw;
        public static Dictionary<int, string> AddonEvents;

        public static CompType SelectedType = CompType.Interpreter;
        public static ComponentInterface SelectedInterface = ComponentInterface.Dependent;
        public static Registration SelectedRegistration = Registration.Systemwide;
        public static bool[] AddonEventSelection = new bool[25];

        public static string TargetFolder;  // Without SolutionFolder
        public static string SolutionName;
        public static string ParadigmName;
        public static string ComponentName;
        public static string IconPath;
        public static string NewGuid;       // NewGuid, because Guid is a type
        public static string MgaPath;

        public static string ProjectTemplateLocation;
        public static string TemplateFileName;


        public static string GenerateSolution()
        {
            DTE2 dte;
            object obj;
            string outputfolder = "";

            try
            {
                if (SolutionGenerator.SelectedType == CompType.Addon)
                {
                    SolutionGenerator.TemplateFileName = "CSharpAddon.zip";
                }
                else // interpreter
                {
                    SolutionGenerator.TemplateFileName = "CSharpInterpreter.zip";
                }

                // Prefer latest VS
                System.Type type = null;
                if (type == null)
                {
                    type = System.Type.GetTypeFromProgID("VisualStudio.DTE.14.0");
                }
                if (type == null)
                {
                    type = System.Type.GetTypeFromProgID("VisualStudio.DTE.12.0");
                }
                if (type == null)
                {
                    type = System.Type.GetTypeFromProgID("VisualStudio.DTE.11.0");
                }
                if (type == null)
                {
                    type = System.Type.GetTypeFromProgID("VisualStudio.DTE.10.0");
                }

                obj = Activator.CreateInstance(type, true);
                dte = (DTE2)obj;
                Solution4 sln = (Solution4)dte.Solution;

                outputfolder = Path.Combine(TargetFolder, SolutionName);
                string solutionName = SolutionName + ".sln";

                if (Directory.Exists(outputfolder))
                {
                    Directory.Delete(outputfolder, true);
                }

                // Unpack the sufficent template project
                Stream TemplateStream = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream(
                            System.Reflection.Assembly.GetExecutingAssembly().GetName().Name + ".Templates." + SolutionGenerator.TemplateFileName);
                FileStream FileWriter = new FileStream(Path.Combine(SolutionGenerator.ProjectTemplateLocation, SolutionGenerator.TemplateFileName), FileMode.Create);
                using (TemplateStream)
                using (FileWriter)
                {
                    TemplateStream.CopyTo(FileWriter);
                }

                sln.Create(outputfolder, solutionName);

                string TemplatePath = sln.GetProjectTemplate(SolutionGenerator.TemplateFileName, "CSharp");
                sln.AddFromTemplate(TemplatePath, outputfolder, SolutionName, false);
                var project = sln.Projects.Item(1);
                var dotNetVersion = new FrameworkName(".NETFramework", new Version(4, 0));
                var fullname = dotNetVersion.FullName;
                project.Properties.Item("TargetFrameworkMoniker").Value = dotNetVersion.FullName;
                sln.Close();
            }
            finally
            {
                obj = dte = null;
            }
            return outputfolder;
        }

        public static void PostProcessComponentConfig(string outputfolder)
        {
            StreamReader ComponentConfigReadStream = new StreamReader(Path.Combine(outputfolder, "ComponentConfig.cs"));
            string ContentString = ComponentConfigReadStream.ReadToEnd();
            ComponentConfigReadStream.Close();

            // Fill in paradigm name
            ContentString = ContentString.Replace(@"$paradigmname$", ParadigmName);

            if (ParadigmName == "*")
            {
                if (SolutionGenerator.SelectedType == CompType.Addon)
                {
                    ContentString.Replace("COMPONENTTYPE_ADDON", "COMPONENTTYPE_ADDON | componenttype_enum.COMPONENTTYPE_PARADIGM_INDEPENDENT DELETE;");
                }
                else // Interpreter
                {
                    ContentString.Replace("COMPONENTTYPE_INTERPRETER", "COMPONENTTYPE_INTERPRETER | componenttype_enum.COMPONENTTYPE_PARADIGM_INDEPENDENT DELETE;");
                }
            }

            // Fill in ComponentName
            ContentString = ContentString.Replace(@"$componentname$", ComponentName);

            if (SelectedType == CompType.Addon)
            {
                // Set EventMask
                StringBuilder eventmask = new StringBuilder();
                bool firsttime = true;

                for (int i = 0; i <= 24; ++i)
                {
                    if (SolutionGenerator.AddonEventSelection[i])
                    {
                        string temp;
                        AddonEvents.TryGetValue(i, out temp);

                        if (firsttime)
                        {
                            eventmask.Append("objectevent_enum." + temp);
                        }
                        else
                        {
                            eventmask.Append(" | objectevent_enum." + temp);
                        }
                        firsttime = false;
                    }
                }

                if (firsttime == true)
                {
                    ContentString = ContentString.Replace(@"$eventmask$", "0");
                }
                else
                {
                    ContentString = ContentString.Replace(@"$eventmask$", eventmask.ToString());
                }
                ContentString = ContentString.Replace(@"$eventmask$", eventmask.ToString());

                // Set ComponentType
                ContentString = ContentString.Replace(@"$componenttype$", "ADDON");
            }
            else // interpreter
            {
                // Set IconName
                ContentString = ContentString.Replace(@"$iconname$", "Component.ico");

                if (IconPath != String.Empty && File.Exists(IconPath))
                {
                    File.Copy(IconPath, Path.Combine(outputfolder, "Component.ico"), true);
                }

                // Set IconPath
                ContentString = ContentString.Replace(@"$iconpath$", "null");

                // Set ComponentType
                ContentString = ContentString.Replace(@"$componenttype$", "INTERPRETER");
            }

            // Set Registration Scope
            if (SolutionGenerator.SelectedRegistration == Registration.Systemwide)
            {
                ContentString = ContentString.Replace(@"$regaccessmode$", "REGACCESS_SYSTEM");
            }
            else if (SolutionGenerator.SelectedRegistration == Registration.User)
            {
                ContentString = ContentString.Replace(@"$regaccessmode$", "REGACCESS_USER");
            }
            else if (SolutionGenerator.SelectedRegistration == Registration.Both)
            {
                ContentString = ContentString.Replace(@"$regaccessmode$", "REGACCESS_BOTH");
            }

            // Fill in progID
            ContentString = ContentString.Replace(@"$progid$", SolutionName.Replace(" ", ""));

            // Fill in guid
            ContentString = ContentString.Replace(@"$guid$", NewGuid);

            StreamWriter ComponentConfigWriteStream = new StreamWriter(Path.Combine(outputfolder, "ComponentConfig.cs"));
            ComponentConfigWriteStream.Write(ContentString);
            ComponentConfigWriteStream.Close();
        }

        public static void RenameProject(string outputfolder)
        {

            if (SolutionGenerator.SelectedType == CompType.Addon)
            {
                // Rename namespace, classname, and filename
                StreamReader ComponentConfigReadStream = new StreamReader(Path.Combine(outputfolder, "MyAddon.cs"));
                string ContentString = ComponentConfigReadStream.ReadToEnd();
                ComponentConfigReadStream.Close();

                ContentString = ContentString.Replace("MyAddon : IMgaComponentEx", SolutionName + "Addon : IMgaComponentEx");
                ContentString = ContentString.Replace("MyAddon", SolutionName);

                StreamWriter ComponentConfigWriteStream = new StreamWriter(Path.Combine(outputfolder, "MyAddon.cs"));
                ComponentConfigWriteStream.Write(ContentString);
                ComponentConfigWriteStream.Close();

                File.Move(Path.Combine(outputfolder, "MyAddon.cs"), Path.Combine(outputfolder, SolutionName + ".cs"));


                // Rename filereference in the csproj file
                ComponentConfigReadStream = new StreamReader(Path.Combine(outputfolder, SolutionName + ".csproj"));
                ContentString = ComponentConfigReadStream.ReadToEnd();
                ComponentConfigReadStream.Close();

                ContentString = ContentString.Replace("MyAddon", SolutionName);

                // Uncomment signature
                ContentString = ContentString.Replace("<!--DELETE", "");
                ContentString = ContentString.Replace("DELETE-->", "");

                ComponentConfigWriteStream = new StreamWriter(Path.Combine(outputfolder, SolutionName + ".csproj"));
                ComponentConfigWriteStream.Write(ContentString);
                ComponentConfigWriteStream.Close();
            }
            else // interpreter
            {
                // Rename namespace, classname, and filename
                StreamReader FileReadStream = new StreamReader(Path.Combine(outputfolder, "MyInterpreter.cs"));
                string ContentString = FileReadStream.ReadToEnd();
                FileReadStream.Close();

                ContentString = ContentString.Replace("MyInterpreter : IMgaComponentEx", SolutionName + "Interpreter : IMgaComponentEx");
                ContentString = ContentString.Replace("MyInterpreter", SolutionName);
                ContentString = SolutionGenerator.AddEntryPointCode(ContentString);

                StreamWriter FileWriteStream = new StreamWriter(Path.Combine(outputfolder, "MyInterpreter.cs"));
                FileWriteStream.Write(ContentString);
                FileWriteStream.Close();

                File.Move(Path.Combine(outputfolder, "MyInterpreter.cs"), Path.Combine(outputfolder, SolutionName + ".cs"));


                // Rename filereference in the csproj file
                FileReadStream = new StreamReader(Path.Combine(outputfolder, "" + SolutionName + ".csproj"));
                ContentString = FileReadStream.ReadToEnd();
                FileReadStream.Close();

                ContentString = ContentString.Replace("MyInterpreter", SolutionName);

                // Uncomment signature reference
                ContentString = ContentString.Replace("<!--DELETE", "");
                ContentString = ContentString.Replace("DELETE-->", "");

                FileWriteStream = new StreamWriter(Path.Combine(outputfolder, "" + SolutionName + ".csproj"));
                FileWriteStream.Write(ContentString);
                FileWriteStream.Close();
            }
        }

        public static void GenerateSignature(string outputfolder)
        {
            // Search sn.exe
            string SNLocation = null;

            foreach (var path in new[] {
                    new {reg= @"SOFTWARE\Microsoft\Microsoft SDKs\NETFXSDK\4.6.1\WinSDK-NetFx40Tools", file="bin\\sn.exe", view= RegistryView.Registry32 },
                    new {reg= @"SOFTWARE\Microsoft\Microsoft SDKs\NETFXSDK\4.6\WinSDK-NetFx40Tools", file="bin\\sn.exe", view= RegistryView.Registry32 },
                    new {reg= MainWindow.MSSDK_REGISTRY_KEYPATH_8_1A, file= "bin\\NETFX 4.5.1 Tools\\sn.exe", view= RegistryView.Registry64 },
                    new {reg= MainWindow.MSSDK_REGISTRY_KEYPATH, file="bin\\sn.exe", view= RegistryView.Registry64 }
                })
            {
                using (RegistryKey localMachine = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, path.view))
                {
                    using (RegistryKey masterKey = localMachine.OpenSubKey(path.reg))
                    {
                        if (masterKey == null)
                        {
                            continue;
                        }
                        string installationFolder = (string)masterKey.GetValue("InstallationFolder", null);
                        string SNCandidate = Path.Combine(installationFolder, path.file);

                        if (File.Exists(SNCandidate))
                        {
                            SNLocation = SNCandidate;
                            break;
                        }
                    }
                }
            }
            if (string.IsNullOrEmpty(SNLocation))
            {
                throw new Exception("Cannot locate sn.exe. Is VS2010 SDK installed?");
            }

            System.Diagnostics.ProcessStartInfo pinfo = new System.Diagnostics.ProcessStartInfo();
            pinfo.Arguments = "-k \"" + outputfolder + "\\AssemblySignature.snk\"";
            pinfo.CreateNoWindow = true;
            pinfo.UseShellExecute = true;
            pinfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            pinfo.FileName = SNLocation;
            System.Diagnostics.Process myProc = System.Diagnostics.Process.Start(pinfo);
        }

        public static void GenerateDomainSpecificInterface()
        {
            try
            {
                // Overwrite ParadigmName if DS interface is generated
                SolutionGenerator.ParadigmName = GeneratorFacade.main(MgaPath, TargetFolder + @"\" + SolutionName, "GME.CSharp." + SolutionName);

                if (GeneratorFacade.Errors.Count != 0)
                {
                    throw new Exception("Error occured during the domain specific interface generation.");
                }
            }
            catch (Exception)
            {
                object[] args = new object[1];
                args[0] = GeneratorFacade.Errors;
                if (GeneratorFacade.Errors.Count != 0)
                {
                    SolutionGenerator.mw.Dispatcher.Invoke(SolutionGenerator.mw.errorwindowdel, args);
                }
                throw;
            }

            // Add files to the projectfile
            string AddString = String.Empty;
            foreach (string s in GeneratorFacade.generatedFiles)
            {
                AddString += "\t<Compile Include=\"" + ParadigmName + @"\";
                AddString += System.IO.Path.GetFileName(s);
                AddString += "\" />";
                AddString += Environment.NewLine;
            }

            try
            {
                // Rename filereference in the csproj file
                StreamReader FileReadStream = new StreamReader(Path.Combine(TargetFolder, SolutionName) + @"\" + SolutionName + ".csproj");
                string ContentString = FileReadStream.ReadToEnd();
                FileReadStream.Close();

                ContentString = ContentString.Replace(@"<!--$ADDITIONALFILES$-->", AddString);

                StreamWriter FileWriteStream = new StreamWriter(Path.Combine(TargetFolder, SolutionName) + @"\" + SolutionName + ".csproj");
                FileWriteStream.Write(ContentString);
                FileWriteStream.Close();
            }
            catch (Exception)
            {
                MessageBox.Show("Error occured: Cannot find the previously generated VS projectfile in the specified folder with the specified solutionname.");
                throw;
            }

        }

        public static string AddEntryPointCode(string ContentString)
        {
            if (SolutionGenerator.SelectedInterface == ComponentInterface.Dependent)
            {
                string getrootfolder_domain_specific = @"RootFolder rf = new RootFolder(project.RootFolder);";
                getrootfolder_domain_specific += Environment.NewLine + "\t\t\t";
                getrootfolder_domain_specific += @"GMEConsole.Out.Write(rf.Name);";

                ContentString = ContentString.Replace(ENTRYPOINTCODE_REPLACESTRING, getrootfolder_domain_specific);
            }
            else // Independent
            {
                string getrootfolder_csharp = @"IMgaFolder rootFolder = project.RootFolder;";
                getrootfolder_csharp += Environment.NewLine + "\t\t\t";
                getrootfolder_csharp += @"GMEConsole.Out.WriteLine(rootFolder.Name);";

                ContentString = ContentString.Replace(ENTRYPOINTCODE_REPLACESTRING, getrootfolder_csharp);
            }

            return ContentString;
        }

    }
}
