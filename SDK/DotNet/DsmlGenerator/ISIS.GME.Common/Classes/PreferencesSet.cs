using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Contains the set specific preferences.
	/// </summary>
	public class PreferencesSet : PreferencesMARS
	{
		/// <summary>
		/// <para>Subtype icon name</para>
		/// <para>Image(.bmp, .gif, .png) file name to display the object if it an
		/// subtype. Icon search paths can be set in File/Settings menu.</para>
		/// </summary>
		public string SubTypeIcon
		{
			get { return Preferences.GetStrValueByName("subTypeIcon", Impl); }
			set { Preferences.SetStrValueByName("subTypeIcon", Impl, value); }
		}

		/// <summary>
		/// <para>Instance icon name</para>
		/// <para>Image(.bmp, .gif, .png) file name to display the object if it an
		/// instance. Icon search paths can be set in File/Settings menu.</para>
		/// </summary>
		public string InstanceIcon
		{
			get { return Preferences.GetStrValueByName("instanceIcon", Impl); }
			set { Preferences.SetStrValueByName("instanceIcon", Impl, value); }
		}

		public PreferencesSet(global::GME.MGA.IMgaFCO impl)
			: base(impl)
		{
		}
	}
}
