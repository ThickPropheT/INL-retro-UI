Usage: inlretro [options]

Options/Flags:
  --console=console, -c console                 Console port, {GBA,GENESIS,N64,NES}
  --dump_filename=filename, -d filename         If provided, dump cartridge ROMs to this filename
  --dump_ram_filename=filename, -a filename     If provided write ram to this filename
  --help, -h                                    Displays this message.
  --lua_filename=filename, -s filename          If provided, use this script for main application logic
  --mapper=mapper, -m mapper                    NES, SNES, GB consoles only, mapper ASIC on cartridge
                                                NES:    {action53,bnrom,cdream,cninja,cnrom,dualport,easynsf,fme7,
                                                         mapper30,mmc1,mmc3,mmc4,mmc5,nrom,unrom}
                                                GB:     {mbc1,romonly}
                                                SNES:   {lorom,hirom}
  --nes_prg_rom_size_kbyte=size, -x size_kbytes NES-only, size of PRG-ROM in kilobytes
  --nes_chr_rom_size_kbyte=size, -y size_kbytes NES-only, size of CHR-ROM in kilobytes
  --rom_size_kbyte=size, -k size_kbytes         Size of ROM in kilobytes, non-NES systems.
  --rom_size_mbit=size, -z size_mbits           Size of ROM in megabits, non-NES systems.
  --verify_filename=filename, -v filename       If provided, writeback written rom to this filename
  --wram_size_kbyte=size, -w size_kbytes        NES-only, size of WRAM in kilobytes
  --write_filename=filename, -p filename        If provided, write this data to cartridge
  --write_ram_filename=filename, -b filename    If provided write this file's contents to ram