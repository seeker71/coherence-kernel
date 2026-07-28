/* boot.S — the HatiOS boot sector: 512 bytes, the first breath.
 *
 * The BIOS drops us at 0x7C00 in 16-bit real mode with nothing but a CPU
 * and firmware interrupts. This sector:
 *   1. loads the kernel (62 sectors, CHS 0/0/2..63 — one track's honest
 *      maximum on the standard 63-sectors-per-track geometry) to 0x10000,
 *   2. opens the A20 gate (port 0x92) so memory above 1 MB is addressable,
 *   3. installs a flat GDT and enters 32-bit protected mode,
 *   4. far-jumps into the kernel entry.
 *
 * GNU assembler syntax; assembled 16-bit with .code16 and linked to 0x7C00.
 */
    .code16
    .global _start
_start:
    cli
    xorw    %ax, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss
    movw    $0x7C00, %sp
    sti

    /* say B on the BIOS teletype so a dead disk is loud, not silent */
    movb    $0x0E, %ah
    movb    $'B', %al
    int     $0x10

    /* read kernel: 63 sectors starting at CHS (0,0,2) -> es:bx = 0x1000:0000 */
    movw    $0x1000, %ax
    movw    %ax, %es
    xorw    %bx, %bx
    movb    $0x02, %ah          /* BIOS read sectors */
    movb    $62,   %al          /* count: sectors 2..63, one full track */
    movb    $0,    %ch          /* cylinder 0 */
    movb    $2,    %cl          /* sector 2 (1-based; sector 1 is us) */
    movb    $0,    %dh          /* head 0; dl still holds the boot drive */
    int     $0x13
    jc      disk_fail

    /* A20 via the fast gate */
    inb     $0x92, %al
    orb     $2, %al
    outb    %al, $0x92

    /* flat GDT, protected mode, and the far jump into 32-bit code */
    cli
    lgdt    gdtr
    movl    %cr0, %eax
    orl     $1, %eax
    movl    %eax, %cr0
    ljmp    $0x08, $pm_entry

disk_fail:
    movb    $0x0E, %ah
    movb    $'X', %al
    int     $0x10
hang:
    hlt
    jmp     hang

    .code32
pm_entry:
    movw    $0x10, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss
    movw    %ax, %fs
    movw    %ax, %gs
    movl    $0x90000, %esp
    movl    $0x10000, %eax
    jmp     *%eax

    .align 8
gdt:
    .quad 0x0000000000000000    /* null */
    .quad 0x00CF9A000000FFFF    /* 0x08: code, base 0, limit 4G, 32-bit */
    .quad 0x00CF92000000FFFF    /* 0x10: data, base 0, limit 4G */
gdtr:
    .word gdtr - gdt - 1
    .long gdt

    .org 510
    .word 0xAA55                /* the boot signature: this sector is alive */
