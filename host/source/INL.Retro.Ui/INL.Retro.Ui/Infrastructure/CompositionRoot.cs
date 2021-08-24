﻿using INL.Retro.Ui.Console;
using INL.Retro.Ui.Console.Operation;
using INL.Retro.Ui.Console.Operation.Dump;
using INL.Retro.Ui.Console.Operation.Summary;
using INL.Retro.Ui.Console.Snes;
using INL.Retro.Ui.Hardware;
using INL.Retro.Ui.Interop;
using INL.Retro.Ui.Mvvm;
using INL.Retro.Ui.Mvvm.Extension;
using System.IO;

namespace INL.Retro.Ui.Infrastructure
{
    public static class CompositionRoot
    {
        public static MainView Compose()
        {
            const string hostDirectory = @"..\";

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