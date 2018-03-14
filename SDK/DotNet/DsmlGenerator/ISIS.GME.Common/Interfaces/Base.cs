using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;

namespace ISIS.GME.Common.Interfaces
{
    /// <summary>
    /// Represents the generic domain independent Base.
    /// </summary>
    public interface Base
    {
        global::GME.MGA.IMgaObject Impl { get; }

        void Delete();

        System.Guid Guid
        {
            get;
        }

        /// <summary>
        /// Name of the object
        /// </summary>
        string Name
        {
            get;
            set;
        }

        /// <summary>
        /// Gme identifier of the object
        /// </summary>
        string ID
        {
            get;
        }

        /// <summary>
        /// True if the object is in a library.
        /// </summary>
        bool IsLib
        {
            get;
        }

        /// <summary>
        /// Gets the parent.
        /// </summary>
        Container ParentContainer
        {
            get;
        }

        /// <summary>
        /// Gets the object path, which is not always unique.
        /// </summary>
        string Path
        {
            get;
        }

        Classes.PropertiesBase Properties
        {
            get;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="type"></param>
        void Serialize(
            System.IO.StreamWriter writer,
            Serializer.Type type = Serializer.Type.OnlyThisObject);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="type"></param>
        void Serialize(
            ISIS.GME.Common.IndentedStreamWriter writer,
            Serializer.Type type = Serializer.Type.OnlyThisObject);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="filter">
        /// Specifies a filter for the all child object.
        /// Current object will not be filtered out.
        /// Please use <paramref name="includeCurrentObject"/> = false.</param>
        /// <param name="type"></param>
        /// <param name="includeCurrentObject"></param>
        void Serialize(
            ISIS.GME.Common.IndentedStreamWriter writer,
            Func<IEnumerable<Interfaces.Base>, IEnumerable<Interfaces.Base>> filter,
            Serializer.Type type = Serializer.Type.Children,
            bool includeCurrentObject = true);

        /// <summary>
        /// <para>Traverses the subgraph using the specified options.</para>
        /// <para>Note: DFS = Depth First Search</para>
        /// </summary>
        /// <param name="filter"></param>
        /// <param name="action"></param>
        /// <param name="type"></param>
        /// <param name="includeCurrentObject"></param>
        /// <param name="indent"></param>
        void TraverseDFS(
            Func<IEnumerable<Interfaces.Base>, IEnumerable<Interfaces.Base>> filter,
            Action<ISIS.GME.Common.Interfaces.Base, int> action,
            Serializer.Type type = Serializer.Type.SubGraph,
            bool includeCurrentObject = true,
            int indent = 0);

        /// <summary>
        /// Gets the current meta kind.
        /// </summary>
        string Kind
        {
            get;
        }

    }
}
