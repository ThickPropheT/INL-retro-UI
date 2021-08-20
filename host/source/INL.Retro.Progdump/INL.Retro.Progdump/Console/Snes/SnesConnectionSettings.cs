using INL.Retro.Progdump.Hardware;

namespace INL.Retro.Progdump.Console.Snes
{
    public class SnesConnectionSettings : ConnectionSettings
    {
        public SnesConnectionSettings()
            : base("./scripts/snes/v2proto_hirom.lua")
        {
        }
    }
}
