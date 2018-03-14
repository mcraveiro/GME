using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;
using System.Diagnostics.Contracts;

namespace ISIS.GME.Common.Classes
{
	public class Set : FCO, ISIS.GME.Common.Interfaces.Set
	{
		public static Interfaces.Set CreateGeneric(Interfaces.Container parent, string roleStr = null)
		{
			return ISIS.GME.Common.Utils.CreateObject<Set>(parent, roleStr);
		}

		public virtual Aspect GenericAspect
		{
			get
			{
				return new Aspect(Impl as IMgaFCO);
			}
		}

		public virtual IEnumerable<Aspect> Aspects
		{
			get
			{
				return Aspect.GetAspects(Impl as IMgaFCO);
			}
			set
			{
				Aspect.SetAspects(Impl as IMgaFCO, value);
			}
		}

		public IEnumerable<Interfaces.FCO> GenericMembers
		{
			get { return ISIS.GME.Common.Utils.CastSetMembers<Classes.FCO>(Impl as MgaSet); }
			set { throw new NotImplementedException(); }
		}

		public PreferencesSet Preferences
		{
			get { return new PreferencesSet(Impl as global::GME.MGA.IMgaFCO); }
		}

		/// <summary>
		/// 
		/// </summary>
		public virtual IEnumerable<ISIS.GME.Common.Interfaces.Set> GenericInstances
		{
			get
			{
				Contract.Requires(Impl != null);
				Contract.Requires(Impl is IMgaFCO);

				foreach (IMgaFCO item in (Impl as IMgaFCO).DerivedObjects)
				{
					if (item.IsInstance)
					{
						ISIS.GME.Common.Classes.Set result = new Set();
						result.Impl = item as IMgaObject;
						yield return result;
					}
				}
			}
		}

		/// <summary>
		/// 
		/// </summary>
		public virtual IEnumerable<ISIS.GME.Common.Interfaces.Set> GenericSubTypes
		{
			get
			{
				Contract.Requires(Impl != null);
				Contract.Requires(Impl is IMgaFCO);

				foreach (IMgaFCO item in (Impl as IMgaFCO).DerivedObjects)
				{
					if (item.IsInstance ? false : item.ArcheType != null)
					{
						// if subtype
						ISIS.GME.Common.Classes.Set result = new Set();
						result.Impl = item as IMgaObject;
						yield return result;
					}
				}
			}
		}
	}
}
