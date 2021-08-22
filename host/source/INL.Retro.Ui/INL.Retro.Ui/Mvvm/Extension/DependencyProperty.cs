using System;
using System.Linq.Expressions;
using System.Reflection;
using System.Windows;

namespace INL.Retro.Ui.Mvvm.Extension
{
    public class DependencyProperty<TOwner>
    {
        public static DependencyProperty Register<TProperty>(
            Expression<Func<TOwner, TProperty>> propertyExpression,
            PropertyChangedCallback onChange = null)
        {
            if (!(propertyExpression.Body is MemberExpression member)
                || !(member.Member is PropertyInfo property))
                throw new ArgumentException("Expression must refer to a property.", nameof(propertyExpression));

            return DependencyProperty.Register(property.Name, typeof(TProperty), typeof(TOwner), new PropertyMetadata(onChange));
        }
    }
}
