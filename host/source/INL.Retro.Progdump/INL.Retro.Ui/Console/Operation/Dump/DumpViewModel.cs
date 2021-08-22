using INL.Retro.Ui.Hardware;
using INL.Retro.Ui.Hardware.Memory;
using INL.Retro.Ui.Mvvm.Extension;
using Prism.Commands;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Input;

namespace INL.Retro.Ui.Console.Operation.Dump
{
    public interface IDumpViewModel : IDeviceOperationViewModel
    {
        string OutputName { get; }

        string OutputLocation { get; }
        ICommand SelectOutputLocationCommand { get; }

        bool DumpRom { get; set; }
        bool DumpSRam { get; set; }

        ICommand DumpCommand { get; }

        bool IsDumping { get; }

        int CurrentDumpProgress { get; }
        int TotalDumpProgress { get; }
    }

    public class DumpViewModel : DeviceOperationViewModel, IDumpViewModel
    {
        private readonly IDirectorySelector _directorySelector;
        private readonly IProgressMonitor _progressMonitor;

        private readonly DelegateCommand _dumpCommand;

        private string _outputName;
        private string _outputLocation;

        private bool _dumpRom;
        private bool _dumpSRam;

        private bool _isDumping;

        private int _dumpProgress;
        private int _dumpTotal = 1;

        public override string Name => "Dump";

        public string OutputName
        {
            get => _outputName;
            set
            {
                var romName = Rom.Name;

                // TODO improve the behavior of the default value handling here
                // TODO could the text box be automatically selected when
                // TODO you click inside?
                if (string.IsNullOrEmpty(value))
                    value = romName;

                else if (Equals(_outputName, romName))
                    value = value.Replace(romName, "");

                SetProperty(ref _outputName, value, DumpOptionsChanged);
            }
        }

        // TODO add path validation here
        public string OutputLocation
        {
            get => _outputLocation;
            set => SetProperty(ref _outputLocation, value, DumpOptionsChanged);
        }

        public ICommand SelectOutputLocationCommand { get; }

        public bool DumpRom
        {
            get => _dumpRom;
            set => SetProperty(ref _dumpRom, value, DumpOptionsChanged);
        }
        public bool DumpSRam
        {
            get => _dumpSRam;
            set => SetProperty(ref _dumpSRam, value, DumpOptionsChanged);
        }

        public ICommand DumpCommand => _dumpCommand;

        public bool IsDumping
        {
            get => _isDumping;
            private set => SetProperty(ref _isDumping, value);
        }

        public int CurrentDumpProgress
        {
            get => _dumpProgress;
            private set => SetProperty(ref _dumpProgress, value);
        }

        public int TotalDumpProgress
        {
            get => _dumpTotal;
            private set => SetProperty(ref _dumpTotal, value);
        }

        public DumpViewModel(
            IDirectorySelector directorySelector,
            IProgressMonitor progressMonitor)
        {
            _directorySelector = directorySelector;
            _progressMonitor = progressMonitor;

            SelectOutputLocationCommand = new DelegateCommand(SelectOutputDirectory);
            _dumpCommand = new DelegateCommand(StartDump, CanDump);

            // TODO abstract this away somewhere
            OutputLocation = $@"{Environment.CurrentDirectory}\Dumps";

            if (Directory.Exists(OutputLocation))
                return;

            Directory.CreateDirectory(OutputLocation);
        }

        protected override void OnConnectImpl(IConnectedDevice device)
        {
            // TODO load output location from configuration

            // TODO this incorrectly clears the text
            OutputName = Rom.Name;
        }

        private void SelectOutputDirectory()
        {
            if (!_directorySelector.SelectDirectory(OutputLocation, out var directoryPath))
                return;

            OutputLocation = directoryPath;
        }

        private void ResetProgress()
        {
            CurrentDumpProgress = 0;
            TotalDumpProgress = 1;
        }

        private void CanDumpChanged()
            => _dumpCommand.RaiseCanExecuteChanged();

        private void DumpOptionsChanged()
        {
            ResetProgress();
            CanDumpChanged();
        }

        private bool CanDump()
            => !string.IsNullOrEmpty(OutputName)
            && !string.IsNullOrEmpty(OutputLocation)
            && (DumpRom || DumpSRam);

        // TODO add customizable extensions here
        // TODO add manually included extension safety here
        private string BuildRomPath()
            => $"{Path.Combine(OutputLocation, OutputName)}.sfc";

        private string BUildSRamPath()
            => $"{Path.Combine(OutputLocation, OutputName)}.sram";

        private void StartDump()
        {
            IsDumping = true;

            ResetProgress();

            var operations = new List<DumpOperation>();

            if (DumpSRam)
                operations.Add(new DumpOperation(SRam, BUildSRamPath()));

            if (DumpRom)
                operations.Add(new DumpOperation(Rom, BuildRomPath()));

            // TODO add error handling here
            Task.Run(() =>
                _progressMonitor.StartAndMonitor(
                    (complete, total) =>
                    {
                        TotalDumpProgress = total;
                        CurrentDumpProgress = complete;

                        Debug.WriteLine($"{complete} / {total}");
                    },
                    operations.ToArray()))
                .ContinueWith(t => IsDumping = false);
        }

        protected override void OnDisconnectImpl(IDisconnectedDevice device)
        {
            // TODO this doesn't correctly clear the text
            OutputName = null;

            // TODO save output location to configuration
        }
    }
}
