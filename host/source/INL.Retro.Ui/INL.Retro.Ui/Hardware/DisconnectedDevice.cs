using INL.Retro.Ui.Interop;
using System;

namespace INL.Retro.Ui.Hardware
{
    public interface IDisconnectedDevice
    {
        bool TryConnect(IConnectionSettings settings, out IConnectedDevice connectedDevice);
    }

    public class DisconnectedDevice : IDisconnectedDevice
    {
        private readonly IInteropAdapter _interopAdapter;
        private readonly IDeviceFactory _deviceFactory;

        public DisconnectedDevice(IInteropAdapter interopAdapter, IDeviceFactory deviceFactory)
        {
            _interopAdapter = interopAdapter;
            _deviceFactory = deviceFactory;
        }

        public bool TryConnect(IConnectionSettings settings, out IConnectedDevice connectedDevice)
        {
            IntPtr connectionPtr;
            if (_interopAdapter.device_tryConnect(
                    new Interop.ConnectionSettings
                    {
                        libusb_log = 0,
                        scripts_path = settings.ScriptFile.FullName
                    },
                    out connectionPtr))
            {
                connectedDevice = _deviceFactory.NewConnectedDevice(connectionPtr);
                return true;
            }

            connectedDevice = null;
            return false;
        }
    }
}
