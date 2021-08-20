using INL.Retro.Progdump.Hardware;
using INL.Retro.Progdump.Hardware.Memory;
using INL.Retro.Progdump.Interop;
using System;
using System.Reflection;

namespace INL.Retro.ProgdumpConsole
{
    class Program
    {
        static void Main(string[] args)
        {
            const string dumperRoot = @"F:\Games\Utilities\INL-retro-progdump-master";
            var hostDirectory = $@"{dumperRoot}\host";

            var interopAdapter = new InteropAdapter(new System.IO.DirectoryInfo(hostDirectory));
            var deviceFactory = new DeviceFactory(interopAdapter);

            var device = new DisconnectedDevice(interopAdapter, deviceFactory);

            var monitor = new ProgressMonitor();

            IConnectedDevice connectedDevice;
            if (device.TryConnect(
                new Progdump.Hardware.ConnectionSettings("./scripts/snes/v2proto_hirom.lua"),
                out connectedDevice))
            {
                var header = connectedDevice.ReadHeader();
                var rom = header.Rom;
                var sram = header.SRam;

                foreach (var property in rom.GetType().GetProperties(BindingFlags.Instance | BindingFlags.Public))
                {
                    Console.WriteLine($"{property.Name}: {property.GetValue(rom)}");
                }

                const string dumpPath = @"F:\Games\SNES\ROM Dumps\TEST";

                monitor.StartAndMonitor((complete, total) => Console.WriteLine($"{(complete / (double)total) * 100}% Complete"),
                    new DumpOperation(rom, $@"{dumpPath}\test.rom"),
                    new DumpOperation(sram, $@"{dumpPath}\test.sram"));

                connectedDevice.Disconnect();
            }

            //var manager = new SessionManager();
            //if (manager.TryOpen(new SnesSessionSettings()))
            //{
            //    Console.Write(manager.CurrentSession.RomTitle);
            //}

            //manager.Close();

            Console.ReadLine();
        }
    }

    public class Operation : IReportProgress
    {
        public int Total { get; set; }

        public void Start(Action<int> progressCallback)
        {
            for (var i = 0; i < Total; i++)
            {
                // do something ...

                progressCallback(i + 1);
            }
        }
    }
}
