void cpu_t::test_al_i8()
{
    u8 result = AL & fetchb(ip++);

    eflags.carry = 0;
    eflags.overflow = 0;

    if(!result) eflags.zero = 1;
    else eflags.zero = 0;
    if(result & 0x80) eflags.sign = 1;
    else eflags.sign = 0;

    int parity = 1;
    for(int i = 0; i < 8; i++)
    {
        if(result & (1 << i)) parity ^= 1;
    }
}