using System.ComponentModel;
using System.Reflection;

namespace INL.Retro.Progdump.Extensions
{
    public static class PropertyInfoExtensions
    {
        public static string GetDescriptionOrName(this PropertyInfo i)
            => i.GetCustomAttribute<DescriptionAttribute>().Description
                ?? i.Name;
    }
}
