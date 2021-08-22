using INL.Retro.Ui.Mvvm.Extension;
using System.Windows;

namespace INL.Retro.Ui.Mvvm
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainView : Window
    {
        public MainView()
        {
            InitializeComponent();

            Closing += MainView_Closing;
        }

        private void MainView_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            var vm = DataContext as ILifecycleViewModel;

            if (vm == null)
                return;

            e.Cancel = !vm.OnClosing();
        }
    }
}
