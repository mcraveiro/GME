using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;
using System.Diagnostics.Contracts;

namespace ISIS.GME.Common.Classes
{
	public class Aspect
	{
		protected IMgaFCO Impl;

		public string Icon
		{
			get
			{
				if (Impl.ParentModel == null)
				{
					// parent is not a model
					return "";
				}
				string icon;
				int x;
				int y;
				foreach (MgaPart part in Impl.Parts)
				{
					if (part.MetaAspect.Name == Name)
					{
						part.GetGmeAttrs(out icon, out x, out y);
						return icon;
					}
				}
				if (string.IsNullOrEmpty(Name))
				{
					// if the user did not defined the name of the aspect
					MgaPart part = Impl.Parts.Cast<MgaPart>().FirstOrDefault();
					if (part != null)
					{
						part.GetGmeAttrs(out icon, out x, out y);
						return icon;
					}
					return "";
				}
				return "";
			}
			set
			{
				if (Impl.ParentModel != null)
				{
					string icon;
					int x;
					int y;
					foreach (MgaPart part in Impl.Parts)
					{
						if (part.MetaAspect.Name == Name || string.IsNullOrEmpty(Name))
						{
							part.GetGmeAttrs(out icon, out x, out y);
							part.SetGmeAttrs(value, x, y);
						}
					}
				}
			}
		}

		public string Name { get; private set; }

		public int X
		{
			get
			{
				if (Impl.ParentModel == null)
				{
					// parent is not a model
					return -3;
				}
				string icon;
				int x;
				int y;
				foreach (MgaPart part in Impl.Parts)
				{
					if (part.MetaAspect.Name == Name)
					{
						part.GetGmeAttrs(out icon, out x, out y);
						return x;
					}
				}
				if (string.IsNullOrEmpty(Name))
				{
					// if the user did not defined the name of the aspect
					MgaPart part = Impl.Parts.Cast<MgaPart>().FirstOrDefault();
					if (part != null)
					{
						part.GetGmeAttrs(out icon, out x, out y);
						return x;
					}
					// no aspect found
					return -2;
				}
				// no value found
				return -1;
			}
			set
			{
				if (Impl.ParentModel != null)
				{
					// parent is a model
					string icon;
					int x;
					int y;
					foreach (MgaPart part in Impl.Parts)
					{
						if (part.MetaAspect.Name == Name || string.IsNullOrEmpty(Name))
						{
							part.GetGmeAttrs(out icon, out x, out y);
							part.SetGmeAttrs(icon, value, y);
						}
					}
				}
			}
		}

		public int Y
		{
			get
			{
				if (Impl.ParentModel == null)
				{
					// parent is not a model
					return -3;
				}
				string icon;
				int x;
				int y;
				foreach (MgaPart part in Impl.Parts)
				{
					if (part.MetaAspect.Name == Name)
					{
						part.GetGmeAttrs(out icon, out x, out y);
						return y;
					}
				}
				if (string.IsNullOrEmpty(Name))
				{
					// if the user did not defined the name of the aspect
					MgaPart part = Impl.Parts.Cast<MgaPart>().FirstOrDefault();
					if (part != null)
					{
						part.GetGmeAttrs(out icon, out x, out y);
						return y;
					}
					return -2;
				}
				return -1;
			}
			set
			{
				if (Impl.ParentModel != null)
				{
					string icon;
					int x;
					int y;
					foreach (MgaPart part in Impl.Parts)
					{
						if (part.MetaAspect.Name == Name || string.IsNullOrEmpty(Name))
						{
							part.GetGmeAttrs(out icon, out x, out y);
							part.SetGmeAttrs(icon, x, value);
						}
					}
				}
			}
		}

		public string FormatProvider
		{
			get
			{
				throw new NotImplementedException();
			}
			set
			{
				throw new NotImplementedException();
			}
		}

		public Aspect(IMgaFCO impl, string aspectName = "")
		{
			Contract.Requires(impl != null);

			Impl = impl;
			Name = aspectName;
		}

		public static IEnumerable<Aspect> GetAspects(IMgaFCO impl)
		{
			Contract.Requires(impl != null);
			if (impl.ParentModel != null)
			{
				foreach (MgaPart part in impl.Parts)
				{
					yield return new Aspect(impl, part.MetaAspect.Name);
				}
			}
		}

		internal static void SetAspects(IMgaFCO impl, IEnumerable<Aspect> value)
		{
			Contract.Requires(impl != null);
			if (impl.ParentModel != null)
			{
				foreach (var aspect in value)
				{
					MgaPart part = impl.Parts.Cast<MgaPart>().FirstOrDefault(x => x.MetaAspect.Name == aspect.Name);
					if (part == null)
					{
						throw new ArgumentOutOfRangeException(String.Format(
							"{0} aspect was not found for {1} object.",
							aspect.Name,
							impl.Meta.Name));
					}
					else
					{
						Aspect newValue = new Aspect(impl, aspect.Name);
						newValue.Icon = aspect.Icon;
						newValue.X = aspect.X;
						newValue.Y = aspect.Y;
					}
				}
			}
		}
	}
}
