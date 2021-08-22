using Microsoft.WindowsAPICodePack.Dialogs;
using System;
using System.Runtime.InteropServices;
using System.Text;

namespace INL.Retro.Ui.Mvvm.Extension
{
    public interface IDirectorySelector
    {
        bool SelectDirectory(string defaultPath, out string directoryPath);
    }

    public class DirectorySelector : IDirectorySelector
    {
        public bool SelectDirectory(string defaultPath, out string directoryPath)
        {
            var dialog = new CommonOpenFileDialog
            {
                IsFolderPicker = true,
                InitialDirectory = defaultPath
            };

            if (dialog.ShowDialog() == CommonFileDialogResult.Ok)
            {
                directoryPath = dialog.FileName;
                return true;
            }

            directoryPath = string.Empty;
            return false;
        }
    }
}
