using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Interfaces
{
	/// <summary>
	/// Represents the generic domain independent Folder.
	/// </summary>
	public interface Folder : Base, Container
	{

		/// <summary>
		/// Contains the properties of this object.
		/// </summary>
		new Classes.PropertiesFolder Properties
		{
			get;
		}
	}
}
