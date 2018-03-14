using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Interfaces
{
	/// <summary>
	/// Represents the generic domain independent Set.
	/// </summary>
	public interface Set : FCO
	{
		ISIS.GME.Common.Classes.Aspect GenericAspect
		{
			get;
		}

		IEnumerable<ISIS.GME.Common.Classes.Aspect> Aspects
		{
			get;
			set;
		}

		IEnumerable<Interfaces.FCO> GenericMembers
		{
			get;
			set;
		}

		/// <summary>
		/// Contains the preferences
		/// </summary>
		Classes.PreferencesSet Preferences
		{
			get;
		}

		/// <summary>
		/// Contains the derived instances.
		/// </summary>
		IEnumerable<ISIS.GME.Common.Interfaces.Set> GenericInstances
		{
			get;
		}

		/// <summary>
		/// Contains the derived subtypes.
		/// </summary>
		IEnumerable<ISIS.GME.Common.Interfaces.Set> GenericSubTypes
		{
			get;
		}
	}
}
