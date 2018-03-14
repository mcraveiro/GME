using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Represents the generic domain independent Folder.
	/// </summary>
	public class Folder : Base, ISIS.GME.Common.Interfaces.Folder
	{
		public static Interfaces.Folder CreateGeneric(Interfaces.Folder parent)
		{
			return ISIS.GME.Common.Utils.CreateObject<Folder>(parent);
		}

		public virtual IEnumerable<ISIS.GME.Common.Interfaces.Base> AllChildren
		{
			get
			{
				throw new NotSupportedException(
					"This property must be implemented in the derived class.");
			}
		}

		public new PropertiesFolder Properties
		{
			get { return new PropertiesFolder(Impl); }
		}
	}
}
