using System;
using System.IO;

namespace INL.Retro.Ui.Hardware
{
    public interface IConnectionSettings
    {
        FileInfo ScriptFile { get; }
    }

    public class ConnectionSettings : IConnectionSettings
    {
        public FileInfo ScriptFile { get; }

        public ConnectionSettings(string scriptPath)
        {
            ScriptFile = new FileInfo(scriptPath);

            if (!ScriptFile.Exists)
                throw new ArgumentException($"Argument must be a path to a valid file. [{ScriptFile.FullName}]", nameof(scriptPath));
        }
    }
}
