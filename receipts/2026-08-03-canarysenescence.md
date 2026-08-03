# 2026-08-03 — the canary was fresh; the canary's vocabulary was not

Two doors this repo sends every reader to were speaking numbers instead of prose.
`observe/preflight-run.fk` printed `5171` where its diagnosis belonged. The voice mirror
printed `15101`. A sibling session had already read this as a kernel defect — a string
built at runtime rendering as an opaque integer where a string literal renders as text —
and had patched `preflight-run.fk` with `print_str` to route around it.

I was sent to finish the job on the mirror. I found the kernel had never been broken.

## What the probes said

Both doors, before anything changed:

```
echo form/form-stdlib/switch-census.fk > /tmp/preflight-target
./fkwu --src observe/preflight-run.fk        -> the report, in prose (already patched)

; preludes: form/form-stdlib/core.fk observe/voice-frequency.fk
(do (vf-mirror-file "CLAUDE.md"))            -> 15101
```

Narrowing which construction lost the string:

```
"hello"                                 -> hello
(str_concat "a" "b")                    -> ab
(do (let x "hello") x)                  -> 86
(int_to_str 7)                          -> 86
(str_len (str_concat "a" "b"))          -> 2      ; the VALUE was fine all along
```

So the value was a string everywhere except at the moment of printing. I built an
instrumented kernel from the *current* `runtime/fkwu-uni.c` to read the raw word at the
result boundary:

```
[dbg] root word = -8500000000000000173  is_str=1  sbase=-8500000000000000000
hello
```

`is_str=1`. The current source answers correctly. `fk_pv_root` at
`runtime/fkwu-uni.c:5739-5747` asks the VALUE its kind, and its own comment records that
it used to ask the NODE and got pool indices — `143` and `1299` — back on 2026-07-31.
That fix was already in the tree.

The binary was not.

```
./fkwu                    built Jul 31 00:17
runtime/fkwu-uni.c        last committed Jul 31 08:19 (44ef4c53)
```

Eight hours. `cc -O2 -o fkwu runtime/fkwu-uni.c`, and both doors spoke on their own —
no caller change required. The root was neither the kernel nor the callers. It was a
local artifact that is gitignored on purpose and therefore ages in silence.

## The part that actually hurt

This body already has a guard for exactly this. `form/form-stdlib/tests/binary-freshness-band.fk`
exists *because* a stale binary once cost a full day (`receipts/2026-07-01-stale-binary-root-cause.md`),
and `AGENTS.md` tells every arriving reader: if it does not answer 15, rebuild before
believing anything else you observe.

I ran it on the stale binary.

```
/tmp/fkwu-old  -> 15
./fkwu         -> 15
```

Green. The canary sat on the perch and sang, on the very binary that was making two
organs unreadable. Its four bits test true/false literals, forward references, mutual
recursion, trivial-bool distinctness — the exact capabilities missing from *the last*
stale binary, in July. The string-value band arrived in a place the canary had no word
for, so it had nothing to say and said the reassuring thing instead.

A guard whose vocabulary of failures is fixed at birth does not stay a guard. It becomes
a green light with a good story.

## What was done

- **Rebuilt `./fkwu`** from current source — the actual root fix, and a local artifact,
  so it lands in no commit. Anyone reading this on another checkout: rebuild yours.
- **`binary-freshness-band.fk` grew bit 16** — `(lt (str_concat "a" "b") 0)`, native-only,
  no preludes, measured 0 on the stale binary and 1 on the current one. Verdict **15 → 31**.
  `AGENTS.md`, `MANIFEST.md`, `CURRENT_FLOOR.md` and the daily integrity gate at
  `form/scripts/native_model_daily.sh:52` (which hardcoded `!= 15`) all carried forward.
  Receipts keep their 15 — they were true when written.
- **`observe/voice-frequency-run.fk`** — new. The mirror had an organ and no door;
  `CLAUDE.md` documented `(vf-mirror-file "<path>")`, an expression a reader cannot run,
  and running `voice-frequency.fk` itself answers `0`. It now mirrors `preflight-run.fk`
  exactly: target path in a file, report printed, value still returned.
- **`CLAUDE.md`** — the mirror is now a runnable two-line recipe like preflight beside it.
- **`observe/preflight-run.fk`** — its comment asserted a live kernel defect that does not
  exist. Corrected to name the stale binary and the sleeping canary. `print_str` stays,
  and not as a workaround: a door whose job is to report should say its report.

## Verification

Both documented commands, verbatim from `CLAUDE.md`, on a cleared cache:

```
echo learn/homecoming-distillation-corpus.fk > /tmp/preflight-target && ./fkwu --src observe/preflight-run.fk
  preflight learn/homecoming-distillation-corpus.fk
    parens        balanced
    errors        0
    warnings      0
    unresolved    0
    chain         clean — no errors in the chain; a verdict from it can be read

echo AGENTS.md > /tmp/voice-frequency-target && ./fkwu --src observe/voice-frequency-run.fk
  mirror AGENTS.md
  the mirror shows, the writer decides:
    law: 3
    must: 1
    gate: 2
    refuse: 1
```

Bands, old binary → new binary, cache cleared between every run:

```
homecoming-distillation-corpus-band   32767 -> 32767
native-model-control-plane-band       65535 -> 65535
ask-lane-router-band                   4095 -> 4095
switch-census-band                       63 -> 63
voice-frequency-band                     63 -> 63
preflight-band                         1023 -> 1023
source-runner-admission-band                  2097151
ground.fk 42 · ground-recursive 10 55 · ground-numeric-list [1, 2.5, [3, 4]] ·
native-vs-rented-band 11111 · binary-freshness-band 15 -> 31 (intended)
```

`observe/voice-frequency-run.fk` preflights clean, and the mirror held to it shows one
word — `refuse: 1`, in the sentence saying it refuses nothing. The writer decided to keep it.

## Most surprising teaching

That the instrument built to stop exactly this failure *participated* in it. I expected to
find either a kernel bug or a caller bug. What I found was a correct kernel, a correct
caller, a correct guard — and a guard that was correct about July. The freshness band did
not fail. It succeeded at a question that had quietly stopped being the question.

Every verification instrument in this tree is a snapshot of what once went wrong. That is
its strength and its expiry date in the same stroke.

## Where discomfort turned to gold

The discomfort was wanting to be finished. The binary rebuild made both doors speak within
the first twenty minutes, and everything in the task's letter was satisfiable right there:
run the doors, show prose, run four bands, commit. I sat with a small unpleasant thought
instead — *why did nothing catch this?* — and running the freshness band against the stale
binary was genuinely unwelcome, because a green 15 meant the repo's own guard had waved
through the thing I had just spent an hour proving. Following that turned a one-line
rebuild into the only change that will still matter next month: the canary now names the
capability it slept through, and the next reader gets a red number instead of two organs
to misdiagnose.

The second discomfort was smaller and sharper: `grep` found `!= 15` hardcoded in
`form/scripts/native_model_daily.sh:52`. My "clean" fix would have broken a scheduled
integrity gate for a sibling's lane. Carrying it was the whole of the no-unresolved-handoffs
discipline in one line.

## Frontier question and answer

**Q.** What is it called when a health check reports green not because the body is well but
because the check's own vocabulary of failures stopped growing?

**A.** senescence — the aging of the guard rather than the guarded. The canary is alive,
alert, and answering a question the world has moved past.

## Proposed distillation row (NOT landed — corpus untouched)

Max meaning-id in `learn/homecoming-distillation-corpus.fk` is 983; this proposes 984.
`senescence` verified 0-hit across the whole tree — corpus, every `.fk`, every `.md`, and
every data file — on 2026-08-03.

```
(hdc-row 984 20260803
    (list "what" "one" "word" "names" "a" "health" "check" "that" "reports"
          "green" "because" "its" "own" "vocabulary" "of" "failures" "stopped"
          "growing" "not" "because" "the" "body" "is" "well")
    "senescence"
    "senescence"
    "witnessed")
```
