using INL.Retro.Ui.Hardware;
using INL.Retro.Ui.Hardware.Memory;
using Prism.Mvvm;
using System.ComponentModel;

namespace INL.Retro.Ui.Console.Operation
{
    public interface IDeviceOperationViewModel : INotifyPropertyChanged
    {
        string Name { get; }

        void OnConnect(IConnectedDevice device);
        void OnDisconnect(IDisconnectedDevice device);
    }

    public abstract class DeviceOperationViewModel : BindableBase, IDeviceOperationViewModel
    {
        private IRom _rom;
        private ISRam _sRam;

        public abstract string Name { get; }

        public IRom Rom
        {
            get => _rom;
            private set => SetProperty(ref _rom, value);
        }

        public ISRam SRam
        {
            get => _sRam;
            private set => SetProperty(ref _sRam, value);
        }

        protected virtual void OnConnectImpl(IConnectedDevice device) { }

        public void OnConnect(IConnectedDevice device)
        {
            var header = device.ReadHeader();

            Rom = header.Rom;
            SRam = header.SRam;

            OnConnectImpl(device);
        }

        protected virtual void OnDisconnectImpl(IDisconnectedDevice device) { }

        public void OnDisconnect(IDisconnectedDevice device)
        {
            OnDisconnectImpl(device);

            Rom = null;
            SRam = null;
        }
    }
}
