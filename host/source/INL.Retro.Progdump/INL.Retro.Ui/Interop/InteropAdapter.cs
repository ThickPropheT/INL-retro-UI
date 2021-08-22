using INL.Retro.Ui.Hardware;
using System;
using System.Diagnostics;
using System.IO;

namespace INL.Retro.Ui.Interop
{
#pragma warning disable IDE1006 // Naming Styles

    public interface IInteropAdapter
    {
        bool device_tryConnect(ConnectionSettings settings, out IntPtr connectionPtr);

        void device_dumpRom(IConnection connection, string path, ProgressCallback callback);
        void device_dumpSRam(IConnection connection, string path, ProgressCallback callback);

        void device_disconnect(IConnection connection);

        RomHeader GetRomHeader(IntPtr connectionPtr);
    }

#pragma warning restore IDE1006 // Naming Styles

    public class InteropAdapter : IInteropAdapter
    {
        public InteropAdapter(DirectoryInfo interopRoot)
        {
            if (!interopRoot.Exists)
                throw new ArgumentException($"Argument must be a path to a valid directory.[{interopRoot.FullName}]", nameof(interopRoot));

            Directory.SetCurrentDirectory(interopRoot.FullName);
        }

        public bool device_tryConnect(ConnectionSettings settings, out IntPtr connection)
            => NativeMethods.Device_tryConnect(settings, out connection);

        public void device_dumpRom(IConnection connection, string path, ProgressCallback callback)
            => NativeMethods.device_dumpRom(connection.Ptr, path, callback);

        public void device_dumpSRam(IConnection connection, string path, ProgressCallback callback)
            => NativeMethods.device_dumpSRam(connection.Ptr, path, callback);

        public void device_disconnect(IConnection connection)
            => NativeMethods.device_disconnect(connection.Ptr);

        public RomHeader GetRomHeader(IntPtr connectionPtr)
            => connectionPtr.ToStruct<ConnectionToken>()
                .rom_header.ToStruct<RomHeader>();
    }
}
