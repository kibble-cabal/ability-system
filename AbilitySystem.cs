using Godot;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace AS
{

    public interface IInstanceWrapper<T> where T : GodotObject
    {
        T Instance { get; set; }

        void SetInstance(T instance)
        {
            ArgumentNullException.ThrowIfNull(instance);
            if (!ClassDB.IsParentClass(instance.GetClass(), GetType().Name)) throw new ArgumentException("\"instance\" has the wrong type.");
            Instance = instance;
        }
    }

    internal static class Utils
    {
        static readonly Dictionary<ulong, dynamic> CachedInstances = [];
        static DateTime PreviousClearTime = DateTime.Now;

        public static T? CreateWrapperFromObject<T>(GodotObject instance) where T : class
        {
            if (instance == null)
                return null;

            ulong id = instance.GetInstanceId();
            if (CachedInstances.TryGetValue(id, out dynamic? value))
                return (T)value;

            if ((DateTime.Now - PreviousClearTime).TotalSeconds > 1)
            {
                var query = CachedInstances.Where((i) => GodotObject.IsInstanceIdValid(i.Key)).ToArray();
                foreach (var i in query)
                    CachedInstances.Remove(i.Key);
                PreviousClearTime = DateTime.Now;
            }

            switch (instance.GetClass())
            {
                case "Ability":
                    {
                        var newInstance = new Ability((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "AbilityEvent":
                    {
                        var newInstance = new AbilityEvent((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "AbilitySystem":
                    {
                        var newInstance = new AbilitySystem((Node)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "Attribute":
                    {
                        var newInstance = new Attribute((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "Tag":
                    {
                        var newInstance = new Tag((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "Effect":
                    {
                        var newInstance = new Effect((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "AttributeEffect":
                    {
                        var newInstance = new AttributeEffect((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "LoopEffect":
                    {
                        var newInstance = new LoopEffect((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "TagEffect":
                    {
                        var newInstance = new TagEffect((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "TryActivateAbilityEffect":
                    {
                        var newInstance = new TryActivateAbilityEffect((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "WaitEffect":
                    {
                        var newInstance = new WaitEffect((Resource)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "AbilitySystemViewer":
                    {
                        var newInstance = new AbilitySystemViewer((VBoxContainer)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "AttributeViewer":
                    {
                        var newInstance = new AttributeViewer((Control)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "AbilityViewer":
                    {
                        var newInstance = new AbilityViewer((Control)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "EventViewer":
                    {
                        var newInstance = new EventViewer((Control)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
                case "TagViewer":
                    {
                        var newInstance = new TagViewer((Control)instance);
                        CachedInstances[id] = newInstance;
                        return newInstance as T;
                    }
            }
            throw new NotImplementedException();
        }

        private static IEnumerable<T> WhereNotNull<T>(this IEnumerable<T?> value) => value.Where(value => value != null).Select<T?, T>(value => value!);
        public static T? TryAs<[MustBeVariant] T>(this Variant variant) where T : class
        {
            try
            {
                return variant.As<T>();
            }
            catch
            {
                return null;
            }
        }

        public static IEnumerable<OutType> Convert<[MustBeVariant] InType, OutType>(this IEnumerable values)
            where InType : GodotObject
            where OutType : class, IInstanceWrapper<InType> => values.Cast<Variant>()
                .Select(TryAs<InType>)
                .WhereNotNull()
                .Select(CreateWrapperFromObject<OutType>)
                .WhereNotNull();

        public static IEnumerable<OutType> Convert<[MustBeVariant] InType, OutType>(this Variant variant)
            where InType : GodotObject
            where OutType : class, IInstanceWrapper<InType> => variant.As<Godot.Collections.Array<InType>>().Convert<InType, OutType>();

        public static IEnumerable<OutType> Convert<InType, [MustBeVariant] OutType>(this IEnumerable<InType> values)
            where InType : class, IInstanceWrapper<OutType>
            where OutType : GodotObject => values.Select(val => val.Instance).WhereNotNull();
    }
}