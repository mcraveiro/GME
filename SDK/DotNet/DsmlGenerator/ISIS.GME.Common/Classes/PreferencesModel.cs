using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Contains the model specific preferences.
	/// </summary>
	public class PreferencesModel : PreferencesMARS
	{
		/// <summary>
		/// <para>Background color</para>
		/// <para>Window background color for the open models.</para>
		/// </summary>
		public System.Drawing.Color BackgroundColor
		{
			get { return Preferences.GetColorValueByName("backgroundColor", Impl); }
			set { Preferences.SetColorValueByName("backgroundColor", Impl, value); }
		}

		/// <summary>
		/// <para>Border color</para>
		/// <para>Model border color. Set this value to change the model
		/// border color.</para>
		/// </summary>
		public System.Drawing.Color BorderColor
		{
			get { return Preferences.GetColorValueByName("borderColor", Impl); }
			set { Preferences.SetColorValueByName("borderColor", Impl, value); }
		}

		/// <summary>
		/// <para>Type displayed</para>
		/// <para>Displays the type (subtype) if the model is an Instance</para>
		/// </summary>
		public bool IsTypeInfoShown
		{
			get { return Preferences.GetBoolValueByName("isTypeInfoShown", Impl); }
			set { Preferences.SetBoolValueByName("isTypeInfoShown", Impl, value); }
		}

		/// <summary>
		/// <para>Is auto routed</para>
		/// <para>Are the models' connections treated by the auto router
		/// by default.</para>
		/// </summary>
		public bool IsModelAutoRouted
		{
			get { return Preferences.GetBoolValueByName("isModelAutoRouted", Impl); }
			set { Preferences.SetBoolValueByName("isModelAutoRouted", Impl, value); }
		}

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

		public PreferencesModel(global::GME.MGA.IMgaFCO impl)
			: base (impl)
		{
		}
	}
}
