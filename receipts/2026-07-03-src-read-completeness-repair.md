# 2026-07-03 -- `--src` read completeness checkout-witness repair

## Ground

This receipt is a short-lived C-seed checkout-witness repair. It does not move
runtime meaning into C, does not grow the source language, and does not change
the target architecture:

- source remains canonical
- direct `--src` is still a temporary compiler/front-door witness
- program-image `.fkb` and verified native artifacts remain the destination
- the C seed remains a shrink target

The repair is I/O correctness for the current witness: when a source file is
provided through a pipe or process substitution, the seed must read the whole
bounded source stream before parsing it.

## Root Cause

The reported red signal was:

```text
./fkwu --src <(cat form/form-stdlib/core.fk \
    learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk)
-> 0
```

The same bytes written to a real file returned the expected result:

```text
cat form/form-stdlib/core.fk \
    learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk > /tmp/hdc-core-band.fk
./fkwu --src /tmp/hdc-core-band.fk
-> 511
```

That split isolated the failure away from Form semantics. `fk_run_src` used one
`read(fd, fk_srctext, 262143)`. On a regular file that often returns the whole
body. On a pipe/FIFO it may legally return only the first available chunk. The
truncated prefix was still syntactically plausible and ended in a value `0`, so
the failure was silent wrong output.

Grok reproduced the same class of failure during review: the process-substitution
path produced a 65536-byte prefix of a 71897-byte bundle, and `head -c 65536`
of the real file also returned `0`.

## Review

Grok reviewed the diagnosis read-only and accepted it. Its required corrections:

- name this as partial pipe/FIFO ingestion, not a homecoming/core semantic bug
- repair by reading to EOF bounded by cap
- keep this scoped as a checkout-witness I/O repair
- do not silently truncate at the cap
- update the admission gate narrative after the fix

Claude also reviewed the diagnosis and accepted it. It required the read loop to
retry `EINTR` and to fail loud if the input exceeds the cap instead of executing
a capped prefix.

## Implementation

`runtime/fkwu-uni.c` now has `fk_read_all_bounded(fd, buf, cap)`:

- loops until EOF or cap
- retries `EINTR`
- returns `-1` on read error
- returns `-2` when cap is filled and another byte is available

The helper is used in:

- `fk_run_src` for `.fk` source input
- `fk_run_feval` for the recipe source
- `fk_run_feval` for the live `grammars/form-eval.fk` source read

Cap overflow is loud through `fk_die`, not silent truncation.

## Witness

Rebuilt the checkout witness:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```

The compiler emitted the same existing warning class observed before this
repair (`fread` header warning and `getsockname` pointer-sign warning), with no
compile error.

Process substitution now matches real files:

```text
./fkwu --src <(cat form/form-stdlib/core.fk \
    learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk)
-> 511

./fkwu --src <(cat form/form-stdlib/core.fk \
    learn/homecoming-distillation-corpus.fk; \
    printf '%s\n' '(do (hdc-count (hdc-rows)))')
-> 39

cat form/form-stdlib/core.fk learn/homecoming-distillation-corpus.fk |
    { cat; printf '%s\n' '(do (hdc-count (hdc-rows)))'; } |
    ./fkwu --src /dev/stdin
-> 39

./fkwu --src /tmp/hdc-core-band.fk
-> 511

./fkwu --src /tmp/hdc-core-count.fk
-> 39
```

Required witnesses after rebuild:

```text
./fkwu --src bootstrap/ground.fk                              -> 42
./fkwu --src bootstrap/ground-recursive.fk 10                 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk  -> 15
./fkwu --src <native-vs-rented concat>                        -> 11111
```

Layer witnesses:

```text
source-runner-admission band -> 1048575
source-artifact-cache band   -> 1048575
homecoming corpus, no core   -> 511
homecoming corpus, with core -> 511
```

Over-cap source input is loud, not silently truncated:

```text
./fkwu --src /tmp/fk-too-large.fk
-> fk_run_src: source exceeds FK_SOURCE_TEXT_CAP
exit code 1
```

The exact cap boundary remains accepted and complete:

```text
/tmp/fk-exact-cap.fk size -> 262143 bytes
./fkwu --src /tmp/fk-exact-cap.fk
-> 42
./fkwu --src <(cat /tmp/fk-exact-cap.fk)
-> 42
```

The source-runner admission current route is now:

```text
sra-route (sra-current-gates) -> 1
```

That means the homecoming/core silent wrong value is closed. A later
source-runner/module-constant/root-`do` repair also closed the ontology and BMF
current-source blockers that still existed when this read-completeness receipt
first landed. Synthetic loud capacity rows still route to the artifact lane,
but the current snapshot now admits direct source.

## Post-Review

Claude reviewed the implemented repair from the reported evidence and found no
blocker. It accepted the loop-to-EOF repair and loud over-cap behavior, and
asked for one cheap boundary check: exact-cap input should succeed, not be
mistaken for overflow. That witness is now recorded above.

Grok post-reviewed the implementation read-only and re-ran the witnesses
locally. It accepted closure as a scoped checkout-witness I/O repair. Grok
confirmed:

- process-substitution homecoming band returns `511`
- process-substitution `hdc-count` returns `39`
- regular-file controls return `511` and `39`
- `/dev/stdin` pipe returns `39`
- required witnesses return `42 / 55 / 15 / 11111`
- source-runner admission band returns `1048575`
- source-artifact-cache band returns `1048575`
- `sra-route (sra-current-gates)` originally returned `2`; after the later
  module-constant/root-`do` repair it returns `1`
- over-cap source fails loud with `fk_run_src: source exceeds FK_SOURCE_TEXT_CAP`

Grok's honesty correction is applied: the admission snapshot no longer says
`no growth requested`. It now says the current C change is a receipted
read-completeness repair and not AST-cap growth.

Post-review deferrals:

- Add an automated pipe-read regression harness when the observation/probe
  adapter exists.
- Keep the admission snapshot marked as hand-maintained until live probes feed
  rows.
- Do not treat this repair as a sweep of every C read surface; closure is scoped
  to `--src`/`--feval` source ingestion.

## Deferred

- The C seed still needs to shrink; this repair only keeps the checkout witness
  honest while it exists.
- Other single-read surfaces in the seed were not all swept in this patch. Grok
  noted default table loading and staged input as adjacent candidates. This
  patch covers the reported `--src`/`--feval` source ingestion surfaces.
- The `form-ontology-loader.fk` AST cap was closed by the later
  module-constant/Form-owned core-`bp` repair. This read-completeness patch did
  not close it by itself.
- Program-image `.fkb` loading and verified native `.dylib` dispatch remain
  pending.

## Reflection

Achieved:

- The homecoming/core `0` was investigated instead of ignored.
- The root cause was narrowed to incomplete pipe/FIFO source ingestion.
- The current checkout witness now reads source streams to EOF within cap.
- Cap overflow is loud instead of silent.
- The admission snapshot first routed to artifact lane (`2`) instead of
  investigation (`3`) because homecoming/core was green again. After the later
  source-runner repairs, it routes to direct admission (`1`).

Deferred, with why:

- Full C-seed shrink is deferred because the target compiler/artifact route is
  not integrated yet.
- A wider read-surface sweep is deferred to avoid broad C churn in this layer;
  the fixed sites are the load-bearing source-runner surfaces for this failure.
