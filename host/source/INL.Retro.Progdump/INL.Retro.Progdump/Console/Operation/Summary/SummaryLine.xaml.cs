using INL.Retro.Progdump.Mvvm.Extension;
using System.Windows;
using System.Windows.Controls;

namespace INL.Retro.Progdump.Console.Operation.Summary
{
    /// <summary>
    /// Interaction logic for SummaryLine.xaml
    /// </summary>
    public partial class SummaryLine : UserControl
    {
        public static readonly DependencyProperty LabelProperty =
            DependencyProperty<SummaryLine>
                .Register(l => l.Label);

        public static readonly DependencyProperty ValueProperty =
            DependencyProperty<SummaryLine>
                .Register(l => l.Value);

        public string Label
        {
            get => this.GetValue<string>(LabelProperty);
            set => SetValue(LabelProperty, value);
        }

        public string Value
        {
            get => this.GetValue<string>(ValueProperty);
            set => SetValue(ValueProperty, value);
        }

        public SummaryLine()
        {
            InitializeComponent();
        }
    }
}
