// EPOS ARMv7 System Call Entry Implementation

#include <architecture/armv8/armv8_cpu.h>

extern "C" { void _exec(void *); }

__BEGIN_SYS

void CPU::syscalled()
{
    if(Traits<Build>::MODE == Traits<Build>::KERNEL)
        _exec(reinterpret_cast<void *>(CPU::r1()));             // the message to EPOS Framework is passed on register r1
}

void CPU::context_load_helper()
{/*
#ifdef __cortex_a__
    ASM("       pop     {r12}                           \n"
        "       msr     sp_usr, r12                     \n"
        "       pop     {r12}                           \n"
        "       msr     lr_usr, r12                     \n"
        "       pop     {r12}                           \n"
        "       msr     spsr_cfxs, r12                  \n"
        "       ldmfd   sp!, {r0-r12, lr, pc}^          \n");
#endif*/
}

__END_SYS
