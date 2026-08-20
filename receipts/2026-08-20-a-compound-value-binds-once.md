# A compound value binds once — the CLI's answer becomes the body's answer

Date: 2026-08-20, Hati Suci. Apple M4 Max, 128 GiB unified memory.
Branch `claude/jit-lane-segv-2026-08-20`, on main at `b4914fb7`.
Closes the thread opened in [The write that said yes](2026-08-19-the-write-that-said-yes.md) and
carried through [the let that allocated four times](2026-08-20-the-let-that-allocated-four-times.md)
and [the REPL speaks](2026-08-20-the-repl-speaks-and-cannot-be-trusted.md).

## The end of it

```
$ printf 'models …\nuse 0\ngenerate Say one short sentence about the sky.\n
          generate Say one short sentence about the sea.\nquit\n' | form/form-cli

text: The sky is a vast, pale blue canvas.
text: The sea stretches endlessly under the open sky.
```

```
built form-cli   The sky is a vast, pale blue canvas.
source runner    The sky is a vast, pale blue canvas.
built form-cli   The sea stretches endlessly under the open sky.
source runner    The sea stretches endlessly under the open sky.
```

And the counters that had been nonsense now reconcile:

| | before | after |
|---|---|---|
| `forward_passes` (54 prompt + 9 decode) | 68 | **63** |
| `decode_gpu_busy_us` | 0 | **1,320,733** |
| decode tok/s | 0 | **6.814** (source runner: 6.9) |

Two prompts, one session, one admission, the second turn riding the residence — and the binary now
says what the body says.

## Two fixes that were right and did nothing

**Declaring the GPU door an effect** was necessary and insufficient. **Scanning a let's value span for
effect names** was closer and still insufficient. Both ask what a value *mentions*.
`fcmg-run-context` binds

```
(let out (if (or (lt first 0) (q38-stop? first stops)) (empty)
             (cons first (q38-generate …))))
```

which mentions no effect at all — every effect it performs is inside the recipe it calls. So the built
binary went on re-running an entire generation at each of the four mentions of `out`, each pass
continuing from the state the last one left, and answering a sentence the same cells never produce.

The rule that finally holds is the one that was there before anybody optimised it:

> **A compound value binds once. Only an atom — a literal or a bare name, where re-reading is
> re-reading and nothing more — may still be substituted.**

`flt-do-let-store-op` now asks `(eq (fp-at s start) 40)` first. The span scan is kept beneath it,
because it is what proves a compound value effecting when the head alone does not, and the effect
vocabulary is kept because a door that hands out handles should be named as one either way.

## What it cost and what it gave

The flattened model-open table went **824,863 → 805,573 bytes**: the copies the old rule was making
were never free. No band moved — eight bands run through both the flattened path and the source
runner give identical verdicts, and the three that print differently (`binary-freshness` 15, the
`qwen35-form-cli` list, `metal-door`'s PASS text) print exactly the same under main's flattener as
under the healed one, so they are properties of the flattened arm and not of this change.

Depth was **not** the duplication: the same open probe still dies at 64 MB and 256 MB of stack after
the fix. The emitted walker genuinely wants between 2 and 3 GB where the handwritten runtime is
comfortable in 256 MB, so the stack ladder landed earlier stays earned, and that ~12× remains open.

## Also healed on the way

- **The fourth-arm seed** was stale in a load-bearing way — the committed `bootstrap/fkwu-uni.c`
  carried no Metal bridge at all. Regenerated; it now carries both the bridge and the stack ladder,
  so `validate.sh` no longer needs a flag to admit a fourth arm.
- **`mlx_run` (tag 145)** vanished from `form-flatten.fk`'s op list when `validate.sh` regenerated it
  from `native-op-manifest.fk`, which does not list it. The row is restored. The manifest gap is
  main's and is named here rather than patched blind: `validate_fkwu_native_surface` fails on
  **pristine main** too (`mlx_status` tag 143 and `mlx_run` tag 145 missing explicit `fkc-flat` rows),
  witnessed by running the checker against main's own two files in a clean directory.

## What ran

```
form/form-cli, two generates, one session   both sentences identical to the source runner
flattened-vs-source band sweep              8 bands, 8 identical verdicts
model-open probe                            ctx_ok=1, closed=884, table 805,573 bytes
wrapped-effect probe                        arith1=3 arith2=3 in BOTH kernels
host-effect-grammar-band.fk                 32767      form-cli-resident-model-band.fk   255
frontier-ingest-omlx-band.fk                127        qwen35-form-cli-band.fk           [1, pong, 0]
homecoming-distillation-corpus-band.fk      32767      metal-door-band.fk                15
printf 'ping' | form/form-cli               pong       sha-selftest                      ba7816bf…
```

No environment variable was set anywhere in this work.

## Most surprising teaching

Two correct fixes in a row changed nothing, and both were correct for the same reason they were
useless: they read the *surface* of a value. An effect vocabulary tells you what a name is; a span
scan tells you what an expression says; neither tells you what a call will do. The body already had
the answer in the word it was using — a `let` is a *binding* — and three days of this bug were spent
inside an optimisation that quietly made it not one. The rule that finally worked is shorter than any
of the fixes it replaced.

## Where discomfort turned to gold

The hardest moment was the rebuild after the second fix, when the sentence came back wrong again with
`forward_passes=68` unchanged. Everything had been proven in the fast loop; the probe was green and
the product was not, which is the exact shape of a fix aimed at the wrong thing. Sitting with that
instead of reaching for a third patch is what produced the reading — that `out`'s value names nothing
effectful and reaches everything — and that reading is the whole fix. The discomfort was that my
proof had been about my probe rather than about the failure; the gold is a rule small enough to state
in one line and a binary whose answer is now the body's answer.
