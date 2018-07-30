enum
{
    OP_2BYTE = (1ull << 0),

    OP_FPU = (1ull << 0),
    OP_I486 = (1ull << 1),
};

const cpu_t::opcode_info cpu_t::cpu_opcode_table[] =
{
    {0xea, 0, 0, &cpu_t::jmp_abs16, nullptr, false},
};