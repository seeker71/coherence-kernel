# Receipt — merge, re-ground, walk (2026-07-01)

The cycle named directly: commit the day's work, merge to main, rebuild `fkwu` from the merged
tree and re-run the bootstrap grounding cells, then walk the three proof-oracles over what's new.
Not narrated after the fact — each step run and read before the next.

## Merge

`claude/practical-gates-55ddda` (commit `eb1852bd`) fast-forward merged into local `main`
(`fe05960f` → `eb1852bd`) from the primary worktree. Not pushed to `origin` — that's a further,
separate step, not implied by "merge" alone.

## Re-ground

Fresh `cc -O2` build of `runtime/fkwu-uni.c` from the merged tree, then the same bootstrap
sequence AGENTS.md opens with:

```
bootstrap/ground.fk                      -> 42
observe/native-vs-rented.fk (check)      -> 11111
observe/core-grounding.fk (check)        -> 11111
```

Bit-identical to every prior grounding this session. The body stands on the merged state.
Every band landed today re-verified on this fresh binary too: `core-str-shim-band` 15,
`reception-consent-band` 255, `arrival-band` 1023, `relationship-store-band` 31, `come-in-band` 31
— unchanged from pre-merge.

## Walk — and an honest correction to today's earlier claims

Built `walkers/go` and `walkers/rust` fresh from the merged tree (`walkers/ts` via `npx tsx`, no
build step) and ran them directly, rather than assuming the Tier 0 shim would be walker-portable
because it's "just" Form calling documented primitives.

**It is not.** `core-str-shim-band.fk` fails on all three:

| Walker | `char_at` (-> `substring`) | `ord` (-> `str_byte_at`) |
|---|---|---|
| Go | works (`substring` is in its documented surface) | **`unbound function "str_byte_at"`** |
| Rust | **`unbound function: substring`** | **`unbound function: str_byte_at`** |
| TS | fails | fails |

This is a **more fragmented picture than this morning's review captured**: Grok's review (and my
synthesis of it) treated `str_byte_at` as safely "native" without checking whether the walkers
carry it — they don't, on any of the three. And the walkers disagree with *each other*: Go has
`substring`, Rust doesn't. This is a live instance of exactly the root-cause diagnosis from
`receipts/2026-07-01-tier0-ord-char-at-shim.md`'s own Tier 1/2 discussion — a fresh Form recipe,
written today, silently isn't four-way provable, and nothing caught that except manually running
all three walkers by hand.

**What this does and doesn't mean:**
- The actual bug fixed today (bytes corrupting to null on `fkwu --src`) is real and stays fixed —
  that corruption only ever happened on `fkwu`, so `fkwu`-witnessed is the correct and sufficient
  proof for *that* claim, not a shortfall.
- But the Tier 0 shim itself is **not** a four-way-provable recipe as written, and I should not
  have implied otherwise. Naming it now rather than letting it stand uncorrected.

**Clean four-way, reconfirmed post-merge (unaffected by the shim):**
- `core-band.fk` (pre-existing, doesn't call the new `char_at`/`ord`): Go=Rust=TS=**255**.
- `reception-consent-band.fk` (no `bp`/`intern_node`/string-primitive dependency at all): Go=Rust=TS=**255**.

**Confirmed walker-incompatible by design, not a bug** (per `walkers/README.md`'s own scope: pure
recipe surface only, no host effects, no node identity):
- `arrival-band.fk` → Go: `unbound function "intern_trivial_string"`.
- `relationship-store-band.fk` → Go: `unbound function "fs_exists"`.
- `come-in-band.fk` inherits both.

## What this actually argues for

This walk is itself a small, unplanned data point for Tier 1 (the per-target op-availability
manifest from `receipts/2026-07-01-tier0-ord-char-at-shim.md`): a manifest built only from
`fkwu-optable.h` would have called `char_at`/`ord` "fine" the moment they're defined in
`core.fk`, without ever knowing the walkers disagree on `substring` and lack `str_byte_at`
entirely. Confirms Grok's specific pushback — the manifest needs a real per-walker column,
not just the native optable — with a concrete example now on record instead of a hypothetical.

Not fixed here — Tier 1 remains its own stone, scoped, not started today.
