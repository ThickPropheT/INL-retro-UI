using INL.Retro.Progdump.Interop;

namespace INL.Retro.Progdump.Hardware.Memory
{
    public interface ISRam : IDumpableMemory
    {

    }

    public class SRam : DumpableMemory, ISRam
    {
        private readonly IConnection _connection;
        private readonly IInteropAdapter _interopAdapter;

        public SRam(
            int addressBase,
            int startBank,
            int bankSize,
            int size,
            IConnection connection,
            IInteropAdapter interopAdapter)
            : base(addressBase, startBank, bankSize, size)
        {
            _connection = connection;
            _interopAdapter = interopAdapter;
        }

        public override void Dump(string path, ProgressCallback progressReport)
            => _interopAdapter.device_dumpSRam(_connection, path, progressReport);
    }
}
