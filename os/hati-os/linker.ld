/* linker.ld — HatiOS kernel image layout.
 * boot.S jumps to 0x10000; .text.entry must be the first byte there. */
OUTPUT_FORMAT(elf32-i386)
ENTRY(kentry)
SECTIONS
{
    . = 0x10000;
    .text : { *(.text.entry) *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss  : { *(COMMON) *(.bss*) }
}
