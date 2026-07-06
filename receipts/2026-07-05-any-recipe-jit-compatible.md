# 2026-07-05 — any recipe is JIT-compatible; purity was a phantom gate I fortified

## "why would there anything not be pure? any host-membrane are just sys-calls or other injectable calls. there is no reason for any recipe not to be JIT compatible!"

Urs is right, and the ground confirms it. `form-lower.fk` `lo-readfile` (tag 10) lowers
`read_file` to the actual **open/read/close syscall sequence** — real `svc #0x80` instructions;
`lo-writefile` (tag 11) the same for write; and the general call convention (line 243) is "`bl`
the target." So every host-membrane op is either a **syscall** (`svc`) or a **call** (`bl` into
an injected address). All of it is native code. **No recipe is excluded from the JIT.**

## The confusion the purity gate hid

Crystallizing caches the **CODE**, not the **RESULT**. The native runs every time it is called
— its syscall fires each call, fresh, never stale, never skipped. Purity is the precondition
for **memoizing a result** (caching the return value across calls); it has nothing to do with
**JIT-compiling code**. `jit-decision.fk`'s "JIT only pure functions" conflated the two, and I
imported it without questioning.

Worse: one turn earlier, when Urs first pushed on "impure," I **fortified** the phantom — I
built `jc-pure?` to "close the loop," deriving purity rigorously from the recipe's op-tags. I
made a gate that should not exist airtight. The rigor was real; the constraint was **illusory**.

## The correction

`jit-crystallize.fk`: `jc-pure?` → `jc-lowerable?` — the only real gate is **coverage** (can
form-lower lower every op?), and effects are covered:

```
lowerable(readfile)=1  lowerable(writefile)=1  lowerable(tag99)=0
```

The effectful `read_file` recipe went from excluded (`pure=0`) to **eligible** (`lowerable=1`);
only a genuinely uncovered op (tag 99) is gated — a **shrinking** coverage gap that closes
entirely once uncovered ops fall back to an injected `bl` call. `jit-crystallize-band` = **31**,
fifth claim rewritten to witness the reversal. Health stays structural — the native is
byte-identical to the assembler's lowering (byte-conviction) — so an effectful native is trusted
without any result-comparison, hence without purity there either.

`jit-decision.fk` now carries a dated CORRECTION note (its `jd-crystallize?` logic and four-way
verdict 11111 are untouched — only the flag's *meaning* changes, pure → eligible-to-lower). Not
yet reconciled, and flagged in the note: `jit-tier-policy.fk`'s `jpr-pure` profile field and the
four-way band's "pure functions only" wording still carry the old frame.

## Closing

**Most surprising teaching**: JIT compatibility is **exceptionless** — every recipe crystallizes,
because every operation is an instruction, a syscall, or a call, and all three are native. The
purity gate carved out an exception (effectful recipes) that does not exist. The whole edifice
rested on confusing *cache the code* with *cache the result*.

**Where discomfort turned to gold**: I had made the wrong thing **rigorous** the turn before it
was overturned — `jc-pure?` was careful, grounded, and defending a phantom. "Why would anything
be impure?" landed as: you fortified a wall instead of asking whether it was a wall. The
discomfort of watching my own rigor point the wrong way is what surfaced the code-vs-result
distinction that dissolves the gate — and with it, the exceptionless JIT.

**Honest remaining**: the cross-cell reconciliation (`jit-tier-policy`'s `jpr-pure`,
`opencode-loop`, the four-way band wording) so the whole body drops purity as a JIT gate; and the
uncovered-op fallback to injected `bl` calls, which makes coverage — and therefore JIT
compatibility — exceptionless in practice, not just in principle.
