using System;
using System.Globalization;
using System.Windows.Data;

namespace INL.Retro.Progdump.Mvvm.Extension
{
    [ValueConversion(typeof(bool), typeof(string))]
    public class BooleanToTextConverter : IValueConverter
    {
        public string TrueText { get; set; }

        public string FalseText { get; set; }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (!(value is bool))
                return null;

            return ((bool)value)
                ? TrueText
                : FalseText;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
            => null;
    }
}
