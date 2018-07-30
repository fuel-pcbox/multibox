#pragma once

#include "common.h"

enum class translate_kind
{
    TRANSLATE_READ, TRANSLATE_WRITE, TRANSLATE_EXEC
};

enum cpu_type
{
    intel386
};

union x86reg
{
    u64 q;
    u32 l;
    u16 w;
    struct
    {
        u8 l;
        u8 h;
    } b;
};

struct x86seg
{
    u16 selector;
    addr_t base;
    u32 limit;
    u16 flags;
};

struct cpu_t
{
    cpu_type type;

    x86reg regs[8];

    x86seg segs[6];

    union
    {
        struct
        {
            u32 carry : 1;
            u32 reserved1 : 1;
            u32 parity : 1;
            u32 reserved2 : 1;
            u32 aux_carry : 1;
            u32 reserved3 : 1;
            u32 zero : 1;
            u32 sign : 1;
            u32 trap : 1;
            u32 intr : 1;
            u32 direction : 1;
            u32 overflow : 1;
            u32 io_priv_level : 2;
            u32 nested_task : 1;
            u32 reserved4 : 1;
            u32 resume : 1;
            u32 virtual_8086_mode : 1;
            u32 reserved5 : 14;
        };
        u16 word;
        u32 whole;
    } eflags;

    x86seg gdtr, ldtr, idtr, task_reg;

    u32 ip;

    u32 dr[8];
    u32 cr[8];

    u8 instruction[16]; //if the length of the instruction goes over 15 bytes, #GP
    int opcode_length;

    bool operand_size, address_size;

    void* device;

    std::function<u8(void*, addr_t)> rb_real;
    std::function<u16(void*, addr_t)> rw_real;
    std::function<u32(void*, addr_t)> rl_real;
    std::function<void(void*, addr_t, u8)> wb_real;
    std::function<void(void*, addr_t, u16)> ww_real;
    std::function<void(void*, addr_t, u32)> wl_real;

    std::function<u8(void*, u16)> iorb_real;
    std::function<u16(void*, u16)> iorw_real;
    std::function<u32(void*, u16)> iorl_real;
    std::function<void(void*, u16, u8)> iowb_real;
    std::function<void(void*, u16, u16)> ioww_real;
    std::function<void(void*, u16, u32)> iowl_real;

    typedef void (cpu_t::*opcode_func)();
    struct opcode_info
    {
        u8 opcode;
        u64 cpu_level;
        u64 flags;
        opcode_func handler16;
        opcode_func handler32;
        bool lockable;
    };

    static const opcode_info cpu_opcode_table[];

    opcode_func opcode_table_1byte_16[256];
    opcode_func opcode_table_1byte_32[256];

    void unhandled_opcode();

    void jmp_abs16();

    void jmp_abs32();

    void init(cpu_type _type);

    u8 rb_phys(addr_t addr);
    u16 rw_phys(addr_t addr);
    u32 rl_phys(addr_t addr);
    void wb_phys(addr_t addr, u8 data);
    void ww_phys(addr_t addr, u16 data);
    void wl_phys(addr_t addr, u32 data);

    void type_check(x86seg* segment, u32 offset, translate_kind kind);
    void limit_check(x86seg* segment, u32 offset, translate_kind kind);
    void privilege_check(x86seg* segment, u32 offset, translate_kind kind);
    bool page_privilege_check(u32 page_dir_entry, u32 page_tbl_entry);
    bool page_writability_check(u32 page_dir_entry, u32 page_tbl_entry, bool is_user_page);
    u32 translate_addr(x86seg* segment, u32 offset, translate_kind kind);

    void load_segment(int segment, u16 selector);

    u8 fetchb(u32 addr);
    u16 fetchw(u32 addr);
    u32 fetchl(u32 addr);
    u8 rb(x86seg* segment, u32 addr);
    u16 rw(x86seg* segment, u32 addr);
    u32 rl(x86seg* segment, u32 addr);
    void wb(x86seg* segment, u32 addr, u8 data);
    void ww(x86seg* segment, u32 addr, u16 data);
    void wl(x86seg* segment, u32 addr, u32 data);

    void tick();
    void run(s64 cycles);
};

#define EAX	regs[0].l
#define AX	regs[0].w
#define AL	regs[0].b.l
#define AH	regs[0].b.h
#define ECX	regs[1].l
#define CX	regs[1].w
#define CL	regs[1].b.l
#define CH	regs[1].b.h
#define EDX	regs[2].l
#define DX	regs[2].w
#define DL	regs[2].b.l
#define DH	regs[2].b.h
#define EBX	regs[3].l
#define BX	regs[3].w
#define BL	regs[3].b.l
#define BH	regs[3].b.h
#define ESP	regs[4].l
#define EBP	regs[5].l
#define ESI	regs[6].l
#define EDI	regs[7].l
#define SP	regs[4].w
#define BP	regs[5].w
#define SI	regs[6].w
#define DI	regs[7].w

enum segment_t
{
    es = 0, cs, ss, ds, fs, gs,
    maxseg = 6,
};

enum class exception_type
{
    FAULT, TRAP, ABORT
};

enum
{
    ABRT_NONE = 0,
    ABRT_GEN,
    ABRT_TS  = 0xA,
    ABRT_NP  = 0xB,
    ABRT_SS  = 0xC,
    ABRT_GPF = 0xD,
    ABRT_PF  = 0xE
};

struct cpu_exception
{
    exception_type type;
    u8 fault_type;
    u32 error_code;
    bool error_code_valid;
    cpu_exception(exception_type _type, u8 _fault_type, u32 errcode, bool errcodevalid)
        : type(_type)
        , fault_type(_fault_type)
        , error_code(errcode)
        , error_code_valid(errcodevalid) {}

    cpu_exception(const cpu_exception& other) = default;
};