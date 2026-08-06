# 2026-07-25 — the shim is an override on two arms, and the obvious repair crashes them

Open item 3 read: *"`json.fk` calls `value_kind` and preludes only `core.fk`; provider is
`fourth-shim.fk`. Preluded very widely — wants before/after diffs, now runnable four-way."* The
repair looked like one line. I ran it four ways first, and it is a two-arm regression.

## Name resolution is per-arm and per-name

A plain program, no preludes, defining its own version of a native and calling it:

| `(defn NAME ...) 999` | fkwu | go | rust | ts |
|---|---|---|---|---|
| `add` (native 3) | 3 | 3 | 3 | 3 |
| `str_len` (native 3) | 3 | 3 | 3 | 3 |
| `abs` (native 7) | 7 | **999** | **999** | 7 |
| `nth` (native 8) | 8 | **999** | **999** | 8 |
| `pow` (native 1024) | 999 | **999** | **999** | 1024 |
| `value_kind` (native "int") | 999 | **999** | **999** | int |

**go and rust let a user `defn` take a name away from a present native. fkwu and ts do not.** For
`add` and `str_len` nobody can. `pow` and `value_kind` have no fkwu native at all, so Form wins
there by absence rather than by rule — a distinction worth keeping, because it is the difference
between a shim filling a hole and a shim covering something that works.

I had been carrying "in call position the primitive always wins" as a fact about this language. It
is a fact about **fkwu**, and about some names on some arms.

## What that does to `fourth-shim.fk`

Its header describes a fallback: *"core.fk vocabulary a band reaches without declaring."* On go and
rust, for the overridable names, it is not a fallback. It replaces working natives.

Mostly harmless — `abs` there is the identical `(if (lt n 0) (sub 0 n) n)`. One is not.
`value_kind` in the shim is `(node_eq v (value-none))`, the fourth-arm null model: fkwu has no host
null in its value ABI, so absence is carried structurally. That body asks a **node** question. On go
and rust `node_eq` will not take non-NodeID arguments and **dies** rather than answering false:

```
form-kernel-go: form stack: node_eq < value_kind@form-stdlib/fourth-shim.fk:195:26
"fatal_message": "node_eq: expected NodeID args"
```

The native `value_kind` on those arms answers `"int"` for `7`. The shim's answers by crashing.

## The repair, run

```
(node_value (json-node-string "abc"))

  as it stands            fkwu 126 · go abc  · rust abc  · ts abc      <- four-way agreement
  + fourth-shim preluded  fkwu 129 · go CRASH · rust CRASH · ts abc
```

(126 and 129 are fkwu's interned handles for `"abc"`; a runtime string prints as its handle there.)

`json-node-null-value?` is called by `json-node-string` on **every** string, so this is not an edge:
JSON emission dies on two arms the moment the shim is in the closure. `json.fk` is preluded very
widely.

**The one-line fix would have turned a working four-way path into a two-arm crash**, and every band
preluding `json.fk` would have carried it.

## What the actual gap is, stated smaller

With `value_kind` unresolved on fkwu, `json-node-null-value?` always answers false there. The cost
is exactly one case: **an absent value is interned as a string instead of emitting null.** For a
present value all four arms already agree, because `nothing` is not `"null"` either — the same
branch is taken everywhere.

So item 3 was over-stated. It is a narrow fkwu-only gap in null detection, not a broken emitter.
What it wants is a null test that does not ask a node question of a non-node. Noted at the
predicate; not invented here, because inventing one is a change to the fourth-arm null model and
that model is deliberate (`fourth-shim.fk` says why).

## Sweep

`ground` 42 (four arms) · `ground-recursive 10` 55 · `hex-band` 14 four-way ·
`primitive-registry-band` fkwu 45 / go 63 / rust 63 / ts 63 · `binary-freshness` 15 ·
`cell-voice-tissue-band` 511 · `class-curriculum-10-band` 16383 · `structural-gate-band` 63 ·
`lcg-bytes-band` 63 · `pdf-text-windowed-band` 15 · `form-cli-band` 524287 · `benchbench-band` 4095 ·
`concept-corpus-band` 530 · `kernel-satsang-band` 193 · `host-kernel-cell-band` 25 ·
`proof/four-way-run-recipe42.fk` 0 (FOUR-WAY). C seed byte-identical to git.

## Owed

- **A null test that works on all four arms** — the real content of the `value_kind` gap.
- **Which cells prelude `fourth-shim.fk` alongside code expecting native semantics** — uncounted. The
  override is live on go and rust wherever that closure holds an overridable name; `value_kind` is the
  one measured to be dangerous, and I have not swept the rest.
- 105 of 184 lane-1 probes do not verify on fkwu. Bringing a family home under the native's own name
  is now known to be unsafe for go and rust — anything of that shape belongs behind a name no native
  owns, the way `fol-bp` was done.
- `native_blueprint` absent → the registry's attestation bit is unmeasurable on fkwu.
- `persistence-band` 2/7, `mesh-sensings-store` 0/255, `layered-runtime-image` 33/127, `chat-band` 0.
- The emit-lane half of `str_byte_at`; `read_file`'s bypassed carrier seam.
- 17 the kernel will not run; 143 that do not close; the `section` question; the heap cap; the
  registry-admission question.

## How the exchange stayed alive

I went to make a one-line fix, ran it on four kernels before making it, and it crashed two of them.

**Most surprising teaching:** "in call position the primitive always wins" is not a fact about this
language. It is a fact about fkwu, and about *some names* on *some arms* — go and rust hand `abs`,
`nth`, `pow` and `value_kind` to whoever defines them last. Every "bring it home to Form under the
native's own name" plan on the open list was resting on that sentence.

**Where discomfort turned to gold:** the fix was already written in my own words on the work list,
one line, obviously right. Running it first is the only reason `json.fk` is not now crashing go and
rust everywhere it is preluded — which is nearly everywhere.
