#include "cpu.h"
#include "common.h"

#include "cpu_ops_table.h"

#include "cpu_ops.h"

void cpu_t::unhandled_opcode() { printf("oh man oh geez\n"); }

void cpu_t::init(cpu_type _type)
{
    type = _type;
    delayed_interrupt_enable = false;
    eflags.whole = 2;

    for (int i = 0; i < 6; i++)
    {
        segs[i].selector = 0;
        segs[i].base = 0;
        segs[i].limit = 0xffff;
        segs[i].flags = 0x0093;
    }

    segs[cs].selector = 0xf000;
    segs[cs].base = 0xffff0000;
    segs[cs].flags = 0x009b;

    ip = 0x0000fff0;

    EAX = 0;      // We passed the self-test.
    EDX = 0x0308; // 386DX stepping D1.

    cr[0] = 0x7fffffe0;

    for (int i = 0; i < 256; i++)
    {
        opcode_table_1byte_16[i] = &cpu_t::unhandled_opcode;
        opcode_table_1byte_32[i] = &cpu_t::unhandled_opcode;
    }

    for (auto op_info : cpu_opcode_table)
    {
        //TODO
        if (op_info.flags & OP_2BYTE)
        {
        }
        else
        {
            opcode_table_1byte_16[op_info.opcode] = op_info.handler16;
            opcode_table_1byte_32[op_info.opcode] = op_info.handler32;
        }
    }
}

u8 cpu_t::rb_phys(addr_t addr) { return rb_real(device, addr); }

u16 cpu_t::rw_phys(addr_t addr) { return rw_real(device, addr); }

u32 cpu_t::rl_phys(addr_t addr) { return rl_real(device, addr); }

void cpu_t::wb_phys(addr_t addr, u8 data) { wb_real(device, addr, data); }

void cpu_t::ww_phys(addr_t addr, u16 data) { ww_real(device, addr, data); }

void cpu_t::wl_phys(addr_t addr, u32 data) { wl_real(device, addr, data); }

u8 cpu_t::iorb(u16 addr) { return iorb_real(device, addr); }

u16 cpu_t::iorw(u16 addr) { return iorw_real(device, addr); }

u32 cpu_t::iorl(u16 addr) { return iorl_real(device, addr); }

void cpu_t::iowb(u16 addr, u8 data) { iowb_real(device, addr, data); }

void cpu_t::ioww(u16 addr, u16 data) { ioww_real(device, addr, data); }

void cpu_t::iowl(u16 addr, u32 data) { iowl_real(device, addr, data); }

void cpu_t::type_check(x86seg *segment, u32 offset, translate_kind kind)
{
    bool not_system_seg = (segment->flags >> 4) & 1;
    bool executable = (segment->flags >> 3) & 1;

    if (not_system_seg)
    {
        if (executable)
        {
            bool readable = (segment->flags >> 1) & 1;
            switch (kind)
            {
            case translate_kind::TRANSLATE_READ:
                if (!readable)
                    throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
                break;
            case translate_kind::TRANSLATE_WRITE:
                throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
                break;
            default:
                break;
            }
        }
        else
        {
            bool writable = (segment->flags >> 1) & 1;
            switch (kind)
            {
            case translate_kind::TRANSLATE_WRITE:
                if (!writable)
                    throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
                break;
            case translate_kind::TRANSLATE_EXEC:
                throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
                break;
            default:
                break;
            }
        }
    }
    else
    {
        // TODO
        unimplemented("type_check called with a system-type segment! Execution "
               "correctness is not guaranteed past this point!\n");
    }
}

void cpu_t::limit_check(x86seg *segment, u32 offset, translate_kind kind)
{
    u8 fault_type = ABRT_GPF;
    u32 addr = offset & ((1 << (32 - 12)) - 1);

    if (segment == &segs[ss])
        fault_type = ABRT_SS;

    bool executable = (segment->flags >> 3) & 1;

    if (!executable)
    {
        // Data segment.
        bool expand_down = (segment->flags >> 2) & 1;
        bool big_seg = (segment->flags >> 14) & 1; // TODO: Not sure if this is ever used. Test this!
        bool granularity = (segment->flags >> 15) & 1;
        u32 lower_bound;
        u32 upper_bound;
        if (big_seg != granularity)
            printf("B bit doesn't equal granularity bit! Execution correctness "
                   "is not guaranteed past this point!\n");
        if (expand_down)
        {
            if (granularity)
            {
                lower_bound = ((addr << 12) | 0xfff) + 1;
                upper_bound = 0xffffffff; // 4G - 1
            }
            else
            {
                lower_bound = addr + 1;
                upper_bound = 0xffff; // 64K - 1
            }
        }
        else
        {
            lower_bound = 0;
            if (granularity)
                upper_bound = (addr << 12) | 0xfff;
            else
                upper_bound = addr;
        }
        if ((addr < lower_bound) || (addr > upper_bound))
            throw cpu_exception(exception_type::FAULT, fault_type, 0, true);
    }
    else
    {
        bool granularity = (segment->flags >> 15) & 1;
        u32 limit;

        if (granularity)
            limit = (addr << 12) | 0xfff;
        else
            limit = addr;

        if (addr > limit)
            throw cpu_exception(exception_type::FAULT, fault_type, 0, true);
    }
}

void cpu_t::privilege_check(x86seg *segment, u32 offset, translate_kind kind)
{
    bool not_system_seg = (segment->flags >> 4) & 1;
    bool executable = (segment->flags >> 3) & 1;
    int CPL = (segs[cs].flags >> 5) & 3;

    if (not_system_seg)
    {
        if (executable)
        {
            bool conforming = (segment->flags >> 2) & 1;
            if (conforming)
                return;
            else
            {
                int seg_rpl = segment->selector & 3;
                int dpl = (segment->flags >> 5) & 3;
                if (dpl < CPL)
                    throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
                if (dpl < seg_rpl)
                    throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
            }
        }
        else
        {
            int seg_rpl = segment->selector & 3;
            int dpl = (segment->flags >> 5) & 3;
            if (dpl < CPL)
                throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
            if (dpl < seg_rpl)
                throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
        }
    }
    else
    {
        // TODO
        printf("privilege_check_ref called with a system-type segment! "
               "Execution correctness is not guaranteed past this point!\n");
    }
}

bool cpu_t::page_privilege_check(u32 page_dir_entry, u32 page_tbl_entry)
{
    bool page_dir_user = (page_dir_entry >> 2) & 1;
    bool page_tbl_user = (page_tbl_entry >> 2) & 1;

    // If either page table entry is supervisor mode, the page is in supervisor
    // mode.
    return (!page_dir_user || !page_tbl_user) ? true : false;
}

bool cpu_t::page_writability_check(u32 page_dir_entry, u32 page_tbl_entry, bool is_user_page)
{
    bool page_dir_writable = (page_dir_entry >> 1) & 1;
    bool page_tbl_writable = (page_tbl_entry >> 1) & 1;

    // If we're in a supervisor page, it's writable.
    if (!is_user_page)
        return true;
    // If and only if both page table entries are writable, the page is
    // writable.
    else if (page_dir_writable && page_tbl_writable)
        return true;
    else
        return false;
}

u32 cpu_t::translate_addr(x86seg *segment, u32 offset, translate_kind kind)
{
    // Segment-level checks.
    type_check(segment, offset, kind);
    limit_check(segment, offset, kind);
    privilege_check(segment, offset, kind);

    u32 addr = segment->base + offset;

    if (!(cr[0] >> 31))
        return addr;

    // And now the paging stuff.
    u32 page_dir_base = cr[3] & 0xfffff000;
    u32 page_dir_entry_addr = ((addr >> 22) & 0x3ff) << 2;
    page_dir_entry_addr += page_dir_base;

    u32 page_dir_entry = rl_phys(page_dir_entry_addr);

    u32 page_tbl_base = page_dir_entry & 0xfffff000;
    u32 page_tbl_entry_addr = ((addr >> 12) & 0x3ff) << 2;
    page_tbl_entry_addr += page_tbl_base;

    u32 page_tbl_entry = rl_phys(page_tbl_entry_addr);

    bool page_present = (page_dir_entry & 1) && (page_tbl_entry & 1);
    int CPL = (segs[cs].flags >> 5) & 3;

    if (!page_present)
    {
        cr[2] = addr;
        u32 error = 0;
        if (kind == translate_kind::TRANSLATE_WRITE)
            error |= (1 << 1);
        if (CPL > 0)
            error |= (1 << 2);
        throw cpu_exception(exception_type::FAULT, ABRT_PF, error, true);
    }

    bool is_user_page = page_privilege_check(page_dir_entry, page_tbl_entry);

    if (CPL > 0)
    {
        if (!is_user_page)
        {
            cr[2] = addr;
            u32 error = 0x5;
            if (kind == translate_kind::TRANSLATE_WRITE)
                error |= (1 << 1);
            throw cpu_exception(exception_type::FAULT, ABRT_PF, error, true);
        }
    }

    if (kind == translate_kind::TRANSLATE_WRITE)
    {
        bool is_writable_page = page_writability_check(page_dir_entry, page_tbl_entry, is_user_page);
        if (!is_writable_page)
        {
            cr[2] = addr;
            u32 error = 3;
            if (CPL > 0)
                error |= (1 << 2);
            throw cpu_exception(exception_type::FAULT, ABRT_PF, error, true);
        }
    }

    // Now that ALL the checks are finally done, return a translated address.
    addr &= 0xfff;
    page_tbl_entry &= 0xfffff000;
    return page_tbl_entry + addr;
}

void cpu_t::load_segment(int segment, u16 selector)
{
    bool protected_mode = cr[0] & 1;
    if (protected_mode)
    {
        if (!eflags.virtual_8086_mode)
        {
            unimplemented("Protected mode segment loads are not implemented yet.\n");
        }
        else
        {
            segs[segment].selector = selector;
            segs[segment].base = selector << 4;
            segs[segment].limit = 0xffff;
            segs[segment].flags = (segment == cs) ? 0x00fb : 0x00f3;
        }
    }
    else
    {
        segs[segment].selector = selector;
        segs[segment].base = selector << 4;
        if (segment == cs)
        {
            segs[segment].flags = 0x009b;
        }
    }
}

u8 cpu_t::fetchb(u32 offset)
{
    addr_t addr = translate_addr(&segs[cs], offset, translate_kind::TRANSLATE_EXEC);
    return rb_phys(addr);
}

u16 cpu_t::fetchw(u32 offset)
{
    addr_t addr = translate_addr(&segs[cs], offset + 1, translate_kind::TRANSLATE_EXEC) - 1;
    return rw_phys(addr);
}

u32 cpu_t::fetchl(u32 offset)
{
    addr_t addr = translate_addr(&segs[cs], offset + 3, translate_kind::TRANSLATE_EXEC) - 3;
    return rl_phys(addr);
}

u8 cpu_t::rb(x86seg *segment, u32 offset)
{
    addr_t addr = translate_addr(segment, offset, translate_kind::TRANSLATE_READ);
    return rb_phys(addr);
}

u16 cpu_t::rw(x86seg *segment, u32 offset)
{
    addr_t addr = translate_addr(segment, offset + 1, translate_kind::TRANSLATE_READ) - 1;
    return rw_phys(addr);
}

u32 cpu_t::rl(x86seg *segment, u32 offset)
{
    addr_t addr = translate_addr(segment, offset + 3, translate_kind::TRANSLATE_READ) - 3;
    return rl_phys(addr);
}

void cpu_t::wb(x86seg *segment, u32 offset, u8 data)
{
    addr_t addr = translate_addr(segment, offset, translate_kind::TRANSLATE_WRITE);
    wb_phys(addr, data);
}

void cpu_t::ww(x86seg *segment, u32 offset, u16 data)
{
    addr_t addr = translate_addr(segment, offset + 1, translate_kind::TRANSLATE_WRITE) - 1;
    ww_phys(addr, data);
}

void cpu_t::wl(x86seg *segment, u32 offset, u32 data)
{
    addr_t addr = translate_addr(segment, offset + 3, translate_kind::TRANSLATE_WRITE) - 3;
    wl_phys(addr, data);
}

void cpu_t::decode_opcode()
{
    u8 opcode = fetchb(ip++);
    instruction[opcode_length] = opcode;
    opcode_length++;
    if (opcode_length == 15)
        throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
    printf("Opcode:%02x\nCS:%04x\nEIP:%08x\n", opcode, segs[cs].selector, ip - 1);
    if (operand_size)
    {
        if (opcode_table_1byte_32[opcode])
            (this->*opcode_table_1byte_32[opcode])();
    }
    else
    {
        if (opcode_table_1byte_16[opcode])
            (this->*opcode_table_1byte_16[opcode])();
    }
}

u8 cpu_t::decode_modrm_reg16_size16()
{
    u8 modrm = fetchb(ip++);
    mod_reg = (modrm >> 3) & 7;
    u8 mod = (modrm >> 6) & 3;
    mod_addr = 0;
    if(mod == 3) mod_reg_mem = modrm & 7;
    else
    {
        switch(mod)
        {
            case 0:
            {
                if((modrm & 7) == 6)
                {
                    mod_addr = (s16)fetchw(ip);
                    ip += 2;
                    mod_seg = ds;
                    return modrm;
                }
                break;
            }
            case 1: mod_addr = (s8)fetchb(ip++); break;
            case 2: mod_addr = (s16)fetchw(ip); ip += 2; break;
        }
        switch(modrm & 7)
        {
            case 0: mod_addr += BX + SI; mod_seg = ds; break;
            case 1: mod_addr += BX + DI; mod_seg = ds; break;
            case 2: mod_addr += BP + SI; mod_seg = ss; break;
            case 3: mod_addr += BP + DI; mod_seg = ss; break;
            case 4: mod_addr += SI; mod_seg = ds; break;
            case 5: mod_addr += DI; mod_seg = ds; break;
            case 6: mod_addr += BP; mod_seg = ss; break;
            case 7: mod_addr += BX; mod_seg = ds; break;
        }
    }
    return modrm;
}

u8 cpu_t::decode_modrm(int register_size)
{
    if((register_size == REG_16BIT) && !address_size) return decode_modrm_reg16_size16();
    else return 0;
}

void cpu_t::tick()
{
    operand_size = address_size = (segs[cs].flags >> 14) & 1;
    opcode_length = 0;
    if (delayed_interrupt_enable)
    {
        eflags.intr = 1;
        delayed_interrupt_enable = false;
    }
    try
    {
        decode_opcode();
    }
    catch (const cpu_exception &e)
    {
        printf("A fault occurred!");
    }
}

void cpu_t::run(s64 cycles)
{
    for (s64 i = 0ll; i < cycles; i++)
    {
        tick();
    }
}