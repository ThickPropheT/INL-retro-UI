﻿using System.Windows;

namespace INL.Retro.Progdump.Mvvm.Extension
{
    public static class DependencyObjectExtensions
    {
        public static T GetValue<T>(this DependencyObject o, DependencyProperty property)
            => (T)o.GetValue(property);
    }
}