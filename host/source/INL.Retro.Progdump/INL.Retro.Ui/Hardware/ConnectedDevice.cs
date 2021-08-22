using INL.Retro.Ui.Hardware.Memory;
using INL.Retro.Ui.Interop;
using System;

namespace INL.Retro.Ui.Hardware
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
