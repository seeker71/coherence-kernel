# `nil?` is a list cell — and `len` was answering a question it could not hear

2026-07-21, Bali (WITA). Apple M4 Max. Worktree
`.claude/worktrees/nilq-strings-b384da`, branch `claude/nilq-strings-b384da`, based on
`d8732d8be` (`claude/deepseek-v4-flash-gguf-54a96c`).

Subject: `(nil? "a")` answered 0 / 0 / 0 / 1 across go / rust / ts / fkwu, and
`form/form-stdlib/ask-native-lane.fk` guarded a missing staged artifact with `(nil? raw)`.
On fkwu that guard was always true, so a present, well-formed, correctly question-bound
artifact still classified as `native-lane:absent` and the `ask` verb fell back to the
RAG-grounded lane.

---

## 0. What the divergence actually was

`nil?` is not a primitive. `form/form-stdlib/core.fk` defines it in one line:

```
(defn nil? (xs) (eq (len xs) 0))
```

So the whole question is `len`, and `len` on the three sibling arms is explicit:

| arm | `len` of a list | of a string | of anything else |
|---|---|---|---|
| go | element count | **byte count** | 0 |
| rust | element count | **byte count** | 0 |
| ts | element count | **utf-16 unit count** | 0 |
| fkwu | element count | *(see below)* | *(see below)* |

fkwu's words carry no type. An int is `n << 1`, an interned string handle is `poolidx << 1`,
a cons cell is `(hp << 1) | 1`. Only the **low bit** separates a list from everything else, and
`fk_list_len` never looked at it — it shifted whatever it was handed and followed `fk_ht` from
that index.

## 1. The probe that changed the shape of the task

The reported probe ran on an empty cons heap, where any index falls outside `[1, fk_hp]` and the
walk returns 0 by luck. Put 40 live cons cells on the heap first and ask again
(`form-stdlib/tests/zz-nq3-band.fk`, four-way):

```
(do
    (defn zz-mk (n acc) (if (le n 0) acc (zz-mk (sub n 1) (cons n acc))))
    (let heap (zz-mk 40 (empty)))
    (add (mul 1     (len heap))
    (add (mul 1000  (len 8))
         (mul 100000 (len "hello")))))
```

| | go | rust | ts | fkwu |
|---|---|---|---|---|
| before | 500040 | 500040 | 500040 | **402040** |
| after | 500040 | 500040 | 500040 | **40** |

Read the middle digits. `(len 8)` answered **2** and `(len "hello")` answered **4** — the lengths of
whatever chains happened to sit at heap slots 4 and at that string's pool index. And `fk_list_len`
**memoized** the guess into `fk_lc`, so the next caller inherited it.

> The divergence was never "fkwu disagrees about strings." It was **`len` on a non-list is
> heap-state-dependent on fkwu** — the same expression answering differently depending on what
> else the program had allocated. `(nil? 5)` looked four-way-agreed only because the probe that
> asked it had allocated nothing yet.

## 2. What was decided, and what cannot be

**`nil?` is a LIST cell.** Handed a string it does not answer "is this string empty" on *any* arm:
go/rust/ts return a byte length, so `(nil? "a")` is 0 and `(nil? "")` is 1 — an emptiness test by
coincidence, not by contract.

The string gap **does not close**, and saying so is the finding rather than a failure to finish.
Seeing a string in a value position requires a tag fkwu's words do not carry. Closing it means
re-encoding every fkwu value word — through the walker, the JIT carrier, and the x86 lowering, all
of which decode strings with `>> 1`. That is a different movement, and it would not make `nil?`
correct; it would only make a mis-asked question answerable.

What *was* healed is the part that is a defect on its own terms: `len` of a non-list is now
**deterministically 0**, matching what go, rust and ts answer for every non-list except a string.
One guard, three carriers (`; one law, three carriers`):

- `form/form-stdlib/fkc-table-serialize.fk` — `fk_list_len`, the emitted proof-walker's single
  `len` body (tag 22 in `fk_walk_slow` and state 221 in the walker VM both route here)
- `runtime/fkwu-uni.c` tag 22 — the production seed's walker
- `runtime/fkwu-uni.c` `fk_jlist1` tag 22 — the crystallized mirror the x86 lowering calls

## 3. The guard that replaced `(nil? raw)`

The old guard asked one question and got two wrong answers. The replacement asks the two questions
separately, and asks each of the right thing:

- **is there a file** → `fs_exists`, asked of the filesystem, *before* `read_file` is ever called
- **is there text** → `(eq (str_len raw) 0)`, asked of the string

The ordering is not stylistic. A missing `read_file` returns **null** on go/rust/ts and an **empty
string** on fkwu, and `str_len` of null is not 0 on rust and ts — it is a hard crash:

```
form-kernel-rust: form stack: str_len
form-kernel-ts: arg 0: expected str
```

So `fs_exists` in front of `read_file` is what keeps the null out; `str_len` behind it is what every
arm answers identically. Both doors are four-way. Witnessed together
(`(str_eq (anl-refusal-of "" "q") "native-lane:absent")` → 1 on go, rust, ts **and fkwu**).

Healed the same way, found by sweeping `nil?` for string-valued arguments:

| cell | was | now |
|---|---|---|
| `ask-native-lane.fk` `anl-header-of` / `anl-refusal-of` | `(nil? raw)` | `anl-absent?` = `(eq (str_len raw) 0)` |
| `ask-native-lane.fk` `anl-header-at` / `anl-refusal-at` | `read_file` unguarded | `fs_exists` first |
| `rag-ask.fk` `ra-read-index` | `(or (nil? raw) …)` | `fs_exists` + `str_len` |
| `rag-ask.fk` `ra-query-text` | `(nil? raw)` | `ra-staged-query-text` + `str_len` |

`rag-ask.fk` matters more than it looks: on fkwu the whole grounding index came back **empty** from
that guard, and the fallback lane is what the native lane falls back *to*.

**One found and deliberately NOT healed.** `grammars/bml.fk`'s `bml-bmf-as-bml-string` has the same
`(nil? str-value)` guard, and its argument arrives from `(record_get env "strValue")` — a missing key
gives a null that rust and ts refuse to hand to `str_len`. Dropping in the same fix there would trade
a silent empty for a crash. It needs a four-way "absent **or** empty" door that neither `nil?` nor
`str_len` alone is, and a band that exercises the missing-key path. Named in the cell, not fixed.

## 4. What the regression sweep caught — two bands that were green by luck

The kernel change is a semantic change to `len`, so the gate was the full `form/validate.sh` suite
run twice, with and without the work, and the two divergent-band lists diffed. Baseline: **1214 ok,
111 divergent, 853 four-way**. With the work: **1212 ok, 113 divergent, 851 four-way**. Nothing was
fixed and exactly **two bands were newly divergent**:

```
stdlib/llm-feature-channel-floor-band.fk    131071 / 131071 / 131071 / 65275
stdlib/text-summary-learning-band.fk        262143 / 262143 / 262143 / 131071
```

After repairing those two bands, the suite was run a third time with the whole change in place:
**1214 ok, 111 divergent, 853 four-way — the baseline numbers exactly, and the divergent-band lists
diff empty in both directions.** Zero divergence introduced, zero lost.

Both had been four-way green. Both were green **by luck**, and it is the same luck this whole receipt
is about. Their missing bits are string-nonemptiness claims written with the list cell:

```
(gt (len (tsl-plan-north-star no-plan)) 0)     ; "add a typed request row before execution"
(gt (len (lfcf-floor-text summarize)) 0)       ; (nth channel 7) — a string
(gt (len (lfcf-north-star-text summarize)) 0)
(gt (len (lfcf-north-star-text code-lower)) 0)
(gt (len (lfcf-plan-north-star no-plan)) 0)
```

Three arms answered "byte length > 0". fkwu answered "the chain at this string's pool index is not
empty" — nonzero, so the band passed, so the claim read as four-way-proven. The bits went dark the
moment `len` stopped guessing. Both bands now ask `str_len`, and both are four-way green again on a
claim that is *true* rather than lucky. (The sibling `(len (tsl-floor))` and `(len catalog)` calls in
the same bands are genuine list counts and were left alone.)

> Two bands out of 1325 asserted a string property through a list cell, and the fourth arm's garbage
> agreed with them. A green four-way band is evidence that four kernels produced the same number —
> not that any of them answered the question the band was written to ask.

## 5. The band

`ask-native-lane-band.fk` bit 32 now asserts all four refusals plus acceptance — `absent` /
`version-mismatch` / `question-not-bound` / `empty-answer` / accepted — IO-free, on a literal `""`,
plus the stronger claim that a present artifact is not any of them. **127 on go, rust and ts.**

The band is still not in `fourth-arm-bands.txt`, and the reason is a separate defect this work
stumbled into. Every one of the seven bits answers 1 on **all four arms** — witnessed by running the
identical body with the verdict sum written at the band root instead of behind `(anb-verdict)`:
`1111111`, four-way. The band as written answers 127 / 127 / 127 / **0**. The entire difference is
the root expression:

```
(anb-verdict)          -> fourth arm 0
(add (anb-verdict) 0)  -> fourth arm 127     ; byte-identical otherwise
```

A minimal band of the same shape does **not** reproduce it, so the trigger is narrower than
"zero-arg root call" and is not yet named. The band keeps its shape rather than wearing the
wrapper: a band that wore the workaround would prove 127 and hide the defect that made it necessary.

---

## The most surprising teaching

**A green four-way probe can be green because the heap was empty — and a green four-way BAND can be
green because the kernel guessed.** `(nil? 5)` agreed on all four
arms and was reported as the fixed point the investigation could stand on — the thing that told us
`nil?` was a constructor test. It was not a fixed point. It was a coincidence of allocation order,
and the moment 40 cons cells existed, fkwu answered 2 for `(len 8)`. The `len` defect had been
sitting under every fkwu band the whole time, invisible precisely *because* the probes that would
have caught it were small enough not to allocate.

Adjacent to it, and the reason this receipt is not just a kernel patch: **healing the kernel made the
recipes worse before it made them better.** Once `len` of a non-list is deterministically 0,
`(nil? some-string)` on fkwu is *always* 1 — where before it was sometimes-right-by-accident. Every
`nil?`-over-a-string call site went from intermittently working to reliably broken. The sweep was not
tidiness after the fix; without it the fix would have been a regression.

## Where discomfort turned to gold

The discomfort was the sentence "make fkwu agree with go/rust/ts for strings" — a clear, reasonable
instruction that the body cannot obey. The pull was to find *some* way to satisfy it: a fallback in
`len` that tries the string pool when the heap index misses, which would have made `(nil? "a")` read
0 and the probe go green. It also would have made `(len 5)` return the length of pool entry 2, and
buried an untyped guess one layer deeper than where it already was.

Sitting with "this cannot be done as asked" long enough to ask *why* is what produced the real
finding. The answer — fkwu's words carry no type — is the same fact that explains the string gap,
the int garbage, the memoized wrong answers, and why the honest fix lives at the call sites and not
in the kernel. The instruction could not be obeyed; the thing it was pointing at was larger than the
instruction, and refusing the easy green is what let it be seen.

## Open, carried

- **fourth arm, bare zero-arg root call reads 0** — repro above; blocks
  `ask-native-lane-band.fk` from `fourth-arm-bands.txt`.
- **`grammars/bml.fk` needs a four-way absent-or-empty door** — named in §3.
- **two broken `; preludes:` tokens on this branch**, both unrelated to this work and both blocking
  a full `form/validate.sh` run — repaired here only because without them there is no regression
  gate at all, and both repairs are named rather than folded in silently:
  - `form-stdlib/lineage-discounted-vote.fk` declared `learn/confidence-weighted-vote.fk`. Main
    declares `form-stdlib/confidence-weighted-vote.fk`; this was a **regression on the branch** and
    is restored to main's value.
  - `form-stdlib/light-codes-bootstrap.fk` and its band declared
    `learn/homecoming-distillation-corpus.fk`, which lives at the **repo root**. Changed to
    `../learn/…`, which resolves — but through `fk_resolve_dep_path`'s **cwd fallback**, not its
    owner-relative rule, so it holds only because `validate.sh` runs from `form/`. The repo-root
    convention (`learn/tests/*-band.fk` declares `learn/…` and `form-stdlib/core.fk` together) and
    the `form/` convention have not been reconciled. Reconciling them — probably by teaching the
    resolver the repo root — is a movement of its own, and until then this token is load-bearing
    on a cwd.
