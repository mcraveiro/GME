using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;
using System.Diagnostics.Contracts;

namespace ISIS.GME.Common.Classes
{
    public class Model : FCO, ISIS.GME.Common.Interfaces.Model
    {
        public static Interfaces.Model CreateGeneric(Interfaces.Container parent, string roleStr = null)
        {
            return ISIS.GME.Common.Utils.CreateObject<Model>(parent, roleStr);
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

        public virtual IEnumerable<ISIS.GME.Common.Interfaces.Base> AllChildren
        {
            get
            {
                throw new NotSupportedException(
                    "This property must be implemented in the derived class.");
            }
        }

        public PreferencesModel Preferences
        {
            get { return new PreferencesModel(Impl as global::GME.MGA.IMgaFCO); }
        }

        /// <summary>
        /// 
        /// </summary>
        public virtual IEnumerable<ISIS.GME.Common.Interfaces.Model> GenericInstances
        {
            get
            {
                Contract.Requires(Impl != null);
                Contract.Requires(Impl is IMgaFCO);

                foreach (IMgaFCO item in (Impl as IMgaFCO).DerivedObjects)
                {
                    if (item.IsInstance)
                    {
                        ISIS.GME.Common.Classes.Model result = new Model();
                        result.Impl = item as IMgaObject;
                        yield return result;
                    }
                }
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public virtual IEnumerable<ISIS.GME.Common.Interfaces.Model> GenericSubTypes
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
                        ISIS.GME.Common.Classes.Model result = new Model();
                        result.Impl = item as IMgaObject;
                        yield return result;
                    }
                }
            }
        }


        ///// <summary>
        ///// This property must be implemented in the derived class.
        ///// </summary>
        //public virtual IEnumerable<ISIS.GME.Common.Interfaces.Model> DerivedCollection
        //{
        //  get
        //  {
        //    throw new NotSupportedException(
        //      "This property must be implemented in the derived class.");
        //  }
        //}

    }
}
