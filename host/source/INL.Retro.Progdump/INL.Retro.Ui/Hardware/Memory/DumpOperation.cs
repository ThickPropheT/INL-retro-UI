using System;

namespace INL.Retro.Progdump.Hardware.Memory
{
    public class DumpOperation : IReportProgress
    {
        private readonly IDumpableMemory _dumpable;

        private readonly string _dumpPath;

        public int Total => _dumpable.TotalBanks;

        public DumpOperation(IDumpableMemory dumpable, string dumpPath)
        {
            _dumpable = dumpable;
            _dumpPath = dumpPath;
        }

        public void Start(Action<int> progressReport)
            => _dumpable.Dump(_dumpPath, current => progressReport(current));
    }
}
