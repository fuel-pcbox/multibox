void cpu_t::nop()
{
}

void cpu_t::cli()
{
    bool protected_mode = cr[0] & 1;
    if (protected_mode)
    {
        int CPL = (segs[cs].flags >> 5) & 3;
        if (CPL > eflags.iopl)
            throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
    }
    eflags.intr = 0;
}

void cpu_t::sti()
{
    bool protected_mode = cr[0] & 1;
    if (protected_mode && !eflags.virtual_8086_mode)
    {
        int CPL = (segs[cs].flags >> 5) & 3;
        if (CPL > eflags.iopl)
            throw cpu_exception(exception_type::FAULT, ABRT_GPF, 0, true);
    }
    delayed_interrupt_enable = true;
}

void cpu_t::cld()
{
    eflags.direction = 0;
}