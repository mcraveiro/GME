using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Interfaces
{
	/// <summary>
	/// Represents the generic domain independent Atom.
	/// </summary>
	public interface Atom : FCO
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

		/// <summary>
		/// Contains the preferences
		/// </summary>
		Classes.PreferencesAtom Preferences
		{
			get;
		}

		/// <summary>
		/// Contains the derived instances.
		/// </summary>
		IEnumerable<ISIS.GME.Common.Interfaces.Atom> GenericInstances
		{
			get;
		}

		/// <summary>
		/// Contains the derived subtypes.
		/// </summary>
		IEnumerable<ISIS.GME.Common.Interfaces.Atom> GenericSubTypes
		{
			get;
		}
	}
}
