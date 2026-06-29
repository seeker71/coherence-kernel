# Receipt — the pixel walk is a Form recipe that LOWERS, not C. Witnessed, and it taught much (2026-06-29)

**The correction.** I had claimed the 307k-pixel luminance walk "can't be tree-walked, so its honest home is a C
loop." Urs: *it can certainly be written in Form and lowered with native JIT, not C — show it with a receipt.*
Right. Tested. The walk is a **Form recipe**; what it needs is **lowering**, not C. Here is the evidence and what
it taught.

## Witnessed native on Windows — the accumulate loop IS Form

A pure accumulate recipe — the exact shape of a pixel walk — hand-flattened and run on `fkwu` (Windows), no C:

```
recipe:  f(n) = (if (le n 0) 0 (add n (f (n-1))))      ; add/sub/le/if/self-call — the JIT family
  f(5)      = 15           f(50)     = 1275
  f(1000)   = 500500       f(10000)  = 50005000        ; correct at every depth it reaches
```

The loop runs as Form. The logic is right at every scale. So "the walk must be C" is simply false.

## What actually limits the tree-walk — and why it proves *lowering*, not C

The same recipe, two stacks:

```
  1 MB  stack :  f(60)   = 1830   OK    f(70)    overflow      <- wall ~60
  512 MB stack:  f(10000)= 50005000 OK  f(50000) overflow      <- wall ~10k-50k
```

The wall moves with the **stack size**, not with Form. The cause: `fk_walk` (the tree-walker) recurses on the C
call stack, and it is one giant function with a large frame (~17 KB per level). So interpreter recursion depth is
**stack-bound**: a 307,200-pixel walk would need ~5 GB of stack tree-walked — **infeasible**. This is not a
"Form can't" fact and not a "needs C" fact. It is an **interpreter-recursion vs lowered-loop** fact:

- **Tree-walked** (recursion): depth = stack / frame-size. Bounded. Wrong tool for a 307k loop.
- **Lowered** (`form-asm` / self-JIT): the tail-recursion becomes a **counted native loop** — induction var + accumulator, **O(1) stack**, unbounded length. The right tool, and it is *the same recipe*.

## The recipe, and the lane that lowers it

The real walk is now a Form cell: [`model/frame-luma.fk`](../model/frame-luma.fk) — `luma-sum` (tail-recursive
accumulate over `str_byte_at`) + `luma-mean` + `frame-luma`. It is the lowering **target**, not C. The lane is
already in this repo:

- `model/form-asm-x64.fk` — Form → x64 bytes (a real loop, no recursion). The Windows-relevant lowerer.
- `model/form-asm.fk` / `model/form-asm-matvec.fk` — the matvec already lowers Form→asm four-way (the harder case).
- The self-JIT crystallize — **proven ~13× on this exact add/sub/le/if tag family**
  (`receipts/2026-06-29-jit-live-crystallize.md`); fib goes native bit-identical at a measured heat threshold.

"The recipe that proves four-way is the recipe that lowers to native" — one engine. `fk_frame_read`'s C math is a
**scaffold standing in for the un-wired lowering on Windows**, exactly as named in the source marker — not a
necessity, and now with its Form replacement written.

## What this taught

1. My "must be C" was a category error: I conflated *interpreter can't recurse that deep* with *Form can't*. The
   depth limit is the C stack under `fk_walk`, shown directly (it scales with stack size).
2. The tree-walker is the wrong executor for any long loop — even modest ones (~60 deep at 1 MB). That is an
   argument **for lowering as the default for hot loops**, not for hand-C.
3. `fk_walk`'s huge single-function frame is itself a cost worth noting: it makes recursion expensive. A lowered
   loop sidesteps the whole function.

## Honest floor / the remaining rung

- **Witnessed:** the accumulate loop runs as Form on Windows; the limit is stack-bound (proven by the 1 MB vs
  512 MB sweep); the pixel-walk recipe is written (`frame-luma.fk`).
- **Pending (named, not faked):** running `frame-luma.fk` **lowered-native** on Windows needs two known rungs —
  (1) the source-runner **seed** to run `form-asm-x64` over the recipe (the platform-neutral seed gap,
  `windows-flatten-reground`), and (2) the emitted x64 bytes loaded+called (`form-pe-coff` → a loadable function),
  plus the one-line `read_file` `O_BINARY` carrier fix so Form reads the binary frame. None is a "needs C" gap;
  each is lane plumbing. When they land, `frame-luma.fk` lowers, runs native over the real frame, and
  `fk_frame_read`'s math is deleted.

## Reproduce

```
gcc -O2                       -o fkwu.exe   runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32
gcc -O2 -Wl,--stack,536870912 -o fkwu-big.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32
printf '1 0 11 6 1 2 3 5 4 5 0 1 0 0 0 3 6 7 0 2 0 0 0 1 0 0 0 2 0 0 0 7 8 0 0 4 9 10 0 2 0 0 0 1 1 0 0 0\n' > sum.flat
./fkwu.exe    sum.flat 60      # 1830   ; 70 -> overflow  (1 MB wall)
./fkwu-big.exe sum.flat 10000  # 50005000; 50000 -> overflow (512 MB wall) — STACK-bound, not Form-bound
```
