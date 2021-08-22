using System;
using System.Runtime.InteropServices;

namespace INL.Retro.Ui.Interop
{
    internal enum STD_HANDLE
    {
        STD_INPUT_HANDLE = -10,
        STD_OUTPUT_HANDLE = -11,
        STD_ERROR_HANDLE = -12
    }

    internal enum libusb_log_level
    {
        LIBUSB_LOG_LEVEL_NONE = 0,
        LIBUSB_LOG_LEVEL_ERROR,
        LIBUSB_LOG_LEVEL_WARNING,
        LIBUSB_LOG_LEVEL_INFO,
        LIBUSB_LOG_LEVEL_DEBUG,
    };

    [StructLayout(LayoutKind.Sequential)]
    internal struct libusb_context { }

    [StructLayout(LayoutKind.Sequential)]
    internal struct libusb_device_handle { }

    [StructLayout(LayoutKind.Sequential)]
    internal struct USBtransfer
    {
        /// <summary>
        /// <see cref="libusb_device_handle"/>*
        /// </summary>
        public IntPtr handle;

        /// <summary>
        /// uint8_t
        /// </summary>
        public byte endpoint;

        /// <summary>
        /// uint8_t
        /// </summary>
        public byte request;

        /// <summary>
        /// uint16_t
        /// </summary>
        public ushort wValue;

        /// <summary>
        /// uint16_t
        /// </summary>
        public ushort wIndex;

        /// <summary>
        /// uint16_t
        /// </summary>
        public ushort wLength;

        /// <summary>
        /// unsigned char*
        /// </summary>
        [MarshalAs(UnmanagedType.LPStr)]
        public string data;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ConnectionSettings
    {
        public int libusb_log;
        public string scripts_path;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct RomHeader
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string rom_name;

        public int map_mode;

        [MarshalAs(UnmanagedType.LPStr)]
        public string mapping;

        public int rom_type;

        [MarshalAs(UnmanagedType.LPStr)]
        public string rom_type_name;

        public int rom_address_base;
        public int rom_start_bank;
        public int rom_bank_size;
        public int rom_size;

        public int sram_address_base;
        public int sram_start_bank;
        public int sram_bank_size;
        public int sram_size;

        public int destination_code;

        [MarshalAs(UnmanagedType.LPStr)]
        public string destination_name;

        public int developer_code;

        [MarshalAs(UnmanagedType.LPStr)]
        public string developer_name;

        public int version;
        public int compliment_check;
        public int checksum;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct ConnectionToken
    {
        /// <summary>
        /// <see cref="libusb_context"/>*
        /// </summary>
        public IntPtr usb_context;

        /// <summary>
        /// <see cref="USBtransfer"/>*
        /// </summary>
        public IntPtr usb_transfer;

        /// <summary>
        /// lua_State*
        /// </summary>
        public IntPtr L;

        /// <summary>
        /// <see cref="RomHeader"/>*
        /// </summary>
        public IntPtr rom_header;
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void ProgressCallback(int complete);

    internal sealed class NativeMethods
    {
        internal const string DllPath = "inlprog_device.dll";

        private NativeMethods() { }

        [DllImport("Kernel32.dll", SetLastError = true)]
        internal static extern int SetStdHandle(int device, IntPtr handle);

        /// <summary>
        /// </summary>
        /// <param name="settings"><see cref="ConnectionSettings"/></param>
        /// <param><see cref="ConnectionToken"/>*</param>
        /// <returns><langword>true</langword>if a connection was established; <langword>false</langword>otherwise.</returns>
        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern bool Device_tryConnect(ConnectionSettings settings, out IntPtr connection);

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void device_dumpRom(IntPtr connection, string path, ProgressCallback callback);

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void device_dumpSRam(IntPtr connection, string path, ProgressCallback callback);

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void doCallback(IntPtr connection, ProgressCallback callback);

        /// <summary>
        /// </summary>
        /// <param name="context"><see cref="ConnectionToken"/>*</param>
        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void device_disconnect(IntPtr connection);
    }
}
