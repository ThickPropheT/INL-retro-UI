using INL.Retro.Ui.Console.Operation;
using INL.Retro.Ui.Hardware;

namespace INL.Retro.Ui.Console.Snes
{
    public interface ISnesViewModel : IConsoleViewModel
    {

    }

    public class SnesViewModel : ConsoleViewModel, ISnesViewModel
    {
        public override string Name => "SNES / SFC";

        public SnesViewModel(
            IConnectionSettings connectionSettings,
            IDeviceOperationViewModel[] operations,
            IDisconnectedDevice device)
            : base(connectionSettings, operations, device)
        {
        }

        public override void OnSelect()
        {
            Connect();
        }
    }
}
