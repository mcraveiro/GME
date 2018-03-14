using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Interfaces
{
	/// <summary>
	/// Represents the generic domain independent Reference.
	/// </summary>
	public interface Reference : FCO
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

		ISIS.GME.Common.Interfaces.FCO GenericReferred
		{
			get;
			set;
		}

		/// <summary>
		/// Contains the preferences
		/// </summary>
		Classes.PreferencesReference Preferences
		{
			get;
		}

		/// <summary>
		/// Contains the derived instances.
		/// </summary>
		IEnumerable<ISIS.GME.Common.Interfaces.Reference> GenericInstances
		{
			get;
		}

		/// <summary>
		/// Contains the derived subtypes.
		/// </summary>
		IEnumerable<ISIS.GME.Common.Interfaces.Reference> GenericSubTypes
		{
			get;
		}
	}
}
