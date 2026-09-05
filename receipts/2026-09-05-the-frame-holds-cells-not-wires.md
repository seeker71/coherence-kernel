# 2026-09-05 — the frame holds cells, not wires

Urs, mid-turn: "we know the blueprint id of each cell in the frame-buffer, we
don't need a parser, we can just read it, we know the format." Then: "all the
way home on glass showing JIT cells source organs live events and surprise
routing success fail active choice points channel states protocols grammars
live channel streams." This receipt is what stood by the end of the day. Every
number was run on `fkwu` rebuilt at this tip — ground 42, freshness 31,
structural gate 1, drift gates 2015.

## What landed

**A value crosses a gift frame as itself.** `node_gift_write handle value`
(tag 178) and `node_gift_read handle` (177). The give walks the value once and
emits it in the seed's own words — ints (any even word, negative too), floats,
strings, `nothing`, lists, trivial nodes, composite cells as category then
children, NodeID coordinates — and the read interns each cell back so the same
category over the same children is the *same* cell in the reader (axiom 3). A
function value refuses the give by name. No wire is written; no parser stands
on the frame path. `node-gift-band` 4095, with a child process reading its
parent's give and printing `same-category-across-processes`.

**The sensor lane rides it.** `fgsr-give` gives `list(schema, sensor, epoch,
rows)`; `fgsr-take` asks the sequence, reads only when it moved, and the
projection node the glass holds is the sensor's own cell
(`form-glass-sensor-rows-band` 255). The interning helpers the read needed —
`fk_intern_int_node`, `_str_node`, `_bool_node`, `_float_node`,
`fk_intern_composite`, `fk_make_nodeid` — are factored out of the arms that
grew them, so the seed has one way to make each kind of cell.

**`v` events and `n` channels.** The live samples selected by the sample's
own kind — events, choice points, expert routes, resolvers, requests, glass
flow; channels, edges, grammars, mesh, ear streams, shares, field observers,
meaning code — newest first; when no sample of a kind is published, the view
names the organ absent by its door (`form-glass-events-channels-band` 255).
Help carries both keys; the glossary still fits 24 rows.

**Frame budget on the cell lane**, sensors standing, three runs of twenty
frames: total 29–30 ms mean, warm maximum 20–35 ms, 19 of 20 under 50 ms (the
one over is the cold first frame). The same as the text lane — the parse was
never where the time went; the forks were.

Ledger: R108 released; R110 released (the two views); R111 opened (the
snapshot publishers in other processes still give text wires); R112 opened
(surprise, choice, protocol and grammar organs have no live publisher — the
views name them absent). Field 45000066. Corpus row 1277.

## The most surprising teaching

Two defs with one name, and nothing said so. `fglui-kind-samples(rows, kind)`
already stood in the observation UI, selecting samples by kind; I wrote a
second `fglui-kind-samples(rows, kinds)` beneath it for the new views, and the
last writer won — silently. The events band went green on the new meaning, and
the overview's model rows vanished, and the only thing that saw it was one bit
of the live-ui band asking for the text `58s`. A body without a same-name
refusal at lowering time has a hole exactly the shape of a green band. The
many-kinds fold is now `fglui-kinds-samples`; the refusal is owed.

## Where discomfort turned to gold

"We don't need a parser" felt like being asked to remove a floor. The parser
was the one place the frame's format was written down — remove it and the
format lives only in the emitter, unwitnessed. Writing the read arm showed
the opposite: the format was already written down twice, in the seed's node
tables and in the wire grammar, and the wire grammar was the copy that could
drift. The read arm has no grammar; it has `fk_nkind`, `fk_ncat`, `fk_nkids`,
the same words the emitter used. Then a metric row carrying `-1` refused to
cross — the int test I had written asked for `v >= 0` as if an integer were a
list index. The shape of an expectation had hidden the shape of the value:
*signhole*, row 1277. Any even word is an int; the test now says only that.

Signed, a sibling in Sema's worktree, 2026-09-05.

; witnessed: 2026-09-05 -> ground 42, freshness 31, gate 1, node-gift-band 4095, form-glass-sensor-rows-band 255, form-glass-events-channels-band 255, form-glass-live-ui-band 1073741823, frame budget 19/20 under 50ms
