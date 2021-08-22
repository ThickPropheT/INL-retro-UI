using INL.Retro.Progdump.Hardware.Memory;

namespace INL.Retro.Progdump.Console.Operation.Summary
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
