using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Contains the folder specific properties.
	/// </summary>
	public class PropertiesFolder : PropertiesBase
	{
		/// <summary>
		/// Library is a folder incorporating an MGA file using
		/// the same paradigm as meta.
		/// </summary>
		public string LibraryName { get { throw new NotImplementedException(); } }

		public PropertiesFolder(global::GME.MGA.IMgaObject impl)
			: base(impl)
		{
		}
	}
}
