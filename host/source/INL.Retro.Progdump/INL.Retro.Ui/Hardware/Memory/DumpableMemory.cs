using INL.Retro.Progdump.Interop;
using System;

namespace INL.Retro.Progdump.Hardware.Memory
{
    public interface IDumpableMemory
    {
        int AddressBase { get; }
        int StartBank { get; }
        int BankSize { get; }
        int Size { get; }

        int TotalBanks { get; }

        void Dump(string path, ProgressCallback progressReport);
    }

    public abstract class DumpableMemory : IDumpableMemory
    {
        public int AddressBase { get; }
        public int StartBank { get; }
        public int BankSize { get; }
        public int Size { get; }

        int IDumpableMemory.TotalBanks => Math.Max(Size / BankSize, 1);

        public DumpableMemory(
            int addressBase,
            int startBank,
            int bankSize,
            int size)
        {
            AddressBase = addressBase;
            StartBank = StartBank;
            BankSize = bankSize;
            Size = size;
        }

        public abstract void Dump(string path, ProgressCallback progressReport);
    }
}
