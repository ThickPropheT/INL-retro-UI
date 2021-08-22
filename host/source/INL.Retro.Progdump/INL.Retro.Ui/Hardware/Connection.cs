using INL.Retro.Progdump.Interop;
using System;

namespace INL.Retro.Progdump.Hardware
{
    public interface IConnection : IDisposable
    {
        IntPtr Ptr { get; }
    }

    internal class Connection : IConnection
    {
        private readonly IInteropAdapter _interopAdapter;

        public IntPtr Ptr { get; }

        public Connection(IntPtr connectionPtr, IInteropAdapter interopAdapter)
        {
            Ptr = connectionPtr;
            _interopAdapter = interopAdapter;
        }

        public void Dispose()
            => _interopAdapter.device_disconnect(this);
    }
}
