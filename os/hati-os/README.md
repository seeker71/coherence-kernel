# HatiOS — a small, honest, 100% native operating system

Bare-metal i386, freestanding C + GNU assembler. No libc, no host OS —
between this code and the CPU there is nothing but the firmware that reads
the first 512 bytes. *Hati* follows this repo's own naming (the hati-os lane
in `form/form-stdlib/hati-os-targets.fk`).

## What it is, exactly

- **boot.S** — the 512-byte boot sector: BIOS loads it at `0x7C00`; it reads
  the kernel (62 sectors, one honest track) to `0x10000`, opens the A20
  gate, installs a flat GDT, enters 32-bit protected mode, and jumps.
- **entry.S** — the kernel entry at `0x10000`, the interrupt stubs, and
  `switch_ctx` — the real context switch (eflags + callee-saved registers,
  per-task stacks).
- **kernel.c** — drivers (VGA text, 16550 serial — the witness channel);
  interrupts (IDT, remapped 8259 PICs, PIT at 100 Hz); a **preemptive
  round-robin scheduler** (the timer interrupt hands the CPU on; two
  `heart` tasks beat forever as living proof); a page-granular physical
  allocator (bitmap, 1 MB..8 MB); a ramfs (`name -> bytes`); and a serial
  shell: `help ps mem ls cat write echo uptime spin halt`.
- **linker.ld** — the image layout law.

Honest floors, named: single CPU, ring 0 only (no privilege split), no
paging/virtual memory, no disk filesystem (ramfs only), CHS boot read of one
track. Each is a floor with the next stone visible from it.

## Build (one-liners, no scripts — the same law as BOOTSTRAP.md)

```
gcc -m32 -ffreestanding -fno-pic -fno-stack-protector -Os -Wall -Wextra -c kernel.c -o kernel.o
gcc -m32 -c entry.S -o entry.o
ld -m elf_i386 -T linker.ld -o kernel.elf entry.o kernel.o
objcopy -O binary kernel.elf kernel.bin
gcc -m32 -c boot.S -o boot.o
ld -m elf_i386 -Ttext 0x7C00 --oformat binary -o boot.bin boot.o
cat boot.bin kernel.bin > hati.img
truncate -s 32256 hati.img
```

## Run and witness

```
qemu-system-i386 -drive file=hati.img,format=raw -nographic -no-reboot \
  -device isa-debug-exit,iobase=0xf4,iosize=0x04
```

Serial is the console; type at the `hati>` prompt; `halt` exits qemu cleanly
through the debug-exit device. `witness/first-boot.txt` is a committed
transcript of a full session: boot banner, preemption proof (`ps` run
counts and heart counters climbing between calls), ramfs write/read, timer
uptime, and the clean halt.

## Why the preemption proof is real

`heart-a` and `heart-b` never yield — they are infinite increment loops.
The only way their counters climb while the shell is reading a serial line
is the PIT interrupt seizing the CPU and `switch_ctx` handing it around.
`spin` busy-waits two seconds in the shell and shows the hearts kept
beating: the scheduler, not cooperation, owns the machine.
