using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common
{
	/// <summary>
	/// Contains the generic settings. E.g. path delimiter.
	/// </summary>
	public static class Settings
	{

		private static string pathDelimiter = "/";

		public static string PathDelimiter
		{
			get { return pathDelimiter; }
			set { pathDelimiter = value; }
		}


	}
}
