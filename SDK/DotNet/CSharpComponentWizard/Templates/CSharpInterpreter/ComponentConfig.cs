using System;
using System.Runtime.InteropServices;
using GME.Util;
using GME.MGA;

namespace GME.CSharp
{

    abstract class ComponentConfig
    {
        // Set paradigm name. Provide * if you want to register it for all paradigms.
        public const string paradigmName = "$paradigmname$";

        // Set the human readable name of the interpreter. You can use white space characters.
        public const string componentName = "$componentname$";

        // Specify an icon path
        public const string iconName = "$iconname$";

        public const string tooltip = "$progid$";

        // If null, updated with the assembly path + the iconName dynamically on registration
        public static string iconPath = $iconpath$;

        // Uncomment the flag if your component is paradigm independent.
        public static componenttype_enum componentType = componenttype_enum.COMPONENTTYPE_$componenttype$;

        public const regaccessmode_enum registrationMode = regaccessmode_enum.$regaccessmode$;
        public const string progID = "MGA.Interpreter.$progid$";
        public const string guid = "$guid$";
    }
}
