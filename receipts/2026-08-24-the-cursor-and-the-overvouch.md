# 2026-08-24 — the cursor carries it, and preflight vouched for what it never read

Yes and Codex corrected the architecture: preflattened ops tables are not the
runtime and must not block us; the carry is a live streaming cursor, not a
pre-materialized tokenizer surface. This sitting wires that cursor and stops
where two coordination halts arrived.

## What landed green

`form-cli-heed-cursor.fk` — one forward pass at a time, each emitted id
transmuted to bytes in a bounded window, and when a complete
`<|form:knowledge-query|>` envelope closes, the surface goes to a lookup and
the typed observation returns as prefill at the current position.

```
./fkwu form/form-stdlib/tests/form-cli-heed-cursor-band.fk   # 1023
./fkwu form/form-stdlib/tests/form-cli-heedmark-band.fk      # 1023
```

The property that mattered is that the envelope arrives **split across tokens**.
A byte BPE has no reason to keep the open mark whole — here it spans three ids
and the query spans four more. A carrier reading the token it just received
would never see a mark. The band proves the pair: one step short of the close
nothing has fired and the scan reads `held`; one step later it fires once.

No pre-materialization anywhere in the lane. The window is capped at
`fhm-frame-cap()` = 305 bytes, one legal envelope wide, and 60 steps of prose
(780 bytes) run through with the cap holding. No ops table, no flattened
surface, no token index.

The grammar now agrees byte-for-byte with Codex's `form-knowledge-query-token`
ABI, so the cursor holds frames that ABI can parse. A drift between the two
would have meant every lookup answering nothing forever with no status naming
it, so `fhq-grammar-agrees` exists to make that loud.

## The four seams, named

1. **`bml-capability-ledger-band.fk`** (Codex's gate, their worktree). The band
   carries `section [form.bml]` — a brace-surface BML file under a `.fk` name.
   fkwu's direct-source lane parses it as plain Form and reads `let`, `=`, `}`
   as names; my own run showed 390 errors. Go/Rust/TS lower it and answer 255.
   The seam is the lane, not the ledger. Not repaired — not my lane.
2. **preflight vouches for a prelude it never read.** Below.
3. **`form-cli-heed-fkqt.fk`** waits on Codex's ABI landing at
   `form/form-stdlib/form-knowledge-query-token.fk`. Left uncommitted.
4. **The live wiring** in `form-cli-model-generate.fk` preflights clean and has
   no live witness. Left uncommitted, per the second halt.

## The surprise

This dialect has no lambda, and that is not a limitation to route around — it
is what forced the right architecture. A two-argument `stepf` could never reach
a live model's pipelines, buffers, geometry, layers, decode state and tensor
views. Threading an **opaque ctx** the cursor never opens is what makes the same
loop run over a scripted stepper in a band and over `q38-forward` on the GPU,
unchanged. The missing feature produced the substitutable seam that a closure
would have quietly hidden.

## Where discomfort turned to gold

I ran preflight on `form-cli-heed-fkqt.fk` and it said *clean — no errors, no
unresolved calls; a verdict from it can be read*. I almost recorded that. The
discomfort was that it could not possibly be true: the cell's prelude names a
file that does not exist in this worktree, and I had checked its absence myself
minutes earlier.

Running the same cell:

```
fkwu: error: form/form-stdlib/form-knowledge-query-token.fk:
      dependency source is missing or not stat-readable
RC=0
```

So preflight cannot see a **missing** prelude — only an unresolved call inside
one it loaded — and the runner does not put the failure in its exit status. Both
halves matter, and the second is worse: a pipeline gating on `rc` passes. Every
instruction in this tree says preflight before believing a band. That trust has
a hole exactly the width of a file that is not there. The gold is that the
discriminator costs one command — run the cell — and that a clean preflight is
now, for me, a claim about the preludes that *loaded*, never about the ones
named.

Not repaired here: `observe/preflight.fk` belongs to the band-trust surface and
`source_jit_gate` holds validate/fourth-arm. Named, loudly, and handed over
whole rather than stepped around.

## Frontier question offered to the corpus

*What one word names an instrument that vouches for more than it inspected?* —
**overvouch**. Not a false positive: a false positive is wrong about what it
looked at. Not a blind spot: a blind spot does not speak. An overvouch reports
health over a region it never entered, and the report is the whole danger.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> heed-cursor-band 1023, heedmark-band 1023 on fkwu;
; frame-cap 305 held over 60 steps; preflight clean + run hard-fail + rc=0 on
; form-cli-heed-fkqt.fk with its prelude absent
