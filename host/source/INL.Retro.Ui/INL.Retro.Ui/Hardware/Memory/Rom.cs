using INL.Retro.Ui.Interop;
using System.ComponentModel;

namespace INL.Retro.Ui.Hardware.Memory
{
    public interface IRom : IDumpableMemory
    {
        string Name { get; }

        int MapMode { get; }
        string Mapping { get; }

        int Type { get; }
        string TypeName { get; }

        int RegionCode { get; }
        string RegionName { get; }

        int DeveloperCode { get; }
        string DeveloperName { get; }

        int Version { get; }
        int ComplimentCheck { get; }
        int Checksum { get; }
    }

    public class Rom : DumpableMemory, IRom
    {
        private readonly IConnection _connection;
        private readonly IInteropAdapter _interopAdapter;

        public string Name { get; }

        public int MapMode { get; }
        public string Mapping { get; }

        public int Type { get; }
        public string TypeName { get; }

        public int RegionCode { get; }
        public string RegionName { get; }

        public int DeveloperCode { get; }
        public string DeveloperName { get; }

        public int Version { get; }
        public int ComplimentCheck { get; }
        public int Checksum { get; }

        public Rom(
            string name,
            int mapMode,
            string mapping,
            int type,
            string typeName,
            int addressBase,
            int startBank,
            int bankSize,
            int size,
            int regionCode,
            string regionName,
            int developerCode,
            string developerName,
            int version,
            int complimentCheck,
            int checksum,
            IConnection connection,
            IInteropAdapter interopAdapter)
            : base(addressBase, startBank, bankSize, size)
        {
            Name = name;
            MapMode = mapMode;
            Mapping = mapping;
            Type = type;
            TypeName = typeName;
            RegionCode = regionCode;
            RegionName = regionName;
            DeveloperCode = developerCode;
            DeveloperName = developerName;
            Version = version;
            ComplimentCheck = complimentCheck;
            Checksum = checksum;

            _connection = connection;
            _interopAdapter = interopAdapter;
        }

        public override void Dump(string path, ProgressCallback progressReport)
            => _interopAdapter.device_dumpRom(_connection, path, progressReport);
    }
}
