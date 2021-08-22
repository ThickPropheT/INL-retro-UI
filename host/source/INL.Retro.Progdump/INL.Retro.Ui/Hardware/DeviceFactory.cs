using INL.Retro.Ui.Hardware.Memory;
using INL.Retro.Ui.Interop;
using System;

namespace INL.Retro.Ui.Hardware
{
    public interface IDeviceFactory
    {
        IDisconnectedDevice NewDisconnectedDevice();
        IConnectedDevice NewConnectedDevice(IntPtr connectionPtr);
    }

    public class DeviceFactory : IDeviceFactory
    {
        private readonly IInteropAdapter _interopAdapter;

        public DeviceFactory(IInteropAdapter interopAdapter)
        {
            _interopAdapter = interopAdapter;
        }

        public IDisconnectedDevice NewDisconnectedDevice()
            => new DisconnectedDevice(_interopAdapter, this);

        public IConnection NewConnection(IntPtr connectionPtr)
            => new Connection(connectionPtr, _interopAdapter);

        private IHeader NewHeader(IConnection connection)
        {
            var header = _interopAdapter.GetRomHeader(connection.Ptr);

            return new Header(
                new Rom(
                    header.rom_name.TrimEnd(),
                    header.map_mode,
                    header.mapping,
                    header.rom_type,
                    header.rom_type_name,
                    header.rom_address_base,
                    header.rom_start_bank,
                    header.rom_bank_size,
                    header.rom_size,
                    header.destination_code,
                    header.destination_name,
                    header.developer_code,
                    header.developer_name,
                    header.version,
                    header.compliment_check,
                    header.checksum,
                    connection,
                    _interopAdapter),
                new SRam(
                    header.sram_address_base,
                    header.sram_start_bank,
                    header.sram_bank_size,
                    header.sram_size,
                    connection,
                    _interopAdapter));
        }

        private IConnectedDevice NewConnectedDevice(IConnection connection)
            => new ConnectedDevice(connection, NewHeader(connection), this);

        public IConnectedDevice NewConnectedDevice(IntPtr connectionPtr)
            => NewConnectedDevice(NewConnection(connectionPtr));
    }
}
