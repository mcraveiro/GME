using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics.Contracts;

namespace ISIS.GME.Common.Classes
{
	public abstract class PreferencesFCO
	{
		/// <summary>
		/// <para>COM object, use this if some functionality is not
		/// implemented in the class.</para>
		/// </summary>
		public global::GME.MGA.IMgaFCO Impl { get; set; }

		/// <summary>
		/// <para>Help URL</para>
		/// <para>Sets this value to specify the URL containig the help
		/// information belonging to the connection.</para>
		/// </summary>
		public Uri Help
		{
			get { return new Uri(Preferences.GetStrValueByName("help", Impl)); }
			set { Preferences.SetStrValueByName("help", Impl, value.OriginalString); }
		}

		/// <summary>
		/// <para>Description</para>
		/// <para>Free format description of the model element for
		/// documentation purposes.</para>
		/// </summary>
		public string Description
		{
			get { return Preferences.GetStrValueByName("description", Impl); }
			set { Preferences.SetStrValueByName("description", Impl, value); }
		}


		public PreferencesFCO(global::GME.MGA.IMgaFCO impl)
		{
			Contract.Requires(impl != null);

			this.Impl = impl;
		}

	}
}
