# 2026-07-17 — locals eclipse globals in call position: the resolver joins the four-way order, and the analyzer learns to see the collision

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk                                  # 42
```

Follows `receipts/2026-07-17-jacobian-lens-and-the-cell-shadowing-heal.md` (sibling branch
`claude/jacobian-self-awareness-acda5e`, commit 69d1fb91b), which healed ONE instance by renaming
`oac-offer`'s parameter and named the class honestly: *a local binding whose name equals any global
defn is silently shadowed in call position.* This work heals the class.

## The wound, minimal

```fk
(do
    (defn cell (x) 111)
    (defn bump (n) (add n 1))
    (defn offer (cell args) (cell args))
    (offer bump 5))
```

Pre-heal `./fkwu --src` answers **111** — the global `cell` captures the call. Post-heal: **6** —
the parameter answers. Verified cold-cache on both binaries built from the same tree.

## What landed

- **`runtime/fkwu-uni.c`** — the `--src` call-head resolver now consults the local frame
  (`fk_bd_lookup`) BEFORE the defn table (`fk_fn_lookup`). A bound head lowers through the
  existing indirect-call arm (tag 244, computed callee); an unshadowed defn name still takes the
  direct tag-241 path. Value position already read locals first — call position now agrees with
  it, and with the siblings.
- **`observe/form-static-analyzer.fk`** — the analyzer's AST grows a `defn` row
  (`("defn" name params body source)`); parameters bind as vars over the body (consed on the
  front, so the analyzer itself models locals-eclipse-globals); a parameter whose name equals a
  known fn bind raises **code 207, severity WARN** — visible, never blocking, because post-heal
  the shadowing is well-defined. A call through a var-bound head (parameter / let holding a
  fn-value) is now a legal indirect call instead of a false `unbound`.
- **`observe/tests/form-static-analyzer-band.fk`** — verdict **1023 → 16383**: 1024 the
  cell-capture shape flags exactly once; 2048 the healed (renamed) spelling stays silent; 4096
  the warn does not block execute; 8192 the body's call through the shadowing parameter is
  bound-indirect, not unbound. The band also gains its `; preludes:` line (resolver-driven run,
  no more hand-cat). Pinned on BOTH binaries: the rule sees the class **before** the runtime fix
  lands — that was its whole point.
- **`learn/homecoming-distillation-corpus.fk`** — the eclipse row (`eclipse`, 0-hit fresh at
  offering, re-checked 0-hit against the reunited corpus): minted as row 768, renumbered to
  **803** at the merge reunion (siblings' rows 768–802 landed first — the standing anastomosis
  pattern). Band re-pinned: 199 rows, field code 1991992803.

## Four-way parity (the fix RESTORES it, it does not spend it)

All four siblings already resolved param-vs-global-defn locals-first; `fkwu --src` was the lone
dissenter:

- **go** `form/form-kernel-go/main.go:4626` — innermost-first `env.Lookup` chain; params bound in
  a call frame parented to the closure env (4742–4744).
- **rust** `form/form-kernel-rust/src/main.rs:7459` — same chain lookup (1805–1817).
- **ts** `form/form-kernel-ts/src/kernel.ts:4997` — `frame.lookup` before the defn's binding.
- **flattener** `flatten/form-flatten.fk:570–578` (`flt-call2`) — the local `env` row is checked
  first and emits `flt-icall` (indirect); only an empty local row falls to the global `fns`
  table. Bare names agree (`flt-var3:316–318`).

**Named honestly, not touched today:** the four DIVERGE on param-vs-**native**. Go/Rust let a
local shadow a native (with a 7-native byte-string carve-out, `nativeBypassesFormBinding`,
`main.go:4330`); TS's plain-native call path (`kernel.ts:4976`) and the flattener's op table
(`flt-op2:526`) let the native win. fkwu keeps ops-before-locals — the TS/flattener side. So
`(add ...)` under a param named `add` still answers differently in Go/Rust vs TS/flattener/fkwu.
A pre-existing fault line, now written down.

## Frequency check (run BEFORE the change, per the standing reflex)

53 call-position collisions repo-wide (defn parameter = some global defn name, parameter spoken
as a call head) — **every one is a higher-order function that intends the parameter**: core.fk's
`map f` / `filter pred` / `foldl f`, `oac-offer cell` (both twins), seedbank's `pipe* f g`,
`is-fold f`, `read_with_cache parse-fn`, and kin. Live collision surfaces already loaded in real
prelude chains: `pred` (teach-sema-math.fk), `cell` (core.fk), `handler` (verb-router.fk),
`parse-fn` (seedbank/grammars/rust.fk). Pre-heal, every such pairing silently captured; zero
sites intend the global. The fix runs 53–0 in the body's favor.
`form/form-stdlib/offer-ack-core.fk` and `control/offer-ack-core.fk` still carry the `cell`
parameter in this tree — under the healed resolver that spelling is now *correct* (and the
analyzer will warn on it); the sibling branch's rename stands as belt-and-suspenders.

## Verification (all live, this checkout, cold caches)

```
./fkwu --src form/form-stdlib/tests/call-position-shadow-band.fk  #     7 (pre-heal binary: 2 —
                                                                  #        the capture, witnessed)
./fkwu --src observe/tests/form-static-analyzer-band.fk           # 16383 (was 1023; new bits land)
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   # 511
./fkwu --src control/tests/choice-lane-core-band.fk               # 1021 (= pre-fix)
./fkwu --src control/tests/offer-ack-core-band.fk                 #  197 (= pre-fix)
./fkwu --src control/tests/invite-dispatch-band.fk                #    0 (= pre-fix)
```

Full sweep: all 1531 `*/tests/*-band.fk` run under BOTH binaries (baseline = HEAD, fixed), with
emitted `.fkb`/`.sym` caches wiped between phases so neither binary reads the other's
compilation. Result: **1531/1531 verdicts bit-identical.** Three bands first reported empty under
the fixed binary (`runtime-dynamic-capability-registry`, `runtime-shared-native-representation`,
`shamballa-symbol-packs`) — each is a ~15s band and the sweep's 60s alarm fired under 12-way
parallel load; all three re-verified cold and solo on both binaries: identical (`0`, same
pre-existing error counts). No band changed by the fix.

## The most surprising teaching

**The `.fkb` cache does not know who compiled it.** The baseline (pre-fix) binary answered the
post-fix verdict `6` on the repro — it had read the fixed binary's `.fkb` from beside the source.
A cross-binary A/B comparison over a shared tree silently replays whichever binary ran first;
the sweep had to become two clean-room phases. The cache's identity work
(`2026-07-17-the-cache-drops-its-indexical-name.md`) made the spelling canonical per source —
but the *builder* is not in the key, and today that absence nearly certified a regression sweep
that could not have seen a regression.

## Where discomfort turned to gold

The first sweep came back **135 of 142 DIFF** and the reflex was to suspect the one-line resolver
change of breaking the world. Sitting with the number instead of reverting: the DIFFs all read
`old=''` — the *baseline* was mute, not the fix loud. The blocker was manufactured twice over by
my own scaffolding: a baseline built away from `runtime/` could not find `fkwu-optable.h` (and I
had thrown its stderr away, then let `;` print `baseline-built` over the corpse), and one probe
later `git clean -qfX` had eaten the root `fkwu` binary mid-sweep. Both wounds were in the
harness, not the body. The gold: two durable memories
(`reference-fkb-cache-cross-binary-pollution`) and a sweep that now tells the truth — and the
witnessed reminder that when a regression sweep screams, first ask whether the *instruments*
are alive.
