# HatiOS — the i386 guest with Form-emitted native leaves

This guest boots through BIOS into 32-bit protected mode under QEMU. Its current
kernel uses freestanding C and assembly; Form emits the machine code used for
string equality and length throughout its shell and ramfs. The guest uses no
libc or guest host OS. QEMU itself runs on the development host.

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
- **linker.ld** — the image layout.
- **guest-native.bml** — Form owns two instruction programs, resolves their
  relative branches and emits actual i386 machine bytes. The assembler packages
  those bytes into a linkable object; there is no generated C body. The same
  cell validates and packs the 512-byte boot sector plus kernel into the disk
  image, refusing a kernel larger than the boot reader can load.

Honest floors, named: single CPU, ring 0 only (no privilege split), no
paging/virtual memory, no disk filesystem (ramfs only), CHS boot read of one
track. Each is a floor with the next stone visible from it.

## Build

```
form-run sh form/scripts/build_hati_os.sh CLANG LLD LLVM_OBJCOPY OUTPUT_DIRECTORY
```

Supply explicit executable paths. On the witnessed macOS host, clang and
llvm-objcopy live in `/opt/homebrew/opt/llvm/bin/`; the installed Rust toolchain's
`rust-lld` provides the ELF linker. It is used only as a linker. No Rust program
or runtime participates in the guest. The script invokes Form for emission and
image packing and carries compiler/linker OS operations.

The C compiler receives `--target=i386-none-elf -march=i386 -mno-sse -mno-sse2
-mno-mmx`. The guest has no SIMD initialization or SIMD context save; generated
instructions must match the saved machine state.
LLD's boot-sector link uses image base zero and address `0x7C00`.

`guest-native-band.fk` checks forward/backward relocation, missing and distant
branch refusal, boot signature, image size and oversized-kernel refusal; its
observed verdict is `255`. Emission is a small Form instruction lowering path,
not a general Form source compiler or a guest-resident JIT.

## Guest entry and value ABI: HGI1

Boot entry is address `0x10000` in protected mode, flat code selector `0x08`,
flat data selector `0x10`, interrupts disabled, and stack at `0x90000`.
`entry.S` calls `kmain`. This is a single CPU, ring-0 address space.

The Form leaves use i386 cdecl: arguments are 32-bit words at `[esp+4]`, then
`[esp+8]`; return values occupy EAX; the caller removes arguments. EBX, ESI,
EDI, EBP and the stack pointer are preserved. EAX, ECX, EDX and arithmetic flags
may change. These are raw guest values, distinct from fkwu's hosted tagged ABI.

`hati_guest_streq(a,b)` returns `0` or `1`; `hati_guest_slen(a)` returns the
number of bytes before NUL. Pointer arguments name readable NUL-terminated
guest memory owned by the caller. Neither leaf allocates, yields or touches a
device. Invalid/unmapped pointers and strings without a reachable terminator
are outside this ABI. The timer interrupt preserves their caller-saved registers,
so the leaves may be preempted. Current kernel callers pass bounded shell,
ramfs or static strings.

## Run and witness

```
qemu-system-i386 -drive file=hati.img,format=raw -nographic -no-reboot \
  -device isa-debug-exit,iobase=0xf4,iosize=0x04
```

Serial is the console; type at the `hati>` prompt; `halt` writes `0x31` through
the debug-exit device. QEMU therefore returns `(0x31 << 1) | 1 = 99`; that
particular exit is the expected guest halt. The current transcript is
`witness/current-serial.txt`, with UART CRLF normalized to LF; its recorded QEMU status is
`witness/current-exit.txt`. It exercises exact, prefix and suffix
comparison, nonempty and empty lengths, ramfs readback, and preemption while the
Form leaves execute. Shell scheduling counts rise from `590` to `790`; both
heart counters increased. The host's native reader observed `1023` over the raw
serial content, exit file and exact leaf bytes in that build's `kernel.bin`:

```sh
printf '%s\n' OUTPUT_DIRECTORY | form-run ./fkwu os/hati-os/witness-run.fk
```

To retain fresh artifacts, use QEMU's stdio character device with
`-chardev stdio,id=uart,logfile=OUTPUT_DIRECTORY/serial.log -serial chardev:uart`
and record its actual exit code in `OUTPUT_DIRECTORY/exit.txt`. Type the
transcript's commands after the prompt appears. The witness reader checks an
observed interactive run; it does not automate serial input or claim a complete
OS regression suite.

## Why the preemption proof is real

`heart-a` and `heart-b` never yield — they are infinite increment loops.
The only way their counters climb while the shell is reading a serial line
is the PIT interrupt seizing the CPU and `switch_ctx` handing it around.
`spin` busy-waits two seconds in the shell and shows the hearts kept
beating: the scheduler, not cooperation, owns the machine.
