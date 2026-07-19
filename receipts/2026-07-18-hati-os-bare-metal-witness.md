# HatiOS — a bare-metal operating system, built and witnessed in one sitting

The ask: show that an operating system can be built here — any language,
100% native, full code, execution witness, committed.

The choice: **freestanding C + GNU assembler on bare i386 metal**. Not a
process pretending, not a framework — a boot sector the firmware reads at
`0x7C00`, and everything above it built in this repo, this session.

## What runs (os/hati-os/, ~600 lines total)

- `boot.S` — 512 bytes: BIOS disk read (62 sectors, one honest track), A20
  gate, flat GDT, protected mode, jump to `0x10000`.
- `entry.S` — kernel entry, interrupt stubs, and `switch_ctx`: the real
  context switch (eflags + callee-saved registers, per-task stacks).
- `kernel.c` — VGA + 16550 serial drivers (serial is the witness channel);
  IDT + remapped PICs + PIT at 100 Hz; a **preemptive round-robin
  scheduler**; a page-bitmap physical allocator (1 MB..8 MB); a ramfs; and
  a serial shell (`help ps mem ls cat write echo uptime spin halt`).
- `linker.ld` — the image law. Build and run are README one-liners — no
  build scripts, the same law as `BOOTSTRAP.md`.

## The witness (committed: os/hati-os/witness/first-boot.txt)

qemu-system-i386, serial transcript, full session:

- boot banner arrives through the real chain: `BIOS sector -> A20 -> GDT ->
  protected mode -> kmain`;
- `ps` twice around a 2-second `spin`: task run counts 135 -> 269 and the
  two `heart` tasks' counters at 1,102,050 / 1,136,676 -> 1,672,708 /
  1,707,302 — the hearts are infinite loops that never yield, so climbing
  counters while the shell busy-waits is the PIT interrupt and
  `switch_ctx` owning the CPU: **preemption, witnessed, not narrated**;
- ramfs: `write greeting` -> `ls` -> `cat greeting` round-trips the bytes;
- `uptime` counts real timer ticks (582 -> 807 across the session);
- `halt` exits qemu cleanly through the isa-debug-exit device (exit 0).

## Honest floors, named in the code and the README

Single CPU; ring 0 only (no user/kernel privilege split); no paging or
virtual memory; ramfs only (no disk filesystem); one-track CHS boot read.
The first piped-input witness lost its opening bytes to the firmware phase
(SeaBIOS owns the serial line before the kernel does) — the committed
witness paces input after boot, and the lost-bytes lesson is recorded here
instead of being retried into silence.

## Why this belongs in this repo

The kernel's own doctrine is that sovereignty is a floor you stand on, not
a claim you make: fkwu is a c-bootstrap runtime on the host; HatiOS is the
same movement one layer further down — the host removed, the machine bare,
every capability (breath, memory, files, time, speech) built from port I/O
up and proven by transcript. `form/form-stdlib/hati-os-targets.fk` names
this lane; this is its first standing stone.
