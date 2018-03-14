using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Contains the connection specific preferences.
	/// </summary>
	public class PreferencesConnection : PreferencesFCO
	{
		/// <summary>
		/// <para>Color</para>
		/// <para>Connection line color. Set this value to change the
		/// connection line color.</para>
		/// </summary>
		public System.Drawing.Color Color
		{
			get { return Preferences.GetColorValueByName("color", Impl); }
			set { Preferences.SetColorValueByName("color", Impl, value); }
		}

		/// <summary>
		/// <para>Name color</para>
		/// <para>Connection label color. Set this value to change the
		/// connection label color.</para>
		/// </summary>
		public System.Drawing.Color NameColor
		{
			get { return Preferences.GetColorValueByName("nameColor", Impl); }
			set { Preferences.SetColorValueByName("nameColor", Impl, value); }
		}

		/// <summary>
		/// <para>Source end style</para>
		/// <para>Sets this value to specify the source end style
		/// of the connection. Eg. Arrow.</para>
		/// </summary>
		public Preferences.LineStyle SrcStyle
		{
			get { throw new NotImplementedException(); }
			set { throw new NotImplementedException(); }
		}

		/// <summary>
		/// <para>Destination end style</para>
		/// <para>Sets this value to specify the destination end style
		/// of the connection. Eg. Arrow.</para>
		/// </summary>
		public Preferences.LineStyle DstStyle
		{
			get { throw new NotImplementedException(); }
			set { throw new NotImplementedException(); }
		}

		/// <summary>
		/// <para>Line type</para>
		/// <para>Sets this value to specify the connection line type.
		/// Eg. Dashed.</para>
		/// </summary>
		public Preferences.LineType LineType
		{
			get { throw new NotImplementedException(); }
			set { throw new NotImplementedException(); }
		}

		/// <summary>
		/// <para>Label format string</para>
		/// <para>Sets this value to form a centered connection label
		/// from the connection attributes. Eg. %name%</para>
		/// </summary>
		public string LabelFormatStr
		{
			get { return Preferences.GetStrValueByName("labelFormatStr", Impl); }
			set { Preferences.SetStrValueByName("labelFormatStr", Impl, value); }
		}

		/// <summary>
		/// <para>Source primary attribute</para>
		/// <para>Displays an attribute as a label on the source side
		/// of the connection.</para>
		/// </summary>
		public string SrcPrimaryLabel
		{
			get { return Preferences.GetStrValueByName("srcLabel1", Impl); }
			set { Preferences.SetStrValueByName("srcLabel1", Impl, value); }
		}

		/// <summary>
		/// <para>Source secondary attribute</para>
		/// <para>Displays an attribute as a label on the source
		/// side of the connection.</para>
		/// </summary>
		public string SrcSecondaryLabel
		{
			get { return Preferences.GetStrValueByName("srcLabel2", Impl); }
			set { Preferences.SetStrValueByName("srcLabel2", Impl, value); }
		}

		/// <summary>
		/// <para>Destination primary attribute</para>
		/// <para>Displays an attribute as a label on the destination
		/// side of the connection.</para>
		/// </summary>
		public string DstPrimaryLabel
		{
			get { return Preferences.GetStrValueByName("dstLabel1", Impl); }
			set { Preferences.SetStrValueByName("dstLabel1", Impl, value); }
		}

		/// <summary>
		/// <para>Destination secondary attribute</para>
		/// <para>Displays an attribute as a label on the destination
		/// side of the connection.</para>
		/// </summary>
		public string DstSecondaryLabel
		{
			get { return Preferences.GetStrValueByName("dstLabel2", Impl); }
			set { Preferences.SetStrValueByName("dstLabel2", Impl, value); }
		}

		/// <summary>
		/// <para>Is auto routed</para>
		/// <para>Is the connection treated by the auto router.</para>
		/// </summary>
		public bool IsAutoRouted
		{
			get { return Preferences.GetBoolValueByName("isAutoRouted", Impl); }
			set { Preferences.SetBoolValueByName("isAutoRouted", Impl, value); }
		}

		public PreferencesConnection(global::GME.MGA.IMgaFCO impl)
			: base(impl)
		{
		}
	}
}
