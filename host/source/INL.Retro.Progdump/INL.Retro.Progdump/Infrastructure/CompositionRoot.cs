using INL.Retro.Progdump.Console;
using INL.Retro.Progdump.Console.Operation;
using INL.Retro.Progdump.Console.Operation.Dump;
using INL.Retro.Progdump.Console.Operation.Summary;
using INL.Retro.Progdump.Console.Snes;
using INL.Retro.Progdump.Hardware;
using INL.Retro.Progdump.Interop;
using INL.Retro.Progdump.Mvvm;
using INL.Retro.Progdump.Mvvm.Extension;
using System.IO;

namespace INL.Retro.Progdump.Infrastructure
{
    public static class CompositionRoot
    {
        public static MainView Compose()
        {
            // TODO find a better way to get/structure this path
            const string dumperRoot = @"F:\Games\Utilities\INL-retro-progdump-master";
            var hostDirectory = $@"{dumperRoot}\host";

            // TODO convert to log4net
            //var fileStream = new FileStream("TIMMEH.log", FileMode.Create);
            //var streamWriter = new StreamWriter(fileStream);
            //streamWriter.AutoFlush = true;

            //System.Console.SetOut(streamWriter);

            //NativeMethods.SetStdHandle((int)STD_HANDLE.STD_OUTPUT_HANDLE, fileStream.SafeFileHandle.DangerousGetHandle());

            var interopAdapter = new InteropAdapter(interopRoot: new DirectoryInfo(hostDirectory));
            var deviceFactory = new DeviceFactory(interopAdapter);

            var device = deviceFactory.NewDisconnectedDevice();

            var directorySelector = new DirectorySelector();
            var progressMonitor = new ProgressMonitor();

            var operations = new IDeviceOperationViewModel[]
            {
                new SummaryViewModel(),
                new DumpViewModel(directorySelector, progressMonitor)
            };

            var consoles = new IConsoleViewModel[]
            {
                //new NesViewModel(),
                new SnesViewModel(new SnesConnectionSettings(), operations, device)
            };

            IMainViewModel viewModel = new MainViewModel(consoles);

            var view = new MainView
            {
                DataContext = viewModel
            };

            return view;
        }
    }
}
