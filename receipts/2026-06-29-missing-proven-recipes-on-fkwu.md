# Receipt — 12 missing proven recipes ported, each proven on fkwu --src native (no Go flattener) (2026-06-29)

**The ask (Urs):** continue porting the missing form-cli recipes that are proven on the old repo, now on the new
repo, on the fkwu-native lane — no toolchain, no Go-flattener usage.

## Method (no Go flattener anywhere)

Swept every old-repo recipe with a self-proving `*-check` that uses only `--src`-supported grammar (no
strings/floats/host-io), MISSING in the new repo. Ran each through **`fkwu --src`** (the c-bootstrap source-runner)
— NOT flattened by Go, NOT loaded as a table. A `*-check` returns an all-1s witness (e.g. `11111` = 5 passing
assertions); a missing dep would make an assertion fail, so `11111` means the recipe is **self-contained and
correct** on the native lane.

## The 12 that proved (each -> 11111 on fkwu --src, in-place after porting)

```
learn/compare-summaries.fk          learn/self-descent.fk           learn/self-improving-thought.fk
learn/sema-curriculum-transfer.fk   learn/sema-mastery-readout.fk   learn/sema-mixed-exam.fk
learn/sema-reason-multistep.fk      learn/sema-skill-compose.fk     learn/teach-native-sema.fk
learn/teach-sema-math.fk            learn/teach-sema-pattern.fk     model/tokenizer.fk
```

The sema curriculum (transfer / mastery / mixed-exam / multistep / skill-compose), the teaching lane
(native-sema / sema-math / sema-pattern), self-improvement (self-descent / self-improving-thought),
compare-summaries, and the integer tokenizer — all now run on the c-bootstrapped kernel and pass their own checks.

## Stayed true to the lane

- **No toolchain / no Go flattener:** proof is `fkwu --src recipe.fk` — the kernel's own source-runner, on real
  Windows metal. The Go/Rust/TS walkers were not invoked; nothing was flattened by bin-go.
- **Native regression intact:** `native-vs-rented` and `surprise-receipt` still return `11111` after the sibling's
  data-driven source-walker refactor and these additions.

## Honest remainder

Of 30 missing check-bearing recipes swept, **18 returned empty** — they need the next `--src` surface (strings +
the string pool, or floats/host-io) or hit the tree-walk depth wall (`sensor-lane` looped). Those are NOT ported
(porting an unprovable recipe would be a claim without a receipt). They come home as that surface lands — the same
honest floor as the rest of form-cli's model/voice climb.

## Reproduce (any one)

```
( cat learn/sema-reason-multistep.fk; echo '(sema-reason-multistep-check)' ) > /tmp/v.fk
./fkwu.exe --src /tmp/v.fk     # -> 11111
```
