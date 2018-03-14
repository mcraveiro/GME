using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;

namespace ISIS.GME.Common.Interfaces
{
    /// <summary>
    /// Represents the generic domain independent RootFolder.
    /// </summary>
    public interface RootFolder : Folder
    {
        RootFolder AttachLibrary(string connectionString);

        System.Collections.Generic.Dictionary<int, System.Type> MetaRefs
        {
            get;
        }
    }
}
