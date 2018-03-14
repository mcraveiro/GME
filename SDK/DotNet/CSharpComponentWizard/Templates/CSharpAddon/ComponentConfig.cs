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

        // Set the human readable name of the addon. You can use white space characters.
        public const string componentName = "$componentname$";

        // Select the object events you want the addon to listen to.
        public const int eventMask = (int)($eventmask$);

        // Uncomment the flag if your component is paradigm independent.
        public static componenttype_enum componentType = componenttype_enum.COMPONENTTYPE_ADDON;

        public const regaccessmode_enum registrationMode = regaccessmode_enum.$regaccessmode$;
        public const string progID = "MGA.Addon.$progid$";
        public const string guid = "$guid$";
    }
}
