using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Represents the generic domain independent Base object.
	/// </summary>
	public class Base : ISIS.GME.Common.Interfaces.Base
	{
		public global::GME.MGA.IMgaObject Impl { get; set; }

		public virtual void Delete()
		{
			Impl.DestroyObject();
		}

		public Guid Guid
		{
			get { return new Guid(Impl.GetGuidDisp()); }
		}

		public string Name
		{
			get { return Impl.Name; }
			set { Impl.Name = value; }
		}

		public string ID
		{
			get { return Impl.ID; }
		}

		public bool IsLib
		{
			get { return Impl.IsLibObject; }
		}

		public virtual ISIS.GME.Common.Interfaces.Container ParentContainer
		{
			get
			{
				MgaObject parent;
				global::GME.MGA.Meta.objtype_enum type;
				Impl.GetParent(out parent, out type);
				if (parent == null)
				{
					return null;
				}
				else if (parent is global::GME.MGA.IMgaFolder)
				{
					return Utils.CreateObject<Folder>(parent);
				}
				else if (parent is global::GME.MGA.IMgaModel)
				{
					return Utils.CreateObject<Model>(parent);
				}
				return null;
			}
		}

		public string Path
		{
			get
			{
				Interfaces.Base parent = this.ParentContainer;
				Stack<Interfaces.Base> stack = new Stack<Interfaces.Base>();
				StringBuilder sb = new StringBuilder();

				stack.Push(this);

				while (parent != null)
				{
					stack.Push(parent);
					parent = parent.ParentContainer;
				}

				foreach (var item in stack)
				{
					if (item != stack.Peek())
					{
						sb.Append(ISIS.GME.Common.Settings.PathDelimiter);
					}
					sb.Append(item.Name);
				}

				return sb.ToString();
			}
		}

		public PropertiesBase Properties
		{
			get { return new PropertiesBase(Impl); }
		}

		public virtual void Serialize(
			System.IO.StreamWriter writer,
			Serializer.Type type = Serializer.Type.OnlyThisObject)
		{
			// serialize this object
			writer.WriteLine(Name);

			if ((type == Serializer.Type.SubGraph ||
				type == Serializer.Type.Children) &&
				this is Interfaces.Container)
			{
				if ((this as Interfaces.Container).AllChildren == null)
				{
					int zso = 0;
				}
				// serialize child objects
				foreach (Interfaces.Base b in (this as Interfaces.Container).AllChildren)
				{
					if (type == Serializer.Type.SubGraph)
					{
						b.Serialize(writer, type);
					}
					else if (type == Serializer.Type.Children)
					{
						b.Serialize(writer, Serializer.Type.OnlyThisObject);
					}
				}
			}
		}

		public virtual void Serialize(
			ISIS.GME.Common.IndentedStreamWriter writer,
			Serializer.Type type = Serializer.Type.OnlyThisObject)
		{
			// serialize this object
			writer.WriteLine(Name, true);

			if ((type == Serializer.Type.SubGraph ||
				type == Serializer.Type.Children) &&
				this is Interfaces.Container)
			{
				writer.Depth++;
				// serialize child objects
				foreach (Interfaces.Base b in (this as Interfaces.Container).AllChildren)
				{
					if (type == Serializer.Type.SubGraph)
					{
						b.Serialize(writer, type);
					}
					else if (type == Serializer.Type.Children)
					{
						b.Serialize(writer, Serializer.Type.OnlyThisObject);
					}
				}
				writer.Depth--;
			}
		}

		public virtual void Serialize(
			ISIS.GME.Common.IndentedStreamWriter writer,
			Func<IEnumerable<Interfaces.Base>, IEnumerable<Interfaces.Base>> filter,
			Serializer.Type type = Serializer.Type.Children,
			bool includeCurrentObject = true)
		{
			if (includeCurrentObject)
			{
				// serialize this object
				writer.WriteLine(Name, true);
			}

			if ((type == Serializer.Type.SubGraph ||
				type == Serializer.Type.Children) &&
				this is Interfaces.Container)
			{
				writer.Depth++;
				// serialize child objects
				var filtered = filter((this as Interfaces.Container).AllChildren);
				bool include = false;
				foreach (Interfaces.Base b in (this as Interfaces.Container).AllChildren)
				{
					if (filtered.Contains(b))
					{
						include = true;
					}
					else
					{
						include = false;
					}
					if (type == Serializer.Type.SubGraph)
					{
						b.Serialize(writer, filter, type, include);
					}
					else if (type == Serializer.Type.Children)
					{
						b.Serialize(writer, filter, Serializer.Type.OnlyThisObject, include);
					}
				}
				writer.Depth--;
			}
		}

		public virtual void TraverseDFS(
			Func<IEnumerable<Interfaces.Base>, IEnumerable<Interfaces.Base>> filter,
			Action<ISIS.GME.Common.Interfaces.Base, int> action,
			Serializer.Type type = Serializer.Type.SubGraph,
			bool includeCurrentObject = true,
			int indent = 0)
		{
			if (includeCurrentObject)
			{
				// call user's function
				action(this, indent);
			}

			if ((type == Serializer.Type.SubGraph ||
				type == Serializer.Type.Children) &&
				this is Interfaces.Container)
			{
				// get the user defined filtered list
				var filtered = filter((this as Interfaces.Container).AllChildren).ToList();
				// TODO: split the foreach into 2 part.
				var regular = (this as Interfaces.Container).AllChildren.Where(x => filtered.FirstOrDefault(y => y.ID == x.ID) == null);
				bool include = false;

				foreach (Interfaces.Base b in (this as Interfaces.Container).AllChildren)
				{
					if (filtered.FirstOrDefault(y => y.ID == b.ID) != null)
					{
						include = true;
					}
					else
					{
						include = false;
					}
					if (type == Serializer.Type.SubGraph)
					{
						// call recursively
						b.TraverseDFS(
							filter,
							action,
							type,
							include,
							indent + 1);
					}
					else if (type == Serializer.Type.Children)
					{
						// next object is the last one
						b.TraverseDFS(
							filter,
							action,
							Serializer.Type.OnlyThisObject,
							include,
							indent + 1);
					}
				}
			}
		}

		public string Kind
		{
			get { return Impl.MetaBase.Name; }
		}

        public override bool Equals(object obj)
        {
            if (obj == null)
                return false;
            if (!(obj is Base))
                return false;
            Base that = obj as Base;
            if (Impl == null && that.Impl == null)
                return true;
            if (Impl == null || that.Impl == null)
                return false;
            return Impl.ID == that.Impl.ID;
        }

        public override int GetHashCode()
        {
            if (Impl == null)
                return 0;
            return Impl.ID.GetHashCode();
        }
	}
}
