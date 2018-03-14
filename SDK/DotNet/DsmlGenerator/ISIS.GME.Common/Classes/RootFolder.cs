using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Represents the generic domain independent RootFolder.
	/// </summary>
	public class RootFolder : Folder, Interfaces.RootFolder
	{
		public Interfaces.RootFolder AttachLibrary(string connectionString)
		{
			try
			{
				RootFolder rf = new RootFolder();
				rf.Impl = Impl.Project.RootFolder.AttachLibraryV3(connectionString, true) as IMgaObject;
				return rf;
			}
			catch (Exception ex)
			{
				throw ex;
			}
		}

		public override void Delete()
		{
			if (ParentContainer != null)
			{
				// this is a library and remove it.
				Impl.DestroyObject();
			}
			else
			{
				throw new Exception("Project's root folder could not be deleted.");
			}
		}

		public static Interfaces.RootFolder GetRootFolder(MgaProject project)
		{
			return ISIS.GME.Common.Utils.CreateObject<RootFolder>(project.RootFolder as IMgaObject);
		}

        public virtual System.Collections.Generic.Dictionary<int, System.Type> MetaRefs
        {
            get
            {
                return new Dictionary<int, Type>();
            }
        }
	}
}
