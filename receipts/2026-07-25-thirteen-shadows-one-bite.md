# 2026-07-25 — thirteen shadowed names, one that bites, and a claim that was owed a stamp

Item (1): size the native-shadow class properly. Three instances had turned up by accident today
(`bp`, `empty`, `floor`); the question was how many there are.

## The detector, and its two smoke tests

A Form `defn` is **dead** when its name is in fkwu's optable, because in call position the primitive
wins. So: every `defn` name in the tree, intersected with the 169 optable names.

Smoke-tested both ways before believing it — `bp` should appear (known dead), `hex-encode` should not
(a Form-only name). Both correct. `floor` no longer appears, which is the third confirmation: my
rename last turn cleared the only definition of it.

53512 `defn` definitions in the tree. **13 names shadowed:**

```
abs  bp  dot_product  nth  read_file  record?  record_blueprint
record_get  record_has  record_keys  str_byte_at  write_file  write_file_text
```

## Twelve are deliberate; one bites

The count is not the finding. **Being shadowed is usually intentional here**, and the sharpened
criterion is not *"is it shadowed"* but *"does the native do something different from what the Form
definition means"*.

| name(s) | where | verdict |
|---|---|---|
| `abs`, `record?`, `record_blueprint`, `record_get`, `record_has`, `record_keys` | `fourth-shim.fk` | **by design.** Its own header: *"the flattener's standing prelude — core.fk vocabulary a band reaches without declaring."* It exists to supply vocabulary to lanes that lack the natives. Measured: `abs(-7)` = 7 both ways, and the Form body is the identical `(if (lt n 0) (sub 0 n) n)`. |
| `read_file`, `write_file`, `write_file_text` | `form-fs.fk` | same shape, one cost — see below |
| `str_byte_at` | `fsh-flatten-mod.fk` | agrees with the native today; see the re-witness below |
| `nth` | form-samples | self-contained demo files |
| `dot_product` | `form-kernel-rust/recipes/` | a recipe file |
| **`bp`** | `form-ontology-loader.fk` | **the one that bites** — a pass-through stub answering 902 to names no registry has reviewed, where the Form version raises `form_error`. Already worked around this morning via `fol-bp`. |

**One of thirteen was harmful, and it was already known.** The class is real, small, and mostly
intentional — which is the opposite of what three accidental discoveries in one day suggested.

The one new cost worth naming: `form-fs.fk` defines `read_file` → `host-fs-read-text` and states its
purpose plainly — *"These are the only raw file-carrier calls in this module. Other stdlib code
should use host-fs-\* or fs-\* so the OS carrier remains swappable."* The native shadows that wrapper,
so a caller writing `read_file` reaches the host directly and the swappable-carrier indirection is
bypassed. It works; what is lost is the seam the module was built around. Named, not changed —
changing it means renaming a native's worth of call sites.

## A claim owed a stamp

`str-byte-at.fk` opens: *"The kernel exposes a `str_byte_at` native, but it is NOT correctly emitted
into the 4th kernel — on fkwu it returns 0 (found while bringing Postgres home over pg-wire)."*

Measured today on `fkwu --src`: `(str_byte_at "ABC" 0)` is **65**, `(str_byte_at "ABC" 1)` is **66**.
The native answers correctly on that lane.

Re-witnessed in place with a banner, the original sentence kept verbatim — the ledger records what
was believed and when. And the re-witness states its own limit, which is the part that matters: the
claim says *"not correctly **emitted**"*, and the emit/flatten lane is a different path from `--src`.
**I measured only `--src`.** So the sentence may still hold exactly where it was written and be false
where I looked. That distinction is what remains owed, and whoever next runs the emit lane can close
it.

The Form recipe below the banner stands either way — a recipe crossing four kernels is not made
redundant by one kernel's native being repaired.

## Sweep

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · `hex-band` 14 ·
`cell-voice-tissue-band` 511 · `class-curriculum-10-band` 16383 · `content-address-band` 1111111111 ·
`triangulate-band` 1700 · `midi-bmf-band` 1500 · `structural-gate-band` 63 · `lcg-bytes-band` 63 ·
`file-byte-window-band` 2147483647 · `pdf-text-windowed-band` 15 · `form-cli-band` 524287 ·
`benchbench-band` 4095. C seed byte-identical to git.

## Owed

- **The emit-lane half of the `str_byte_at` claim** — untested here, named in the cell.
- **`read_file`'s bypassed carrier seam** in `form-fs.fk`.
- Five failures still visible: `audit-evidence-cells` 544/1023, `audit-evidence-index-cache` 833/1023,
  `layered-runtime-image` 33/127, `concept-corpus` 143/530, `json-lens-tending` 189/255. Plus
  `persistence-band` 2/7, `mesh-sensings-store` 0/255, `chat-band` 0.
- 17 of the 44 still refused; 143 that do not close; the `section` question; the heap cap; the
  registry question.

## How the exchange stayed alive

I went looking for a class I had met three times in one day, and reported that twelve of its thirteen
members are working as designed.

**Most surprising teaching:** three accidental encounters made shadowing feel like a systemic defect,
and the census says it is a systemic *technique* — `fourth-shim.fk` exists precisely to be shadowed
where the native is present. What made `bp` and `floor` bugs was never the shadow. It was that the
native meant something different from the definition it covered: a stub instead of a lookup, an
arity-1 math op instead of a zero-arg constant.

**Where discomfort turned to gold:** finding my own framing was the thing that needed correcting.
"Native-shadow class, three instances, expect more" was a good story built from three data points,
and the census turned it into one real case I already knew about. Writing that down is less
satisfying than a fix, and it is the more useful thing in the ledger.
