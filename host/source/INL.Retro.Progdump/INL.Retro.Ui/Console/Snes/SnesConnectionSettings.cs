using INL.Retro.Ui.Hardware;

namespace INL.Retro.Ui.Console.Snes
{
    public class SnesConnectionSettings : ConnectionSettings
    {
        public SnesConnectionSettings()
            : base("./scripts/snes/v2proto_hirom.lua")
        {
        }
    }
}
