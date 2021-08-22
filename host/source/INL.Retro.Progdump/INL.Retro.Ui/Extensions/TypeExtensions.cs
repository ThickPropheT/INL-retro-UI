using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace INL.Retro.Progdump.Extensions
{
    public static class TypeExtensions
    {
        public static IEnumerable<PropertyInfo> GetInterfaceProperties(this Type t)
            => t.GetInterfaces()
            .Reverse()
            .SelectMany(i =>
                i.GetProperties(
                    BindingFlags.Public
                    | BindingFlags.Instance));
    }
}
