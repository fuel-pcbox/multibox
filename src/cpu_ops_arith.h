void cpu_t::grp1_ev_i16()
{
    u8 modrm = decode_modrm(REG_16BIT);
    u8 op = (modrm >> 3) & 7;
    u8 mod = (modrm >> 6) & 3;
    switch(op)
    {
        case 7:
        {
            //CMP
            u16 imm = fetchw(ip);
            ip += 2;
            u32 result32 = 0;
            u16 result = 0;
            if(mod == 3)
            {
                result32 = regs[mod_reg_mem].w - (s16)imm;
                result = (u16)result32;
                if(imm > regs[mod_reg_mem].w) eflags.carry = 1;
                else eflags.carry = 0;
                
                u8 imm_adj = imm & 0xf;
                u8 reg_adj = regs[mod_reg_mem].w & 0xf;
                if(imm_adj > reg_adj) eflags.aux_carry = 1;
                else eflags.aux_carry = 0;
            }
            else
            {
                u16 mem_read = rw((seg_prefix < maxseg) ? seg_prefix : mod_seg, mod_addr);
                result32 = mem_read - (s16)imm;
                result = (u16)result;
                if(imm > mem_read) eflags.carry = 1;
                else eflags.carry = 0;

                u8 imm_adj = imm & 0xf;
                u8 reg_adj = mem_read & 0xf;
                if(imm_adj > reg_adj) eflags.aux_carry = 1;
                else eflags.aux_carry = 0;
            }

            if(!result) eflags.zero = 1;
            else eflags.zero = 0;
            if(result & 0x8000) eflags.sign = 1;
            else eflags.sign = 0;

            int parity = 1;
            for(int i = 0; i < 8; i++)
            {
                if(result & (1 << i)) parity ^= 1;
            }
            break;
        }
        default:
        {
            unhandled_opcode();
            break;
        }
    }
}

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
    if(parity == 1) eflags.parity = 1;
    else eflags.parity = 0;
}