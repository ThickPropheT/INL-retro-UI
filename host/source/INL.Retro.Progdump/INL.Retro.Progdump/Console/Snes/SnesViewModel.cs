using INL.Retro.Progdump.Console.Operation;
using INL.Retro.Progdump.Hardware;

namespace INL.Retro.Progdump.Console.Snes
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
