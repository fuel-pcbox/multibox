void cpu_t::jmp_nz_i8()
{
    s8 off = fetchb(ip++);
    if(!eflags.zero) ip += (s8)off;
}

void cpu_t::jmp_rel8()
{
    s16 off = fetchb(ip++);
    ip += (s8)off;
    if(!operand_size) ip &= 0xffff;
}

void cpu_t::jmp_rel16()
{
    s16 off = fetchw(ip);
    ip += 2;
    ip += (s16)off;
    ip &= 0xffff;
}

void cpu_t::jmp_rel32()
{
    s32 off = fetchl(ip);
    ip += 4;
    ip += (s32)off;
}

void cpu_t::jmp_abs16()
{
    u16 off = fetchw(ip);
    ip += 2;
    u16 seg = fetchw(ip);
    ip += 2;

    bool protected_mode = cr[0] & 1;
    if (protected_mode && !eflags.virtual_8086_mode)
    {
        unimplemented("Protected mode far jumps are not implemented yet.\n");
    }
    else
    {
        ip = off;
        load_segment(cs, seg);
    }
}

void cpu_t::jmp_abs32()
{
    u32 off = fetchl(ip);
    ip += 4;
    u16 seg = fetchw(ip);
    ip += 2;

    bool protected_mode = cr[0] & 1;
    if (protected_mode && !eflags.virtual_8086_mode)
    {
        unimplemented("Protected mode far jumps are not implemented yet.\n");
    }
    else
    {
        ip = off;
        load_segment(cs, seg);
    }
}