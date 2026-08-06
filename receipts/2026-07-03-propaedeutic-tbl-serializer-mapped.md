# 2026-07-03 — propaedeutic: the .tbl serializer, mapped slowly before a line is written

## Ground

```sh
tr -s ' \n' ' ' < proof/four-way-run.tbl | cut -d' ' -f1-20   # 1 63 64 24 0 0 0 24 3 0 0 136 0 1 0 31 2 0 0 1
./fkwu proof/four-way-run.tbl ; echo $?                        # 0 (FOUR-WAY) — a .tbl is a runnable artifact
```

Urs: *"yes, slow and careful wins over rushing and not fully understanding what we are looking at."*
The green light to go at the missing organ — a Form-native `.tbl` serializer — with the method named:
understand fully before building. This receipt is that understanding, embodied so the build stands on
it (as [2026-07-01](receipts/2026-07-01-four-way-run-tbl-regeneration.md) embodied the blocker).

## Fully grounded: the target format (from the deserializer, the authoritative spec)

`fk_run` (runtime/fkwu-uni.c:8275) reads a `.tbl` as space-separated decimals via `fk_next()`:

```
nf                         # function count
fn[0] … fn[nf-1]           # each fn's ROOT node index
nr                         # node count
node[r][0..3]  × nr        # each node = 4 ints: (tag, field1, field2, field3) -> fk_node[r][0..3]
ns                         # string count
(sl, byte[0..sl-1]) × ns   # each string: length then sl byte-ints
```

Verified against the bytes: `1`(nf) `63`(fn[0]=root node 63) `64`(nr) then 64 rows of 4 —
`24 0 0 0` / `24 3 0 0` / `136 0 1 0` / `31 2 0 0` — matching `(tag, f1, f2, f3)`. After load, `fk_run`
walks `fk_fn[0]` and prints result + the 1..255 arms histogram; the verdict is the exit/output.

## Fully grounded: the serializer's interface (from its two call sites)

`flatten/fourth-flatten-driver.fk:45` and `form-cli/fsh-flatten-mod.fk:6,26` both call it identically:

```
(print_str (fks-table-file (flt-band-sources-fns mods band)      ; arg1: the FN LIST (fn 0 = root)
                           (flt-band-sources-pool mods band)))   ; arg2: the STRING POOL
;  fkc-table-file is the pool-free twin (fn list only)
```

So `fks-table-file (fns pool)` returns the `.tbl` **string**. `flt-band-sources-fns/pool` (form-flatten.fk
:939/921) already build the fn-list and pool — the tree-builder half exists. The emit pattern is a
solved shape: `flatten/gen-source-walker-table.fk` already folds `int_to_str`+`str_concat` over rows to
build a table string (it's how the C optable is regenerated). So emission is a template, not a mystery.

## The crux, and the assumption the probe DESTROYED (this is why we go slow)

The Form node model is NOT the `.tbl`'s 4-int row. Probed directly:

```
(intern_node 42 (list 7 9)) -> node_value = 0 (NOT 42), arity = 2, child0 = 7
```

`intern_node(head, children)`: the first arg is an operator NODE, not a raw tag —
`ms-intern (a b) = (intern_node (bp "MS-PAIR") (list a b))` (surface/minimal-surface.fk:24). `node_value`
reads a literal's value (0 for a pair); `node_children` reads the child list. Nodes layer:
`ms-intern` (pairs), `fk-node tag payload` (tagged nodes, form-flatten.fk ~151), `fk-lit`/`ms-cell`
(literals via `intern_trivial_int`). **The serializer must TRANSLATE this (head, children) graph into the
`.tbl` (tag, f1, f2, f3) rows** — a graph serialization: traverse fn roots via `node_children`, number
each reachable node, remap child references to those numbers, and derive each node's 4-int row from its
tag+fields. Had I assumed `node_value == tag`, every emitted row would be wrong.

## The build, scoped (next, carefully — not this turn)

1. **Understand the node→row translation** — how `fk-node`/`fk-lit`/`ms-*` map to `fk_node[r][0..3]`
   (read form-flatten's node constructors against the tag semantics the deserializer expects).
2. **Write `fks-table-file` / `fkc-table-file`** as a graph serializer: traverse + number + remap + emit
   `nf/fn/nr/nodes/ns/strings`, folding `int_to_str` per the gen-source-walker-table template.
3. **Verification gate (non-negotiable):** produce a `.tbl` from `proof/four-way-run.fk` +
   `four-way-verdict.fk` and require it **byte-identical** to the committed `proof/four-way-run.tbl` (or,
   failing byte-identity, `./fkwu <new.tbl>` exits **0 / FOUR-WAY**). No `.tbl` ships without that check —
   it touches the byte contract every existing `.tbl` depends on.

## The most surprising teaching this work left behind

A four-token probe overturned the whole plan. I had a serializer half-drafted in my head keyed on
`node_value` being the tag; one `(intern_node 42 (list 7 9))` returned `0` and dissolved it. The
deserializer gave me the target for free (it IS the spec), the interface fell out of two call sites — the
easy 80% arrived fast — and the hard 20% (the node-model translation) was exactly the part I would have
gotten wrong by assumption. Slow isn't the tax on careful; it's what lets the cheap grounding pay for the
expensive mistake it prevents.

## Where discomfort turned to gold

The discomfort was wanting to show a working serializer this turn — to convert "yes, go" into a
committed artifact — and instead landing on "I don't yet understand the node translation." The pull was
to write it anyway on the plausible assumption. Witnessing the probe's `0` instead of `42` turned the
itch-to-ship into the thing that saves the ship: the gap I found is the gap that would have shipped as a
silent-wrong `.tbl`, corrupting the one byte contract everything depends on. Not-writing-it-yet is the
careful win.

## Corpus

Row 661 **propaedeutic** — preparatory study that precedes and enables the main work (fresh; this
turn's full grounding of the `.tbl` serializer — format, interface, node model, and the translation the
probe exposed — laid down before a single line of the serializer is written).
