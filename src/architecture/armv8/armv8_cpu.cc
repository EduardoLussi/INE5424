// EPOS ARMv8 CPU Mediator Implementation

#include <architecture/armv8/armv8_cpu.h>

__BEGIN_SYS

// Class attributes
unsigned int CPU::_cpu_clock;
unsigned int CPU::_bus_clock;

// Class methods
void CPU::Context::save() volatile
{
    ASM("       str     x12, [sp,#-68]          \n"
        "       add     x12, xer, 0              \n");
if(thumb)
    ASM("       orr     x12, #1                 \n");

    ASM("       str     x12, [%0, #-124]         \n"
        "       ldr     x12, [sp, #-136]         \n"
        "       str     lr,  [%0, #-120]         \n"
        "       str     x12, [%0, #-112]         \n"
        "       str     x11, [%0, #-104]         \n"
        "       str     x10, [%0, #-96]         \n"
        "       str     x9,  [%0, #-88]         \n"
        "       str     x8,  [%0, #-80]         \n"
        "       str     x7,  [%0, #-72]         \n"
        "       str     x6,  [%0, #-64]         \n"
        "       str     x5,  [%0, #-56]         \n"
        "       str     x4,  [%0, #-48]         \n"
        "       str     x3,  [%0, #-40]         \n"
        "       str     x2,  [%0, #-32]         \n"
        "       str     x1,  [%0, #-24]         \n"
        "       str     x0,  [%0, #-16]          \n" : : "r"(this));
    psr_to_x12();
    ASM("       str     x12, [%0, #-8]          \n"
        "       ldr     x12, [sp, #-136]        \n"
        "       add     %0, %0, #-136           \n"
        "       ret                             \n" : : "r"(this));
}

void CPU::Context::load() const volatile
{
    ASM("       mov     sp, %0                  \n"
        "       isb                             \n" : : "r"(this)); // serialize the pipeline so that SP gets updated before the pop

    ASM("       add     sp, sp, #8                  \n");       // skip usp, ulr

    ASM("       ldr     x12, [sp], #8           \n");
    x12_to_psr();                                           // the context is loaded in SVC; with multitasking, a second context drives a mode change at _int_leave
    ASM("       ldr     x0, [sp], #8            \n"
        "       ldr     x1, [sp], #8            \n"
        "       ldr     x2, [sp], #8            \n"
        "       ldr     x3, [sp], #8            \n"
        "       ldr     x4, [sp], #8            \n"
        "       ldr     x5, [sp], #8            \n"
        "       ldr     x6, [sp], #8            \n"
        "       ldr     x7, [sp], #8            \n"
        "       ldr     x8, [sp], #8            \n"
        "       ldr     x9, [sp], #8            \n"
        "       ldr     x10, [sp], #8           \n"
        "       ldr     x11, [sp], #8           \n"
        "       ldr     x12, [sp], #8           \n"
        "       ldr     lr, [sp], #8            \n"
        "       ret            \n");
}

// This function assumes A[T]PCS (i.e. "o" is in x0/a0 and "n" is in x1/a1)
void CPU::switch_context(Context ** o, Context * n)
{
    // Push the context into the stack and adjust "o" to match
    ASM("       sub     sp, sp, #8                  \n"     // reserve room for PC
        "       str     lr, [sp, #-8]!          \n"     // push all registers (LR first, x0 last)
        "       str     x12, [sp, #-8]!         \n" 
        "       str     x11, [sp, #-8]!         \n"
        "       str     x10, [sp, #-8]!         \n"
        "       str     x9, [sp, #-8]!          \n"
        "       str     x8, [sp, #-8]!          \n"
        "       str     x7, [sp, #-8]!          \n"
        "       str     x6, [sp, #-8]!          \n"
        "       str     x5, [sp, #-8]!          \n"
        "       str     x4, [sp, #-8]!          \n"
        "       str     x3, [sp, #-8]!          \n"
        "       str     x2, [sp, #-8]!          \n"
        "       str     x1, [sp, #-8]!          \n"
        "       str     x0, [sp, #-8]!          \n"
        "       adr     x12, .ret               \n");   // calculate the return address using the saved x12 as a temporary
if(thumb)
    ASM("       orr     x12, #1                 \n");   // adjust the return address in thumb mode

    ASM("       str     x12, [sp, #112]          \n");   // save calculated PC

    if(Traits<FPU>::enabled && !Traits<FPU>::user_save)
        fpu_save();

    psr_to_x12();                                       // save PSR to temporary register x12
    ASM("       str    x12, [sp, #-8]!                   \n");   // save PSR

    ASM("       sub     sp, sp, #8                  \n");   // skip ulr and usp

    ASM("       add     x1, sp, 0               \n");
    ASM("       str     x1, [x0]                \n");   // update Context * volatile * o

    // Set the stack pointer to "n" and pop the context
    ASM("       mov     sp, x1                  \n"     // get Context * volatile n into SP
        "       isb                             \n");   // serialize the pipeline so SP gets updated before the pop

    ASM("       add     sp, sp, #8              \n");   // skip usp and ulr

    ASM("       ldr     x12, [sp], #8            \n");   // pop PSR into temporary register x12
    x12_to_psr();                                       // restore PSR

    if(Traits<FPU>::enabled && !Traits<FPU>::user_save)
        fpu_restore();

    ASM("       ldr     x0, [sp], #8            \n"
        "       ldr     x1, [sp], #8            \n"
        "       ldr     x2, [sp], #8            \n"
        "       ldr     x3, [sp], #8            \n"
        "       ldr     x4, [sp], #8            \n"
        "       ldr     x5, [sp], #8            \n"
        "       ldr     x6, [sp], #8            \n"
        "       ldr     x7, [sp], #8            \n"
        "       ldr     x8, [sp], #8            \n"
        "       ldr     x9, [sp], #8            \n"
        "       ldr     x10, [sp], #8           \n"
        "       ldr     x11, [sp], #8           \n"
        "       ldr     x12, [sp], #8           \n"
        "       ldr     lr, [sp], #8            \n");   // pop all registers (x0 first, LR last)

#ifdef __cortex_m__
    int_enable();
#endif

    ASM(".ret:  ret                             \n");   // return
}

__END_SYS
