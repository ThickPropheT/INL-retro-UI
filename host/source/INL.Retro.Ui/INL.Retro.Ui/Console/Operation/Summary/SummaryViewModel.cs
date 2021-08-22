using INL.Retro.Ui.Hardware.Memory;

namespace INL.Retro.Ui.Console.Operation.Summary
{
    public interface ISummaryViewModel : IDeviceOperationViewModel
    {
        IRom Rom { get; }
    }

    public class SummaryViewModel : DeviceOperationViewModel, ISummaryViewModel
    {
        public override string Name => "Summary";
    }
}
