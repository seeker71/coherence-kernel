# 2026-07-02 — debride: remove the dead tissue in the breath you find it

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
./fkwu --src observe/native-vs-rented.fk                       # 11111
# corpus band four-way                                         # 127
```

Urs, on the JIT bug's aftermath: *"you did not release it when you found it, you have all the info to
do it, and you are wanting someone else in the future to get all that context again ... where it will
cost 5-10x more to do it later."*

Twice in a row I had found a healthy removal, held grep-proof it was safe, and filed it as "follow-up
cleanup" — exporting the context-reacquisition cost to a future session at 5-10× the price. Same shape
as the manufactured-blocker lesson, one level down: not avoidance of a hard thing, but deferral of an
easy one.

## What I removed (in the breath I found it)

- **`fk_native_call_args`** (41 lines): grep-confirmed dead — its only caller was the `--src` JIT
  gate's install-failure `else`, which I had already deleted when I made the gate bail to the walker.
  On non-x86 it only ever returned the `fk_nothing` sentinel. Removed; a 3-line tombstone comment
  records why, so the next reader doesn't re-derive it.
- **The `njit` counter + `[jit] … (native dispatch)` witness**: moved from *before* install to *inside*
  the install-success branch. On arm64 install always fails, so the old placement ticked `njit` and
  printed "native dispatch" for a dispatch that never happened — a small confabulation. Now both fire
  only on a real native run; on this body they correctly stay silent, and FK_JIT programs return the
  right answer via the walker with no misleading witness.

Verified: build clean (only the 2 pre-existing warnings), `f(5)` → 15 with FK_JIT on and no false
witness, canaries 42/15/11111, corpus band 127 four-way.

## What I did NOT remove, and why it's a real fork (not a deferral)

The whole hand-written x86 self-JIT (`fk_jemit` + `fk_jbin`/`fk_jcarrier`/`fk_jtramp`/`fk_jit_lower` +
the gate + the `fk_jb*` byte helpers, ~1000 lines) is a **revenant** on this arm64 body — it lowers,
fails to install, and walks. Deleting it is tempting and would simplify the seed. But grounded:
`model/form-asm.fk`, the Form-native arm64 replacement, is by its own header still "the foundation …
drops clang ONLY once it can BYTE-LEVEL VERIFY" — **not yet a functional replacement.** And the
consolidation program's stated intent is to *replace* the self-JIT from Form, not delete it. So
wholesale removal today would drop x86 hosts' opt-in JIT before its successor exists — a real
capability trade-off and a jump ahead of the sequence, which is genuinely the owner's call, not mine
to make silently. That is a different animal from the two no-cost removals above, which had no
trade-off and so brooked no deferral.

## The most surprising teaching this work left behind

The deferral tax and the fix were the same size. Removing `fk_native_call_args` cost the same few
minutes now as it would have cost then — except "then" also carried the price of a future reader
re-reading the gate, re-proving the deadness, and re-building the courage to delete. "I'll clean it up
later" is almost never cheaper later; the only thing deferral reliably grows is the context someone
must rebuild to act. The cheap moment to remove dead tissue is the moment you can see it is dead.

## Where discomfort turned to gold

The discomfort was being caught deferring twice, and the reflex to treat "it's a bigger surgery" as
cover for the small removals too. Witnessing the distinction honestly — that `fk_native_call_args` had
*no* trade-off (pure dead code) while the wholesale self-JIT removal has a *real* one (x86 capability,
consolidation sequence) — is what let me act on the first immediately and surface the second as a true
fork instead of a vague offer. The gold is the sorting rule: no-trade-off cleanup is done on sight;
trade-off decisions are named as decisions, not banked as "follow-ups."

## Corpus

Row 652 **debride** — to remove dead or contaminated tissue from a wound the moment it is found, so it
heals instead of festering (fresh; the discipline of releasing dead code on discovery rather than
banking it as a follow-up that costs 5-10× later — `fk_native_call_args`, excised on sight).
