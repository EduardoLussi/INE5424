// EPOS ARMv8 CPU Mediator Declarations

#ifndef __armv8_h
#define __armv8_h

#include <architecture/cpu.h>

__BEGIN_SYS

class ARMv8: protected CPU_Common
{
    friend class Init_System; // for CPU::init()

protected:
    static const bool multicore = Traits<System>::multicore;
    static const bool multitask = Traits<System>::multitask;

public:
    // CPU Native Data Types
    using CPU_Common::Reg8;
    using CPU_Common::Reg16;
    using CPU_Common::Reg32;
    using CPU_Common::Reg64;
    using CPU_Common::Reg;
    using CPU_Common::Log_Addr;
    using CPU_Common::Phy_Addr;

    class Context
    {
    public:
        Context() {}
        Context(Log_Addr usp, Log_Addr ulr, Reg flags, Log_Addr  lr, Log_Addr pc): _usp(usp), _ulr(ulr), _flags(flags), _lr(lr), _pc(pc) {
            if(Traits<Build>::hysterically_debugged || Traits<Thread>::trace_idle) {
                _x0 = 0; _x1 = 1; _x2 = 2; _x3 = 3; _x4 = 4; _x5 = 5; _x6 = 6; _x7 = 7; _x8 = 8; _x9 = 9; _x10 = 10; _x11 = 11; _x12 = 12;
            }
        }

    public:
        Reg _usp;     // usp (only used in multitasking)
        Reg _ulr;     // ulr (only used in multitasking)
        Reg _flags;
        Reg _x0;
        Reg _x1;
        Reg _x2;
        Reg _x3;
        Reg _x4;
        Reg _x5;
        Reg _x6;
        Reg _x7;
        Reg _x8;
        Reg _x9;
        Reg _x10;
        Reg _x11;
        Reg _x12;
        Reg _lr;
        Reg _pc;
    };

    // Interrupt Service Routines
    typedef void (* ISR)();

    // Fault Service Routines (exception handlers)
    typedef void (* FSR)();

protected:
    ARMv8() {};

public:
    static Log_Addr pc() { Reg r; ASM("mov %0, pc" : "=r"(r) :); return r; } // due to RISC pipelining, PC is read with a +8 (4 for thumb) offset

    static Log_Addr sp() { Reg r; ASM("mov %0, sp" : "=r"(r) :); return r; }
    static void sp(Log_Addr sp) { ASM("mov sp, %0" : : "r"(Reg(sp))); ASM("isb"); }

    static Log_Addr sp_el1() { Reg r; ASM("mrs %0, sp_el1" : "=r"(r) :); return r; }
    static void sp_el1(Log_Addr sp) { ASM("msr sp_el1, %0" : : "r"(Reg(sp))); ASM("isb"); }

    static Reg fr() { Reg r; ASM("mov %0, x0" : "=r"(r)); return r; }
    static void fr(Reg r) {  ASM("mov x0, %0" : : "r"(r) : "x0"); }

    static Log_Addr ra() { Reg r; ASM("mov %0, lr" : "=r"(r) :); return r; } // due to RISC pipelining, PC is read with a +8 (4 for thumb) offset

    static void halt() { ASM("wfi"); }

    template<typename T>
    static T tsl(volatile T & lock) {
        register T old = 0;
        //register T one = 1;
        /*ASM("1: ldrexb  %0, [%1]        \n"
            "   strexb  x3, %2, [%1]    \n"
            "   cmp     x3, #0          \n"
            "   bne     1b              \n" : "=&r"(old) : "r"(&lock), "r"(one) : "x3", "cc");*/
        return old;
    }

    template<typename T>
    static T finc(volatile T & value) {
        register T old = 0;
        /*if(sizeof(T) == sizeof(Reg8))
            ASM("1: ldrexb  %0, [%1]        \n"
                "   add     %0, #1          \n"
                "   strexb  x3, %0, [%1]    \n"
                "   cmp     x3, #0          \n"
                "   bne     1b              \n" : "=&r"(old) : "r"(&value) : "x3", "cc");
        else if(sizeof(T) == sizeof(Reg16))
            ASM("1: ldrexh  %0, [%1]        \n"
                "   add     %0, #1          \n"
                "   strexh  x3, %0, [%1]    \n"
                "   cmp     x3, #0          \n"
                "   bne     1b              \n" : "=&r"(old) : "r"(&value) : "x3", "cc");
        else
            ASM("1: ldrex   %0, [%1]        \n"
                "   add     %0, #1          \n"
                "   strex   x3, %0, [%1]    \n"
                "   cmp     x3, #0          \n"
                "   bne     1b              \n" : "=&r"(old) : "r"(&value) : "x3", "cc");*/
        return old - 1;
    }

    template<typename T>
    static T fdec(volatile T & value) {
        register T old = 0;
        /*if(sizeof(T) == sizeof(Reg8))
            ASM("1: ldrexb  %0, [%1]        \n"
                "   sub     %0, #1          \n"
                "   strexb  x3, %0, [%1]    \n"
                "   cmp     x3, #0          \n"
                "   bne     1b              \n" : "=&r"(old) : "r"(&value) : "x3", "cc");
        else if(sizeof(T) == sizeof(Reg16))
            ASM("1: ldrexh  %0, [%1]        \n"
                "   sub     %0, #1          \n"
                "   strexh  x3, %0, [%1]    \n"
                "   cmp     x3, #0          \n"
                "   bne     1b              \n" : "=&r"(old) : "r"(&value) : "x3", "cc");
        else
            ASM("1: ldrex   %0, [%1]        \n"
                "   sub     %0, #1          \n"
                "   strex   x3, %0, [%1]    \n"
                "   cmp     x3, #0          \n"
                "   bne     1b              \n" : "=&r"(old) : "r"(&value) : "x3", "cc");*/
        return old + 1;
    }

    template <typename T>
    static T cas(volatile T & value, T compare, T replacement) {
        register T old = 0;
        /*if(sizeof(T) == sizeof(Reg8))
            ASM("1: ldrexb  %0, [%1]        \n"
                "   cmp     %0, %2          \n"
                "   bne     2f              \n"
                "   strexb  x3, %3, [%1]    \n"
                "   cmp     x3, #0          \n"
                "   bne     1b              \n"
                "2:                         \n" : "=&r"(old) : "r"(&value), "r"(compare), "r"(replacement) : "x3", "cc");
        else if(sizeof(T) == sizeof(Reg16))
            ASM("1: ldrexh  %0, [%1]        \n"
                "   cmp     %0, %2          \n"
                "   bne     2f              \n"
                "   strexh  x3, %3, [%1]    \n"
                "   cmp     x3, #0          \n"
                "   bne     1b              \n"
                "2:                         \n" : "=&r"(old) : "r"(&value), "r"(compare), "r"(replacement) : "x3", "cc");
        else
            ASM("1: ldrex   %0, [%1]        \n"
                "   cmp     %0, %2          \n"
                "   bne     2f              \n"
                "   strex   x3, %3, [%1]    \n"
                "   cmp     x3, #0          \n"
                "   bne     1b              \n"
                "2:                         \n" : "=&r"(old) : "r"(&value), "r"(compare), "r"(replacement) : "x3", "cc");*/
        return old;
    }

    // ARMv8 specifics
    static Reg x0() { Reg r; ASM("mov %0, x0" : "=r"(r) : : ); return r; }
    static void x0(Reg r) { ASM("mov x0, %0" : : "r"(r): ); }

    static Reg x1() { Reg r; ASM("mov %0, x1" : "=r"(r) : : ); return r; }
    static void x1(Reg r) { ASM("mov x1, %0" : : "r"(r): ); }

    static Reg sctlr() { Reg r; ASM("mrs %0, sctlr_el1" : "=r"(r)); return r; }
    static void sctlr(Reg r) {  ASM("msr sctlr_el1, %0" : : "r"(r) : "x0"); }

    static Reg actlr() { Reg r; ASM("mrs %0, actlr_el1" : "=r"(r)); return r; }
    static void actlr(Reg r) {  ASM("msr actlr_el1, %0" : : "r"(r) : "x0"); }

    static void dsb() { /*ASM("dsb");*/ }
    static void isb() { ASM("isb"); }

    static void svc() { ASM("svc 0x0"); }
};

class ARMv8_M: public ARMv8
{
public:
    static const bool thumb = true;

    // CPU Flags
    typedef Reg Flags;
    enum {
        FLAG_THUMB      = 1 << 24,      // Thumb state
        FLAG_Q          = 1 << 27,      // DSP Overflow
        FLAG_V          = 1 << 28,      // Overflow
        FLAG_C          = 1 << 29,      // Carry
        FLAG_Z          = 1 << 30,      // Zero
        FLAG_N          = 1 << 31       // Negative
    };

    // Exceptions
    typedef Reg Exception_Id;
    enum {                      // Priority
        EXC_RESET       = 1,    // -3 (highest)
        EXC_NMI         = 2,    // -2
        EXC_HARD        = 3,    // -1
        EXC_MPU         = 4,    // programmable
        EXC_BUS         = 5,    // programmable
        EXC_USAGE       = 6,    // programmable
        EXC_SVCALL      = 11,   // programmable
        EXC_DEBUG       = 12,   // programmable
        EXC_PENDSV      = 14,   // programmable
        EXC_SYSTICK     = 15    // programmable
    };

    // CPU Context
    class Context: public ARMv8::Context
    {
    public:
        Context() {}
        Context(Log_Addr entry, Log_Addr exit, Log_Addr usp): ARMv8::Context(usp, exit | thumb, FLAG_THUMB, exit | thumb, entry | thumb) {}
    };

protected:
    ARMv8_M() {};

public:
    static unsigned int id() { return 0; }
    static unsigned int cores() { return 1; }

    static void int_enable()  { ASM("cpsie i"); }
    static void int_disable() { ASM("cpsid i"); }
    static bool int_enabled() { return !int_disabled(); }
    static bool int_disabled() {
        bool disabled;
        ASM("mrs %0, primask" : "=r"(disabled));
        return disabled;
    }

    static void smp_barrier(unsigned long cores = cores()) { assert(cores == 1); }

    static Reg pd() { return 0; }       // no MMU
    static void pd(Reg r) {}            // no MMU

    static void flush_tlb() {}          // no MMU
    static void flush_tlb(Reg r) {}     // no MMU

    // ARMv8-M specifics
    static Flags flags() { Reg r; ASM("mrs %0, xpsr"       : "=r"(r) :); return r; }
    static void flags(Flags r) {  ASM("msr xpsr_nzcvq, %0" : : "r"(r) : "cc"); }

    static void psr_to_x12() { ASM("mrs x12, xpsr" : : : "x12"); }
    static void x12_to_psr() {  ASM("msr xpsr_nzcvq, x12" : : : "cc"); }
};

class ARMv8_A: public ARMv8
{
public:
    static const bool thumb = false;

    // CPU Flags
    typedef Reg Flags;
    enum {
        FLAG_M          = 0x1f << 0,       // Processor Mode (5 bits)
        FLAG_T          = 1    << 5,       // Thumb state
        FLAG_F          = 1    << 6,       // FIQ disable
        FLAG_I          = 1    << 7,       // IRQ disable
        FLAG_A          = 1    << 8,       // Imprecise Abort disable
        FLAG_E          = 1    << 9,       // Endianess (0 ->> little, 1 -> big)
        FLAG_GE         = 0xf  << 16,      // SIMD Greater than or Equal (4 bits)
        FLAG_J          = 1    << 24,      // Jazelle state
        FLAG_Q          = 1    << 27,      // Underflow and/or DSP saturation
        FLAG_V          = 1    << 28,      // Overflow
        FLAG_C          = 1    << 29,      // Carry
        FLAG_Z          = 1    << 30,      // Zero
        FLAG_N          = 1    << 31,      // Negative

        // FLAG_M values
        MODE_USR        = 0x10,
        MODE_FIQ        = 0x11,
        MODE_IRQ        = 0x12,
        MODE_SVC        = 0x13,
        MODE_ABORT      = 0x17,
        MODE_UNDEFINED  = 0x1b,
        MODE_SYS        = 0x1f
    };

    // Exceptions
    typedef Reg Exception_Id;
    enum {
        EXC_START                   = 1,
        EXC_UNDEFINED_INSTRUCTION   = 2,
        EXC_SWI                     = 3,
        EXC_PREFETCH_ABORT          = 4,
        EXC_DATA_ABORT              = 5,
        EXC_RESERVED                = 6,
        EXC_IRQ                     = 7,
        EXC_FIQ                     = 8
    };

    enum {
        CLI_DOMAIN = 0x55555555, // 0b01 - Client, all memory domains check for memory access permission
        MNG_DOMAIN = 0xFFFFFFFF  // 0b11 - Manager, memory access permissions are not checked
    };

    // SCTLR bits
    enum {
        MMU_ENABLE  = 1 << 0,  // MMU enable
        DCACHE      = 1 << 2,  // Data cache enable
        BRANCH_PRED = 1 << 11, // Z bit, branch prediction enable
        ICACHE      = 1 << 12, // Instruction cache enable
        AFE         = 1 << 29  // Access Flag enable
    };

    // ACTLR bits
    enum {
        DCACHE_PREFE = 1 << 2, // DCache prefetch Enabled
        SMP          = 1 << 6 // SMP bit
    };

    // CPU Context
    class Context: public ARMv8::Context
    {
    public:
        Context() {}
        Context(Log_Addr entry, Log_Addr exit, Log_Addr usp): ARMv8::Context(usp, exit | thumb, multitask ? (usp ? MODE_USR : MODE_SVC) : MODE_SVC, exit | thumb, entry | thumb) {}
    };

protected:
    ARMv8_A() {};

public:
    /*static Flags flags() { return cpsr(); }
    static void flags(Flags flags) { cpsr(flags); }*/

    static Flags flags() { return 0; }
    static void flags(Flags flags) {}

    static unsigned int id() {
        Reg id = 0;
        //ASM("mrc p15, 0, %0, c0, c0, 5" : "=r"(id) : : );
        return id & 0x3;
    }

    static unsigned int cores() {
        if(Traits<Build>::MODEL == Traits<Build>::Raspberry_Pi3) {
            return Traits<Build>::CPUS;
        } else {
            Reg n = 0;
            /*ASM("mrc p15, 4, %0, c15, c0, 0 \t\n\
                 ldr %0, [%0, #0x004]" : "=r"(n) : : );*/
            return (n & 0x3) + 1;
        }
    }

    static void int_enable() {  flags(flags() & ~(FLAG_F | FLAG_I)); }
    static void int_disable() { flags(flags() | (FLAG_F | FLAG_I)); }

    static bool int_enabled() { return !int_disabled(); }
    static bool int_disabled() { return flags() & (FLAG_F | FLAG_I); }

    static void smp_barrier(unsigned long cores = cores()) { CPU_Common::smp_barrier<&finc>(cores, id()); }

    static void fpu_save() {    ASM("vpush {s0-s15} \n vpush {s16-s31}"); }
    static void fpu_restore() { ASM("vpop  {s0-s15} \n vpop  {s16-s31}"); }

    // ARMv8-A specifics
    /*static Reg cpsr() { Reg r; ASM("mrs %0, cpsr" : "=r"(r) : : ); return r; }
    static void cpsr(Reg r) { ASM("msr cpsr, %0" : : "r"(r) : "cc"); }

    static Reg cpsrc() { Reg r; ASM("mrs %0, cpsr_c" : "=r"(r) : : ); return r; }
    static void cpsrc(Reg r) { ASM("msr cpsr_c, %0" : : "r"(r): ); }

    static void psr_to_x12() { ASM("mrs x12, cpsr" : : : "x12"); }
    static void x12_to_psr() { ASM("msr cpsr, x12" : : : "cc"); }*/

    static Reg cpsr() { return 0; }
    static void cpsr(Reg r) {}

    static Reg cpsrc() { return 0; }
    static void cpsrc(Reg r) {}

    static void psr_to_x12() {}
    static void x12_to_psr() {}

    static void save_regs(bool ret = false) {
        if(ret)
            ASM("stmfd sp!, {x0-x3, x12, lr, pc}");
        else
            ASM("stmfd sp!, {x0-x3, x12, lr}");
    }

    static void restore_regs(bool ret = false) {
        if(ret)
            ASM("ldmfd   sp!, {x0-x3, x12, lr, pc}^");  // including PC in ldmfd cause a mode change to the mode given by PSR (the mode the CPU was before the interrupt)
        else
            ASM("ldmfd sp!, {x0-x3, x12, lr}");
    }

    static void mode(unsigned int m) { ASM("msr cpsr_c, %0" : : "i"(m | FLAG_F | FLAG_I) : "cc"); }

    static void svc_enter(unsigned int from, bool ret = true) {
        mode(MODE_SVC);                 // go to SVC mode to save context
        save_regs(ret);                 // save current context (lr, sp and spsr are banked registers)
        mode(from);                     // go back to mode "from" to recover LR and PSR
        ASM("sub x1, lr, #4");          // x1 = return address when entering mode "from"
        ASM("mrs x2, spsr");            // x2 = "from"_spsr (to be visible at SVC)
        mode(MODE_SVC);                 // go to SVC mode once again
        if(ret) {                       // if we will return, then
            ASM("str x1, [sp, #24]");   // overwrite the saved PC with x1, which contains the recovered return address
            ASM("push {x2}");           // push the recovered PSR
        }
        // upon return, x1 (LR) and x2 (PSR) are preserved and can be further used
    }

    static void svc_leave() {
        ASM("pop {x2}");            // pop saved mode "from" PSR into SVC_spsr
        ASM("msr spsr_cfxs, x2");
        restore_regs(true);              // restore the context, eventually changing back to "from" mode and jumping to the saved return address
    }

    static void svc_stay() { restore_regs(false); }

    static Reg elr_hyp() { Reg r; ASM("mrs %0, ELR_hyp" : "=r"(r) : : ); return r; }
    static void elr_hyp(Reg r) { ASM("msr ELR_hyp, %0" : : "r"(r): ); }

    static void ldmia() { /*ASM("ldmia x0!,{x2,x3,x4,x5,x6,x7,x8,x9}" : : : ); */}
    static void stmia() { /*ASM("stmia x1!,{x2,x3,x4,x5,x6,x7,x8,x9}" : : : );*/ }

    // CP15 operations
    static Reg ttbr0() { Reg r; ASM ("mrs %0, ttbr0_el1" : "=r"(r) : :); return r; }
    static void ttbr0(Reg r) {  ASM ("msr ttbr0_el1, %0" : : "p"(r) :); }

    static Reg ttbcr() { Reg r; ASM ("mrs %0, vtcr_el2" : "=r"(r) : :); return r; }
    static void ttbcr(Reg r) {  ASM ("msr vtcr_el2, %0" : : "p"(r) :); }

    static Reg dacr() { Reg r; ASM ("mrs %0, dacr32_el2" : "=r"(r) : :); return r; }
    static void dacr(Reg r) {  ASM ("msr dacr32_el2, %0" : : "p"(r) :); }

    static Reg pd() { return ttbr0(); }
    static void pd(Reg r) {  ttbr0(r); }

    static void flush_tlb() {      /*ASM("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));*/ } // TLBIALL - invalidate entire unifed TLB
    static void flush_tlb(Reg r) { /*ASM("mcr p15, 0, %0, c8, c7, 0" : : "r" (r));*/ }

    static void flush_branch_predictors() { /*ASM("mcr p15, 0, %0, c7, c5, 6" : : "r" (0));*/ }

    static void flush_caches() {/*
        ASM("// Disable L1 Caches                                                                       \t\n\
             mrc     p15, 0, x1, c1, c0, 0      // read SCTLR                                           \t\n\
             bic     x1, x1, #(0x1 << 2)        // disable D Cache                                      \t\n\
             mcr     p15, 0, x1, c1, c0, 0      // write SCTLR                                          \t\n\
                                                                                                        \t\n\
             // Invalidate Data cache, calculating the cache size and looping through each set and way  \t\n\
             mov     x0, #0x0                   // x0 = 0x0 for L1 dcache 0x2 for L2 dcache             \t\n\
             mcr     p15, 2, x0, c0, c0, 0      // CSSELR cache size selection Register                 \t\n\
             mrc     p15, 1, x4, c0, c0, 0      // CCSIDR read cache size                               \t\n\
             and     x1, x4, #0x7                                                                       \t\n\
             add     x1, x1, #0x4               // x1 = cache line size                                 \t\n\
             ldr     x3, =0x7fff                                                                        \t\n\
             and     x2, x3, x4, lsr #13        // x2 = cache set number - 1                            \t\n\
             ldr     x3, =0x3ff                                                                         \t\n\
             and     x3, x3, x4, lsr #3         // x3 = cache associativity number - 1                  \t\n\
             clz     x4, x3                     // x4 = way position in CISW instruction                \t\n\
             mov     x5, #0                     // x5 = way loop counter                                \t\n\
         way_loop:                                                                                      \t\n\
             mov     x6, #0                     // x6 = set loop counter                                \t\n\
         set_loop:                                                                                      \t\n\
             orr     x7, x0, x5, lsl x4         // set way                                              \t\n\
             orr     x7, x7, x6, lsl x1         // set set                                              \t\n\
             mcr     p15, 0, x7, c7, c6, 2      // DCCISW x7                                            \t\n\
             add     x6, x6, #1                 // increment set counter                                \t\n\
             cmp     x6, x2                     // last set reached?                                    \t\n\
             ble     set_loop                   // if not, iterate set_loop                             \t\n\
             add     x5, x5, #1                 // else, next way                                       \t\n\
             cmp     x5, x3                     // last way reached?                                    \t\n\
             ble     way_loop                   // if not, iterate way_loop                                  ");*/
    }

    static void enable_fpu() {/*
        // This code assumes a compilation with mfloat-abi=hard and does not care for context switches
        ASM("mrc     p15, 0, x0, c1, c0, 2                                              \t\n\
             orr     x0, x0, #0x300000           // single precision                    \t\n\
             orr     x0, x0, #0xc00000           // double precision                    \t\n\
             mcr     p15, 0, x0, c1, c0, 2                                              \t\n\
             mov     x0, #0x40000000                                                    \t\n\
             fmxr    fpexc,x0                                                                ");*/
    }

};

#ifndef __armv9_h

class CPU: public SWITCH<Traits<Build>::MODEL, CASE<Traits<Build>::eMote3, ARMv8_M, CASE<Traits<Build>::LM3S811, ARMv8_M, CASE<DEFAULT, ARMv8_A>>>>::Result
{
    friend class Init_System;

private:
    typedef SWITCH<Traits<Build>::MODEL, CASE<Traits<Build>::eMote3, ARMv8_M, CASE<Traits<Build>::LM3S811, ARMv8_M, CASE<DEFAULT, ARMv8_A>>>>::Result Base;

public:
    // CPU Native Data Types
    using ARMv8::Reg8;
    using ARMv8::Reg16;
    using ARMv8::Reg32;
    using ARMv8::Reg64;
    using ARMv8::Reg;
    using ARMv8::Log_Addr;
    using ARMv8::Phy_Addr;

    // CPU Context
    class Context: public Base::Context
    {
    public:
        Context() {}
        Context(Log_Addr entry, Log_Addr exit, Log_Addr usp): Base::Context(entry, exit, usp) {}

        void save() volatile;
        void load() const volatile;

        friend Debug & operator<<(Debug & db, const Context & c) {
            db << hex
               << "{x0="  << c._x0
               << ",x1="  << c._x1
               << ",x2="  << c._x2
               << ",x3="  << c._x3
               << ",x4="  << c._x4
               << ",x5="  << c._x5
               << ",x6="  << c._x6
               << ",x7="  << c._x7
               << ",x8="  << c._x8
               << ",x9="  << c._x9
               << ",x10=" << c._x10
               << ",x11=" << c._x11
               << ",x12=" << c._x12
               << ",sp="  << &c
               << ",lr="  << c._lr
               << ",pc="  << c._pc
               << ",psr=" << c._flags
               << ",usp=" << c._usp
               << ",ulr=" << c._ulr
               << "}" << dec;
            return db;
        }
    };

public:
    CPU() {}

    using ARMv8::pc;
    using ARMv8::ra;
    using ARMv8::sp;
    using ARMv8::fr;

    using Base::id;
    using Base::cores;

    static Hertz clock() { return _cpu_clock; }
    static void clock(const Hertz & frequency); // defined along with each machine's IOCtrl
    static Hertz max_clock();
    static Hertz min_clock();
    static Hertz bus_clock() { return _bus_clock; }

    using Base::int_enable;
    using Base::int_disable;
    using Base::int_enabled;
    using Base::int_disabled;

    using ARMv8::halt;

    using Base::fpu_save;
    using Base::fpu_restore;

    using ARMv8::tsl;
    using ARMv8::finc;
    using ARMv8::fdec;
    using ARMv8::cas;

    static void switch_context(Context ** o, Context * n);

    template<typename ... Tn>
    static Context * init_stack(Log_Addr usp, Log_Addr sp, void (* exit)(), int (* entry)(Tn ...), Tn ... an) {
        sp -= sizeof(Context);
        Context * ctx = new(sp) Context(entry, exit, usp); // init_stack is called with usp = 0 for kernel threads
        init_stack_helper(&ctx->_x0, an ...);
        return ctx;
    }

    // In ARMv8, the main thread of each task gets parameters over registers, not the stack, and they are initialized by init_stack.
    template<typename ... Tn>
    static Log_Addr init_user_stack(Log_Addr usp, void (* exit)(), Tn ... an) { return usp; }

    static void syscall(void * message);
    static void syscalled();

    using CPU_Common::htole64;
    using CPU_Common::htole32;
    using CPU_Common::htole16;
    using CPU_Common::letoh64;
    using CPU_Common::letoh32;
    using CPU_Common::letoh16;

    using CPU_Common::htobe64;
    using CPU_Common::htobe32;
    using CPU_Common::htobe16;
    using CPU_Common::betoh64;
    using CPU_Common::betoh32;
    using CPU_Common::betoh16;

    using CPU_Common::htonl;
    using CPU_Common::htons;
    using CPU_Common::ntohl;
    using CPU_Common::ntohs;

private:
    template<typename Head, typename ... Tail>
    static void init_stack_helper(Log_Addr sp, Head head, Tail ... tail) {
        *static_cast<Head *>(sp) = head;
        init_stack_helper(sp + sizeof(Head), tail ...);
    }
    static void init_stack_helper(Log_Addr sp) {}

    static void context_load_helper();

    static void init();

private:
    static unsigned int _cpu_clock;
    static unsigned int _bus_clock;
};

inline CPU::Reg64 htole64(CPU::Reg64 v) { return CPU::htole64(v); }
inline CPU::Reg32 htole32(CPU::Reg32 v) { return CPU::htole32(v); }
inline CPU::Reg16 htole16(CPU::Reg16 v) { return CPU::htole16(v); }
inline CPU::Reg64 letoh64(CPU::Reg64 v) { return CPU::letoh64(v); }
inline CPU::Reg32 letoh32(CPU::Reg32 v) { return CPU::letoh32(v); }
inline CPU::Reg16 letoh16(CPU::Reg16 v) { return CPU::letoh16(v); }

inline CPU::Reg64 htobe64(CPU::Reg64 v) { return CPU::htobe64(v); }
inline CPU::Reg32 htobe32(CPU::Reg32 v) { return CPU::htobe32(v); }
inline CPU::Reg16 htobe16(CPU::Reg16 v) { return CPU::htobe16(v); }
inline CPU::Reg64 betoh64(CPU::Reg64 v) { return CPU::betoh64(v); }
inline CPU::Reg32 betoh32(CPU::Reg32 v) { return CPU::betoh32(v); }
inline CPU::Reg16 betoh16(CPU::Reg16 v) { return CPU::betoh16(v); }

inline CPU::Reg32 htonl(CPU::Reg32 v)   { return CPU::htonl(v); }
inline CPU::Reg16 htons(CPU::Reg16 v)   { return CPU::htons(v); }
inline CPU::Reg32 ntohl(CPU::Reg32 v)   { return CPU::ntohl(v); }
inline CPU::Reg16 ntohs(CPU::Reg16 v)   { return CPU::ntohs(v); }

#endif

__END_SYS

#endif
