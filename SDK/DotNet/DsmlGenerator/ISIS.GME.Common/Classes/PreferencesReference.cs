using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Contains the reference specific preferences.
	/// </summary>
	public class PreferencesReference : PreferencesMARS
	{
		/// <summary>
		/// <para>PortLabel shown inside container?</para>
		/// <para>Portname label may be shown either inside or outside
		/// the container.</para>
		/// </summary>
		public bool IsPortLabelInside
		{
			get { return Preferences.GetBoolValueByName("portLabelInside", Impl); }
			set { Preferences.SetBoolValueByName("portLabelInside", Impl, value); }
		}

		/// <summary>
		/// <para>PortLabel Length</para>
		/// <para>Sets the text length shown when displaying portnames.
		/// If 0 total length is shown.</para>
		/// </summary>
		public int PortLabelLength
		{
			get { return Preferences.GetIntValueByName("portLabelLength", Impl); }
			set { Preferences.SetIntValueByName("portLabelLength", Impl, value); }
		}

		public PreferencesReference(global::GME.MGA.IMgaFCO impl)
			: base(impl)
		{
		}
	}
}
