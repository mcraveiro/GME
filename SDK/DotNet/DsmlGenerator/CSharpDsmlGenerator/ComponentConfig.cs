using System;
using System.Runtime.InteropServices;
using GME.Util;
using GME.MGA;

namespace GME.CSharp
{
    
    abstract class ComponentConfig
    {
        // Set paradigm name. Provide * if you want to register it for all paradigms.
		public const string paradigmName = "MetaGME";
		
		// Set the human readable name of the interpreter. You can use white space characters.
        public const string componentName = "CSharp (.Net) Dsml API Generator";
        public const string tooltip = componentName;
        
		// Specify an icon path
		public const string iconName = "CSharpDSMLGenerator.ico";
        
		// If null, updated with the assembly path + the iconName dynamically on registration
        public static string iconPath = null; 
        
		// Uncomment the flag if your component is paradigm independent.
		public static componenttype_enum componentType = componenttype_enum.COMPONENTTYPE_INTERPRETER;
				
        public const regaccessmode_enum registrationMode = regaccessmode_enum.REGACCESS_SYSTEM;
        public const string progID = "MGA.Interpreter.CSharpDSMLGenerator";
        public const string guid = "78BE7B95-3564-4BA9-8FE6-8D9B91EEE0B8";
    }
}
