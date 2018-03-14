using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics.Contracts;

namespace ISIS.GME.Common
{
	public static class Operations
	{
		// TODO: Casting problem...
		private static IEnumerable<T> FlatSubGraph<T>(
			T item)
			where T : ISIS.GME.Common.Interfaces.Base
		{
			Contract.Requires(item != null);

			yield return item;
			if (item is ISIS.GME.Common.Interfaces.Container)
			{
				foreach (T child in (item as ISIS.GME.Common.Interfaces.Container).AllChildren)
				{
					foreach (T flattenedChild in FlatSubGraph(child, x => x.AllChildren))
					{
						yield return flattenedChild;
					}
				}
			}
		}

		public static IEnumerable<T> FlatSubGraph<T>(
			T item,
			Func<ISIS.GME.Common.Interfaces.Container, IEnumerable<T>> next)
			where T : ISIS.GME.Common.Interfaces.Base
		{
			Contract.Requires(item != null);


			yield return item;
			if (item is ISIS.GME.Common.Interfaces.Container)
			{
				if (next(item as ISIS.GME.Common.Interfaces.Container) != null)
				{
					foreach (T child in next(item as ISIS.GME.Common.Interfaces.Container))
					{
						foreach (T flattenedChild in FlatSubGraph(child, next))
						{
							yield return flattenedChild;
						}
					}
				}
			}
		}


		// traversals
		// DFS with indent
		public static void TraversalDFS(
			ISIS.GME.Common.Interfaces.Base subject,
			Action<ISIS.GME.Common.Interfaces.Base, int> action)
		{
			Contract.Requires(subject != null);

			action(subject, 0);
			ChildTraversalDFS(subject, action, 1);
		}

		private static void ChildTraversalDFS(
			ISIS.GME.Common.Interfaces.Base subject,
			Action<ISIS.GME.Common.Interfaces.Base, int> action, int indent)
		{
			Contract.Requires(subject != null);

			if (subject is ISIS.GME.Common.Interfaces.Container)
			{
				foreach (ISIS.GME.Common.Classes.Base o in (subject as ISIS.GME.Common.Interfaces.Container).AllChildren.Distinct())
				{
					action(o, indent);
					ChildTraversalDFS(o, action, indent + 1);
				}
			}
		}

		// DFS without indent
		public static void TraversalDFS(
			ISIS.GME.Common.Classes.Base subject,
			Action<ISIS.GME.Common.Classes.Base> action)
		{
			Contract.Requires(subject != null);

			action(subject);
			ChildTraversalDFS(subject, action);
		}

		private static void ChildTraversalDFS(
			ISIS.GME.Common.Classes.Base subject,
			Action<ISIS.GME.Common.Classes.Base> action)
		{
			Contract.Requires(subject != null);

			if (subject is ISIS.GME.Common.Interfaces.Container)
			{
				foreach (ISIS.GME.Common.Classes.Base o in (subject as ISIS.GME.Common.Interfaces.Container).AllChildren.Distinct())
				{
					action(o);
					ChildTraversalDFS(o, action);
				}
			}
		}

	}

}
