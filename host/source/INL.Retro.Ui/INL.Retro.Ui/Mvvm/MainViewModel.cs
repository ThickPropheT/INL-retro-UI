using INL.Retro.Ui.Console;
using INL.Retro.Ui.Mvvm.Extension;
using Prism.Commands;
using Prism.Mvvm;
using System.ComponentModel;
using System.Linq;
using System.Windows.Input;

namespace INL.Retro.Ui.Mvvm
{
    public interface IMainViewModel : ILifecycleViewModel, INotifyPropertyChanged
    {
        string Description { get; }

        IConsoleViewModel[] Consoles { get; }
        IConsoleViewModel SelectedConsole { get; set; }

        ICommand ToggleConnectionCommand { get; }
    }

    public class MainViewModel : BindableBase, IMainViewModel
    {
        private IConsoleViewModel _selectedConsole;

        public string Description { get; } = "INLretro";

        public IConsoleViewModel[] Consoles { get; }

        public IConsoleViewModel SelectedConsole
        {
            get => _selectedConsole;
            set
            {
                var old = _selectedConsole;
                SetProperty(ref _selectedConsole, value,
                    () =>
                    {
                        old?.OnDeselect();
                        value?.OnSelect();
                    });
            }
        }

        public ICommand ToggleConnectionCommand { get; }

        public MainViewModel(IConsoleViewModel[] consoles)
        {
            Consoles = consoles;
            SelectedConsole = consoles.FirstOrDefault();

            ToggleConnectionCommand = new DelegateCommand(ToggleConnection);
        }

        private void ToggleConnection()
        {
            var selectedConsole = SelectedConsole;

            if (!selectedConsole.IsConnected) selectedConsole.Connect();
            else selectedConsole.Disconnect();
        }

        public bool OnClosing()
        {
            SelectedConsole.Disconnect();

            return true;
        }
    }
}
