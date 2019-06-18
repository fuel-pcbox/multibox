#include "common.h"
#include "cpu.h"
#include "info.h"

u8 low_ram[0xa0000]; // 640KB
u8 bios[0x20000];    // 128KB

u8 pc_rb(void *dev, addr_t addr)
{
    if (addr < 0xa0000)
        return low_ram[addr];
    else if (addr >= 0xe0000 && addr < 0x100000)
        return bios[addr & 0x1ffff];
    else if (addr >= 0xfffe0000)
        return bios[addr & 0x1ffff];
    else
        return 0xff;
}

u16 pc_rw(void *dev, addr_t addr)
{
    if (addr < 0xa0000)
        return *(u16 *)&low_ram[addr];
    else if (addr >= 0xe0000 && addr < 0x100000)
        return *(u16 *)&bios[addr & 0x1ffff];
    else if (addr >= 0xfffe0000)
        return *(u16 *)&bios[addr & 0x1ffff];
    else
        return 0xffff;
}

u32 pc_rl(void *dev, addr_t addr)
{
    if (addr < 0xa0000)
        return *(u32 *)&low_ram[addr];
    else if (addr >= 0xe0000 && addr < 0x100000)
        return *(u32 *)&bios[addr & 0x1ffff];
    else if (addr >= 0xfffe0000)
        return *(u32 *)&bios[addr & 0x1ffff];
    else
        return 0xffffffff;
}

void pc_wb(void *dev, addr_t addr, u8 data)
{
    if (addr < 0xa0000)
        low_ram[addr] = data;
}

void pc_ww(void *dev, addr_t addr, u16 data)
{
    if (addr < 0xa0000)
        *(u16 *)&low_ram[addr] = data;
}

void pc_wl(void *dev, addr_t addr, u32 data)
{
    if (addr < 0xa0000)
        *(u32 *)&low_ram[addr] = data;
}

u8 pc_iorb(void* dev, u16 addr)
{
    switch(addr)
    {
        case 0x64:
        {
            return 0;
        }
        default:
        {
            return 0xff;
        }
    }
}

void pc_iowb(void* dev, u16 addr, u8 data)
{
}

int main(int ac, char **av)
{
    printf("Welcome to multibox\n");
    printf("Version %s\nBuilt on %s at %s\n", version, __DATE__, __TIME__);

    if (ac < 2)
        return 1;

    cpu_t cpu;

    cpu.init(cpu_type::intel386);

    cpu.device = nullptr;

    cpu.rb_real = pc_rb;
    cpu.rw_real = pc_rw;
    cpu.rl_real = pc_rl;
    cpu.wb_real = pc_wb;
    cpu.ww_real = pc_ww;
    cpu.wl_real = pc_wl;

    cpu.iorb_real = pc_iorb;
    cpu.iowb_real = pc_iowb;

    FILE *fp = fopen(av[1], "rb");
    fread(bios + 0x10000, 1, 0x10000, fp);
    fclose(fp);

    cpu.run(50);

    return 0;
}