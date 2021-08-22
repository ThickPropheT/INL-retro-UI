using INL.Retro.Ui.Interop;
using System;

namespace INL.Retro.Ui.Hardware
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
