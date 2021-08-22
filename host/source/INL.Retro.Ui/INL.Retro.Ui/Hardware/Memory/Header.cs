namespace INL.Retro.Ui.Hardware.Memory
{
    public interface IHeader
    {
        IRom Rom { get; }
        ISRam SRam { get; }
    }

    public class Header : IHeader
    {
        public IRom Rom { get; }
        public ISRam SRam { get; }

        public Header(IRom rom, ISRam sram)
        {
            Rom = rom;
            SRam = sram;
        }
    }
}
