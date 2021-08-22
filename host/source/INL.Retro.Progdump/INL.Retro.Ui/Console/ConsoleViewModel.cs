using INL.Retro.Ui.Console.Operation;
using INL.Retro.Ui.Hardware;
using INL.Retro.Ui.Hardware.Memory;
using Prism.Mvvm;
using System;
using System.Linq;

namespace INL.Retro.Ui.Console
{
    public interface IConsoleViewModel
    {
        string Name { get; }

        bool CanConnect { get; }
        bool IsConnected { get; }

        IRom Rom { get; }

        IDeviceOperationViewModel[] Operations { get; }
        IDeviceOperationViewModel SelectedOperation { get; set; }

        void OnSelect();

        void Connect();
        void Disconnect();

        void OnDeselect();
    }

    public abstract class ConsoleViewModel : BindableBase, IConsoleViewModel
    {
        private readonly IConnectionSettings _connectionSettings;

        private Action _doConnect;
        private Action _doDisconnect;

        private bool _isConnected;
        private IRom _rom;

        private IDeviceOperationViewModel _selectedOperation;

        public abstract string Name { get; }

        public virtual bool CanConnect => true;

        public bool IsConnected
        {
            get => _isConnected;
            private set => SetProperty(ref _isConnected, value);
        }

        public IRom Rom
        {
            get => _rom;
            protected set => SetProperty(ref _rom, value);
        }

        public IDeviceOperationViewModel[] Operations { get; }

        public IDeviceOperationViewModel SelectedOperation
        {
            get => _selectedOperation;
            set => SetProperty(ref _selectedOperation, value);
        }

        public ConsoleViewModel(
            IConnectionSettings connectionSettings,
            IDeviceOperationViewModel[] operations,
            IDisconnectedDevice device)
        {
            _connectionSettings = connectionSettings;

            Operations = operations;
            SelectedOperation = operations.FirstOrDefault();

            SetDoConnect(device);
        }

        private void SetDoConnect(IDisconnectedDevice device)
            => _doConnect = () => DoConnect(device);

        private void SetDoDisconnect(IConnectedDevice device)
            => _doDisconnect = () => DoDisconnect(device);

        public virtual void OnSelect() { }

        protected virtual void OnConnect(IConnectedDevice device)
        {
            Rom = device.ReadHeader().Rom;

            foreach (var operation in Operations)
            {
                operation.OnConnect(device);
            }
        }

        private void DoConnect(IDisconnectedDevice device)
        {
            if (!device.TryConnect(_connectionSettings, out var connectedDevice))
                return;

            _doConnect = null;

            OnConnect(connectedDevice);
            IsConnected = true;

            SetDoDisconnect(connectedDevice);
        }

        public void Connect()
            => _doConnect?.Invoke();

        protected virtual void OnDisconnect(IDisconnectedDevice device)
        {
            foreach (var operation in Operations)
            {
                operation.OnDisconnect(device);
            }

            Rom = null;
        }

        private void DoDisconnect(IConnectedDevice device)
        {
            var disconnectedDevice = device.Disconnect();

            _doDisconnect = null;

            OnDisconnect(disconnectedDevice);
            IsConnected = false;

            SetDoConnect(disconnectedDevice);
        }

        public void Disconnect()
            => _doDisconnect?.Invoke();

        public void OnDeselect()
            => Disconnect();
    }
}
