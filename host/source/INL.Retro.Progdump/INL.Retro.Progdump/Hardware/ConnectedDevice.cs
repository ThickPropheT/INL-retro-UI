using INL.Retro.Progdump.Hardware.Memory;
using INL.Retro.Progdump.Interop;
using System;

namespace INL.Retro.Progdump.Hardware
{
    public interface IConnectedDevice : IDisposable
    {
        IConnection Connection { get; }

        IHeader ReadHeader();

        IDisconnectedDevice Disconnect();
    }

    public class ConnectedDevice : IConnectedDevice
    {
        private readonly IHeader _cartridgeHeader;
        private readonly IDeviceFactory _deviceFactory;

        public IConnection Connection { get; }

        public ConnectedDevice(IConnection connection, IHeader cartridgeHeader, IDeviceFactory deviceFactory)
        {
            Connection = connection;
            _cartridgeHeader = cartridgeHeader;
            _deviceFactory = deviceFactory;
        }

        public IHeader ReadHeader()
            => _cartridgeHeader;

        public IDisconnectedDevice Disconnect()
        {
            Connection.Dispose();
            return _deviceFactory.NewDisconnectedDevice();
        }

        public void Dispose()
            => Connection.Dispose();
    }
}
