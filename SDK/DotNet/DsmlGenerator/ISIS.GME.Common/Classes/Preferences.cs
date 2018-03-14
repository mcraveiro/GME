using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Contains constants and helper functions to get and set 
	/// preferences
	/// </summary>
	public static class Preferences
	{

		/// <summary> Default values of the preferences. TODO: Split this
		/// table into multiple subtables for only the color is the
		/// problem MARCS </summary>
		internal static Dictionary<string, string> DefaultValues =
			new Dictionary<string, string>()
		{
			{"autorouterPref"    , "neswNESW"},
			{"backgroundColor"   , "0xffffff"},
			{"borderColor"       , "0x000000"},
			{"color"             , "0xc0c0c0"},
			{"decorator"         , ""},
			{"description"       , ""},
			{"fillColor"         , "0xffffff"},
			{"gradientColor"     , "0xc0c0c0"},
			{"gradientDirection" , "0"},
			{"gradientFill"      , "false"},
			{"help"              , ""},
			{"icon"              , ""},
			{"instanceIcon"      , ""},
			{"isAutoRouted"      , "true"},
			{"isHotspotEnabled"  , "true"},
			{"isModelAutoRouted" , "true"},
			{"isNameEnabled"     , "true"},
			{"isTypeInfoShown"   , "false"},
			{"isTypeShown"       , "false"},
			{"itemShadowCast"    , "false"},
			{"nameColor"         , "0x000000"},
			{"namePosition"      , "4"},
			{"nameWrap"          , "0"},
			{"portColor"         , "0x000000"},
			{"portLabelInside"   , "true"},
			{"portLabelLength"   , "3"},
			{"porticon"          , ""},
			{"roundCornerRadius" , "9"},
			{"roundCornerRect"   , "false"},
			{"shadowColor"       , "0xc0c0c0"},
			{"shadowDirection"   , "45"},
			{"shadowThickness"   , "9"},
			{"subTypeIcon"       , ""},
			{"dstLabel1"         , "<blank>"},
			{"dstLabel2"         , "<blank>"},
			{"dstStyle"          , "butt"},
			{"labelFormatStr"    , ""},
			{"lineType"          , "solid"},
			{"srcLabel1"         , "<blank>"},
			{"srcLabel2"         , "<blank>"},
			{"srcStyle"          , "butt"},
		};

		#region Enums and factories
		private static Dictionary<LineStyle, string> LineStyleFactory =
			new Dictionary<LineStyle, string>(10)
		{
			{LineStyle.Butt, "butt"},
			{LineStyle.Arrow, "arrow"},
			{LineStyle.Diamond, "diamond"},
			{LineStyle.Apex, "apex"},
			{LineStyle.Bullet, "bullet"},
			{LineStyle.EmptyDiamond, "empty diamond"},
			{LineStyle.EmptyApex, "empty apex"},
			{LineStyle.EmptyBullet, "empty bullet"},
			{LineStyle.LeftHalfArrow, "left half arrow"},
			{LineStyle.RightHalfArrow, "right half arrow"},
		};

		public enum LineStyle
		{
			Butt,
			Arrow,
			Diamond,
			Apex,
			Bullet,
			EmptyDiamond,
			EmptyApex,
			EmptyBullet,
			LeftHalfArrow,
			RightHalfArrow,
		}

		private static Dictionary<LineType, string> LineTypeFactory =
			new Dictionary<LineType, string>(2)
		{
			{LineType.Solid, "solid"},
			{LineType.Dashed, "dash"},
		};

		public enum LineType
		{
			Solid,
			Dashed,
		}


		public enum NamePosition
		{

			/// <summary>
			/// North, 0
			/// </summary>
			North = 0,

			/// <summary>
			/// Northeast, 1
			/// </summary>
			Northeast = 1,

			/// <summary>
			/// East, 2
			/// </summary>
			East = 2,

			/// <summary>
			/// Southeast, 3
			/// </summary>
			Southeast = 3,

			/// <summary>
			/// South, 4
			/// </summary>
			South = 4,

			/// <summary>
			/// Southwest, 5
			/// </summary>
			Southwest = 5,

			/// <summary>
			/// West, 6
			/// </summary>
			West = 6,

			/// <summary>
			/// Northwest, 7
			/// </summary>
			Northwest = 7,

			/// <summary>
			/// Center, 8
			/// </summary>
			Center = 8,
		}
		#endregion


		#region Helper functions for the COM interface
		public static string GetStrValueByName(
			string regName,
			global::GME.MGA.IMgaFCO subject)
		{
			string regValue = subject.RegistryValue[regName];

			if (string.IsNullOrEmpty(regValue))
			{
				return DefaultValues[regName];
			}
			else
			{
				return regValue;
			}
		}

		public static void SetStrValueByName(
			string regName,
			global::GME.MGA.IMgaFCO subject,
			string value)
		{
			subject.RegistryValue[regName] = value;
		}

		public static bool GetBoolValueByName(
			string regName,
			global::GME.MGA.IMgaFCO subject)
		{
			string regValue = subject.RegistryValue[regName];
			bool result;

			if (string.IsNullOrEmpty(regValue))
			{
				regValue = DefaultValues[regName];
			}

			if (bool.TryParse(regValue, out result))
			{
				return result;
			}
			else
			{
				throw new FormatException(String.Format(
					"{0} = {1} could not be parsed as {2}.",
					regName,
					regValue,
					result.GetType().FullName));
			}
		}

		public static void SetBoolValueByName(
			string regName,
			global::GME.MGA.IMgaFCO subject,
			bool value)
		{
			subject.RegistryValue[regName] = value.ToString().ToLowerInvariant();
		}


		public static int GetIntValueByName(
			string regName,
			global::GME.MGA.IMgaFCO subject)
		{
			string regValue = subject.RegistryValue[regName];
			int result;

			if (string.IsNullOrEmpty(regValue))
			{
				regValue = DefaultValues[regName];
			}

			if (int.TryParse(regValue, out result))
			{
				return result;
			}
			else
			{
				throw new FormatException(String.Format(
					"{0} = {1} could not be parsed as {2}.",
					regName,
					regValue,
					result.GetType().FullName));
			}
		}

		public static void SetIntValueByName(
			string regName,
			global::GME.MGA.IMgaFCO subject,
			int value)
		{
			subject.RegistryValue[regName] = value.ToString();
		}

		public static NamePosition GetNamePosition(string regName, global::GME.MGA.IMgaFCO subject)
		{
			string regValue = subject.RegistryValue[regName];
			NamePosition result;
			int enumValue = 0;

			if (string.IsNullOrEmpty(regValue))
			{
				regValue = DefaultValues[regName];
			}

			enumValue = int.Parse(regValue);
			string enumName = Enum.GetName(typeof(NamePosition), enumValue);

			if (Enum.TryParse(enumName, out result))
			{
				return result;
			}
			else
			{
				throw new FormatException(String.Format(
					"{0} = {1} could not be parsed as {2}.",
					regName,
					regValue,
					result.GetType().FullName));
			}
		}

		public static System.Drawing.Color GetColorValueByName(
			string regName,
			global::GME.MGA.IMgaFCO subject)
		{
			string regValue = subject.RegistryValue[regName];
			System.Drawing.Color result = System.Drawing.Color.Gray;

			if (string.IsNullOrEmpty(regValue))
			{
				regValue = DefaultValues[regName];
			}
			int color = Convert.ToInt32(regValue.Substring("0x".Length), 16);
			result = System.Drawing.ColorTranslator.FromWin32(color);

			return result;
		}

		public static void SetColorValueByName(
			string regName,
			global::GME.MGA.IMgaFCO subject,
			System.Drawing.Color value)
		{
			subject.RegistryValue[regName] = String.Format(
				"0x{0:x2}{1:x2}{2:x2}",
				value.R,
				value.G,
				value.B);
		}

		#endregion
	}
}
