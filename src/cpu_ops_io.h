void cpu_t::in_al_i8()
{
    bool protected_mode = cr[0] & 1;
    bool virt_8086_mode = eflags.virtual_8086_mode;
    int CPL = (segs[cs].flags >> 5) & 3;
    u8 port = fetchb(ip++);

    if (protected_mode && (virt_8086_mode || (CPL > eflags.iopl)))
    {
        unimplemented("Protected mode I/O is not implemented yet.\n");
    }
    else
    {
        AL = iorb(port);
    }
}

void cpu_t::out_al_i8()
{
    bool protected_mode = cr[0] & 1;
    bool virt_8086_mode = eflags.virtual_8086_mode;
    int CPL = (segs[cs].flags >> 5) & 3;
    u8 port = fetchb(ip++);

    if (protected_mode && (virt_8086_mode || (CPL > eflags.iopl)))
    {
        unimplemented("Protected mode I/O is not implemented yet.\n");
    }
    else
    {
        iowb(port, AL);
    }
}