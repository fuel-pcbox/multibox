void cpu_t::mov_al_i8()
{
    AL = fetchb(ip++);
}

void cpu_t::mov_cl_i8()
{
    CL = fetchb(ip++);
}

void cpu_t::mov_dl_i8()
{
    DL = fetchb(ip++);
}

void cpu_t::mov_bl_i8()
{
    BL = fetchb(ip++);
}

void cpu_t::mov_ah_i8()
{
    AH = fetchb(ip++);
}

void cpu_t::mov_ch_i8()
{
    CH = fetchb(ip++);
}

void cpu_t::mov_dh_i8()
{
    DH = fetchb(ip++);
}

void cpu_t::mov_bh_i8()
{
    BH = fetchb(ip++);
}

void cpu_t::mov_ax_i16()
{
    AX = fetchw(ip);
    ip+=2;
}

void cpu_t::mov_cx_i16()
{
    CX = fetchw(ip);
    ip+=2;
}

void cpu_t::mov_dx_i16()
{
    DX = fetchw(ip);
    ip+=2;
}

void cpu_t::mov_bx_i16()
{
    BX = fetchw(ip);
    ip+=2;
}

void cpu_t::mov_sp_i16()
{
    SP = fetchw(ip);
    ip+=2;
}

void cpu_t::mov_bp_i16()
{
    BP = fetchw(ip);
    ip+=2;
}

void cpu_t::mov_si_i16()
{
    SI = fetchw(ip);
    ip+=2;
}

void cpu_t::mov_di_i16()
{
    DI = fetchw(ip);
    ip+=2;
}

void cpu_t::mov_eax_i32()
{
    EAX = fetchl(ip);
    ip+=4;
}

void cpu_t::mov_ecx_i32()
{
    ECX = fetchl(ip);
    ip+=4;
}

void cpu_t::mov_edx_i32()
{
    EDX = fetchl(ip);
    ip+=4;
}

void cpu_t::mov_ebx_i32()
{
    EBX = fetchl(ip);
    ip+=4;
}

void cpu_t::mov_esp_i32()
{
    ESP = fetchl(ip);
    ip+=4;
}

void cpu_t::mov_ebp_i32()
{
    EBP = fetchl(ip);
    ip+=4;
}

void cpu_t::mov_esi_i32()
{
    ESI = fetchl(ip);
    ip+=4;
}

void cpu_t::mov_edi_i32()
{
    EDI = fetchl(ip);
    ip+=4;
}