# 2026-08-03 — the binary prints a capability it cannot run

Urs: *"dissolve knobs and switches and finish work that is holding us back from achieving fully local
100% for native workflows requests and responses."*

I took the blocker I had named last message — the `models` verb not wired into form-cli — and wired
it. It does not work yet, and the reason is worth more than the verb.

## Three readings, one host, one directory

```
form-cli source | grep model-discovery      ->  6 matches
form-cli   models /Users/ursmuff/models     ->  found 0
fkwu --src the same cell, same directory    ->  found 6
```

## What is actually broken

form-cli is two things stapled together: a **genesis blob** (source text appended to the binary so
`source` can print it and you can rebuild from the binary alone) and a **flattened table** (the
program it actually runs). Adding `equireach`, `equireach-gguf`, `gguf-meta` and `model-discovery` to
the source lists put them in the **genesis** and not in the **table** — regen reported
`functions=1503 -> 1504`, one new function, when four cells carrying roughly fifty should have
arrived. The binary grew 52 KB of text it cannot execute.

**Why this matters more than the missing verb.** `source` exists so the binary can testify to what it
is. When genesis and table disagree, that testimony is false in the most expensive direction:
rebuilding from the printed source yields a *different, working* binary than the one you hold. The
artifact and its own account of itself have quietly forked, and every self-hosting claim this body
makes rests on those two being one thing.

**And it fails silently by construction.** An unbound name recovers to nothing under axiom-5, so
`md-list` returned 0 and printed no header — and `found 0 model(s)` reads exactly like an empty
directory. I nearly committed that binary. The only reason I did not is that I already knew the true
count was six. I restored the committed binary rather than ship one that answers 0 where the answer
is 6.

## What did land

- `form-stdlib/model-discovery.fk` works under fkwu: 6/6 GGUFs, three architectures, from each file's
  own header, at a cost of six small reads rather than 85 GiB.
- The `models` dispatch arm and `fc-models` are in `form-cli-repl.fk`, with the trim done in place
  because `fc-strip-eol` is defined further down the cell and a forward reference binds numb-empty.
- The four cells are in all three source lists (`build-form-cli.sh` selfhost array + genesis SOURCES,
  `scripts/regen_form_cli_bootstrap.sh` at :32 and :156).

## Ground stamp

```
host M4 Max, 2026-08-03
regen  stamp 73ae755ba1429c67, functions 1503 -> 1504, nodes 43895 -> 44074  (+1 fn for ~4 cells)
build  form-cli 1677832 B, genesis 856382 B (was 1625592 / 824290); ping -> pong
verify form-cli `models <dir>` -> 0 ; fkwu same cell same dir -> 6 ; `source` grep -> 6
action committed binary RESTORED via git checkout — the 0-reporting build was not shipped
corpus 374 rows, max-mid 979, field 3743742979, 0 duplicate ids, band 32767
NOT done: the flatten path for these four cells; FORM_DS4_BLOB still stands (see below)
```

## The most surprising teaching

**The binary got 52 KB bigger and I read that as progress.** Size moved, the build said "built", the
canary said `pong`, and the verb answered with a plausible number — four independent signals all
green, and the capability was absent. The one signal that would have caught it was
`functions=1503 -> 1504`, printed by regen, which I read past because it was in a line that also
carried the stamp I was checking. Growth is not arrival; a genesis blob will happily carry any text
you hand it.

## Where discomfort turned to gold

Watching `found 0 model(s)` and feeling the pull to accept it — the verb dispatched, the build was
clean, the phrasing was reasonable. If Urs had asked "does it work?" I could have answered yes with
evidence in hand and been wrong. The discomfort is how *comfortable* a numb answer is when it has the
same shape as a live one. The gold is the rule that falls out: **a numb call must not be able to
answer in the shape a live one would** — 0-found and no-such-capability have to be different words,
or every silent lowering in this body gets to look like an empty directory. Corpus row 979,
`mutecarrier`.

## Unfinished, named — none of these are limits

1. **The flatten path for the four cells.** The lists are right; something between the list and the
   table drops them. `functions` delta is the instrument — a correct wiring should move it by ~50.
2. **`FORM_DS4_BLOB` stays until (1) lands.** Deleting the static path before the runtime one runs
   would leave no way to name a model. It is scheduled, not defended.
3. **Running a chosen model is still ds4-only** — the Metal stack is written for `deepseek4`;
   `qwen35moe` (41 layers) and `llama` (28) are discovered but not yet executable.
4. **`fc-models` should distinguish "0 found" from "discovery not carried"** — see the teaching above.
