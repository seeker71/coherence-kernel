/* entry.S — HatiOS 32-bit entry, interrupt stubs, and the context switch.
 *
 * The kernel image begins here at 0x10000 (see linker.ld): boot.S jumps to
 * the first byte of .text.entry, which hands the CPU to kmain in C.
 */
    .code32
    .section .text.entry
    .global kentry
kentry:
    movl    $0x90000, %esp
    call    kmain
1:  hlt
    jmp     1b

    .text

/* timer interrupt: preserve caller-saved registers, let C decide who runs
 * next. If timer_tick switches tasks, we return here only when THIS task is
 * given breath again — the trap frame waits on this task's own stack. */
    .global timer_stub
timer_stub:
    pushl   %eax
    pushl   %ecx
    pushl   %edx
    call    timer_tick
    popl    %edx
    popl    %ecx
    popl    %eax
    iret

/* any other vector: acknowledge nothing, change nothing, return. A masked
 * PIC keeps these rare; exceptions landing here are an honest boundary. */
    .global default_stub
default_stub:
    iret

/* switch_ctx(u32 *save_esp, u32 load_esp)
 *
 * Saves eflags + callee-saved registers on the current stack, records esp
 * through save_esp, adopts load_esp, restores the mirrored frame, and rets
 * into the next task. A freshly created task fakes exactly this frame with
 * its entry function in the return slot (kernel.c task_create). */
    .global switch_ctx
switch_ctx:
    movl    4(%esp), %eax       /* &prev->esp */
    movl    8(%esp), %edx       /* next->esp  */
    pushfl
    pushl   %ebp
    pushl   %edi
    pushl   %esi
    pushl   %ebx
    movl    %esp, (%eax)
    movl    %edx, %esp
    popl    %ebx
    popl    %esi
    popl    %edi
    popl    %ebp
    popfl
    ret
