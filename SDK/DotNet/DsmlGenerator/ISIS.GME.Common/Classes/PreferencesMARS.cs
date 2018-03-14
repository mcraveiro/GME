using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Contains the common preferences of MARS (Model Atom Reference Set)
	/// </summary>
	public class PreferencesMARS : PreferencesFCO
	{
		/// <summary>
		/// <para>Color</para>
		/// <para>Object color. Set this value to change the object color.</para>
		/// </summary>
		public System.Drawing.Color Color
		{
			get { return Preferences.GetColorValueByName("color", Impl); }
			set { Preferences.SetColorValueByName("color", Impl, value); }
		}

		/// <summary>
		/// <para>Name color</para>
		/// <para>Object name label color. Set this value to change
		/// the object name label color.</para>
		/// </summary>
		public System.Drawing.Color NameColor
		{
			get { return Preferences.GetColorValueByName("nameColor", Impl); }
			set { Preferences.SetColorValueByName("nameColor", Impl, value); }
		}

		/// <summary>
		/// <para>Color</para>
		/// <para>Object background fill color for the closed models.</para>
		/// </summary>
		public System.Drawing.Color FillColor
		{
			get { return Preferences.GetColorValueByName("fillColor", Impl); }
			set { Preferences.SetColorValueByName("fillColor", Impl, value); }
		}

		/// <summary>
		/// <para>Gradient fill enabled</para>
		/// <para>Is the gradient fill feature enabled.</para>
		/// </summary>
		public bool IsGradientFillEnabled
		{
			get { return Preferences.GetBoolValueByName("gradientFill", Impl); }
			set { Preferences.SetBoolValueByName("gradientFill", Impl, value); }
		}

		/// <summary>
		/// <para>Gradient (2nd fill) color</para>
		/// <para>Object gradient background color (the other fill color).</para>
		/// </summary>
		public System.Drawing.Color GradientFillColor
		{
			get { return Preferences.GetColorValueByName("gradientColor", Impl); }
			set { Preferences.SetColorValueByName("gradientColor", Impl, value); }
		}

		/// <summary>
		/// <para>Gradient direction (angle)</para>
		/// <para>Direction of the gradient fill.</para>
		/// </summary>
		public int GradientFillDirection
		{
			get { return Preferences.GetIntValueByName("gradientDirection", Impl); }
			set { Preferences.SetIntValueByName("gradientDirection", Impl, value); }
		}

		/// <summary>
		/// <para>Shadow casting enabled</para>
		/// <para>Is the shadow casting feature enabled.</para>
		/// </summary>
		public bool IsShadowCastEnabled
		{
			get { return Preferences.GetBoolValueByName("itemShadowCast", Impl); }
			set { Preferences.SetBoolValueByName("itemShadowCast", Impl, value); }
		}

		/// <summary>
		/// <para>Shadow color</para>
		/// <para>Object shadow color.</para>
		/// </summary>
		public System.Drawing.Color ShadowColor
		{
			get { return Preferences.GetColorValueByName("shadowColor", Impl); }
			set { Preferences.SetColorValueByName("shadowColor", Impl, value); }
		}

		/// <summary>
		/// <para>Shadow thickness/offset</para>
		/// <para>Thickness/offset of the shadow.</para>
		/// </summary>
		public int ShadowThickness
		{
			get { return Preferences.GetIntValueByName("shadowThickness", Impl); }
			set { Preferences.SetIntValueByName("shadowThickness", Impl, value); }
		}

		/// <summary>
		/// <para>Shadow direction (angle)</para>
		/// <para>Direction of the shadow (angle).</para>
		/// </summary>
		public int ShadowDirection
		{
			get { return Preferences.GetIntValueByName("shadowDirection", Impl); }
			set { Preferences.SetIntValueByName("shadowDirection", Impl, value); }
		}

		/// <summary>
		/// <para>Round rectangle corner enabled</para>
		/// <para>Is rounding of rectangle corner feature enabled.</para>
		/// </summary>
		public bool IsRoundRectangleEnabled
		{
			get { return Preferences.GetBoolValueByName("roundCornerRect", Impl); }
			set { Preferences.SetBoolValueByName("roundCornerRect", Impl, value); }
		}

		/// <summary>
		/// <para>Round rectangle corner radius</para>
		/// <para>Radius of round rectangle corner.</para>
		/// </summary>
		public int RoundRectangleCornerRadius
		{
			get { return Preferences.GetIntValueByName("roundCornerRadius", Impl); }
			set { Preferences.SetIntValueByName("roundCornerRadius", Impl, value); }
		}

		/// <summary>
		/// <para>Name Location</para>
		/// <para>Sets the name label location relative to the atom icon.
		/// Eg. North.</para>
		/// </summary>
		public Preferences.NamePosition NameLocation
		{
			get { return Preferences.GetNamePosition("namePosition", Impl); }
			set { throw new NotImplementedException(); }
		}

		/// <summary>
		/// <para>NameWrap number</para>
		/// <para>Sets the number of characters in a line of the name. If it
		/// is 0, then wrapping is disabled.</para>
		/// </summary>
		public int NameWrap
		{
			get { return Preferences.GetIntValueByName("nameWrap", Impl); }
			set { Preferences.SetIntValueByName("nameWrap", Impl, value); }
		}

		/// <summary>
		/// <para>Name enabled</para>
		/// <para>Displays the name.</para>
		/// </summary>
		public bool IsNameEnabled
		{
			get { return Preferences.GetBoolValueByName("isNameEnabled", Impl); }
			set { Preferences.SetBoolValueByName("isNameEnabled", Impl, value); }
		}

		/// <summary>
		/// <para>Auto Router Preference</para>
		/// <para>Sets the allowed stick point of the connections from
		/// and to this object.</para>
		/// </summary>
		public string AutoRouterPref
		{
			// TODO create a bit field
			get { throw new NotSupportedException(); }
				//return Preferences.GetStrValueByName("autorouterPref", Impl); }
			set { throw new NotSupportedException(); }
		}

		/// <summary>
		/// <para>Hotspots enabled</para>
		/// <para>Enables the hotspot feature in connection mode.</para>
		/// </summary>
		public bool IsHotspotEnabled
		{
			get { return Preferences.GetBoolValueByName("isHotspotEnabled", Impl); }
			set { Preferences.SetBoolValueByName("isHotspotEnabled", Impl, value); }
		}

		/// <summary>
		/// <para>Type displayed</para>
		/// <para>Displays the type (subtype) if the object is an Instance</para>
		/// </summary>
		public bool IsTypeShown
		{
			get { return Preferences.GetBoolValueByName("isTypeShown", Impl); }
			set { Preferences.SetBoolValueByName("isTypeShown", Impl, value); }
		}

		/// <summary>
		/// <para>Decorator</para>
		/// <para>ProgID of an process COM component followed by optional
		/// parameters for installing  custom drawing code.</para>
		/// <para>Syntax: &lt;ProgID&gt; [&lt;Param1=Value&gt;...&lt;ParamX=ValueX&gt;]</para>
		/// </summary>
		public string Decorator
		{
			get { return Preferences.GetStrValueByName("decorator", Impl); }
			set { Preferences.SetStrValueByName("decorator", Impl, value); }
		}

		/// <summary>
		/// <para>Icon name</para>
		/// <para>Image(.bmp, .gif, .png) file name to display the object.
		/// Icon search paths can be set in File/Settings menu.</para>
		/// </summary>
		public string Icon
		{
			get { return Preferences.GetStrValueByName("icon", Impl); }
			set { Preferences.SetStrValueByName("icon", Impl, value); }
		}

		/// <summary>
		/// <para>Port icon name</para>
		/// <para>Specifies the image(.bmp, .gif, .png) file name to display.</para>
		/// </summary>
		public string PortIcon
		{
			get { return Preferences.GetStrValueByName("porticon", Impl); }
			set { Preferences.SetStrValueByName("porticon", Impl, value); }
		}


		public PreferencesMARS(global::GME.MGA.IMgaFCO impl)
			: base(impl)
		{
		}
	}
}
