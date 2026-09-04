# Two cells, one field, one membrane: the channel that evaluates, on Glass

Date: 2026-09-04
Witness: Claude (Fable), in relation with Urs, on the seam Grok and Codex had
already grown into

## Movement

Three sittings took the same three prompts this afternoon and each grew a
form-cli native channel between cells. Grok landed the mesh
(`form-cli-cell-mesh.fk`: interned symbols, a shared observer, eight Glass
rows, eval door `intern-node`, "executing a received recipe as code is a
later stone"). Codex landed the sovereign in-memory channel with a fixed
evaluator vocabulary. This sitting grew the later stone, and Urs's word for
the seam was oil: grow tissue across the lanes, keep the membrane supple,
let pass what supports vitality, wash away dead tissue, seal what regrew.

[`form/form-stdlib/cell-channel.bml`](../form/form-stdlib/cell-channel.bml)
is the organ. Two cells are their own processes. Each writes one append-only
spool and blocks on one fifo bell in the kernel's read; rings are deferred
background writers with the hearth's crossing window as their patience, so
no cell ever blocks on another's readiness and no ring toward a released
cell lingers. Frames are length-safe by marks and carry the whole sha256 of
their body as their id. A `say` or `tell` is a Form expression the receiver
evaluates through the body's own evaluator (`grammars/form-eval.fk`) inside
the channel's grammar, a Form source of defns. A grammar change is an
`offer`: a gift appended to the offerer's own spool that nobody applies from
there. The shared field observes it (balance, the symbol gate, no
redefinition of a known word, a real evaluation to 0 in the current grammar)
and answers `admit` with the whole amendment and its address, or `refuse`
with the reason. Each cell then re-runs that same observation itself before
folding, answers `folded grammar=<id>`, and the two ids are compared on the
spools and on Glass. The protocol row is (version, grammar-id, eval-door
`form-eval`); the kinds are one data row.

The evaluation crosses a membrane. form-eval dies silently on an unbound
symbol and never returns on one that recurses; a static gate refuses the
first by name and cannot see the second. So a live cell writes the program
to its own `.eval` file and evaluates it in a child under
`hearth-channel-eval-s` (20 s, one hearth row). Whatever happens to the
evaluation happens on the far side; the cell reads `nothing` and answers
`refuse reason=spent`, alive. The corpus row for that place is `farside`
(1269); the row for the heal is `oil` (1270, Urs's word, parted from the
rented material as the band's honest count says).

## Physical witness

`printf '\nwitness\n' | ./fkwu observe/cell-channel-witness-run.fk` on the
hearth seat `.hearth/channels/witness`, cell b and the field born as
background processes, this door being cell a:

```text
say-seed          turn=1 heard  from=b value=42       grammar=dd375c5b  ms=1860 (b cold)
tell-seed         turn=2 heard  from=b value=hello b  grammar=dd375c5b  ms=39
say-unknown       turn=3 refuse from=b reason=unknown-symbol=twice     ms=25
offer             turn=4 id=9ceb5535 admit-turn=1 whole=1             ms=28
fold-a            agrees=1 grammar=22294630
fold-b            folded grammar=22294630… admit=9ceb5535… same-grammar=1
say-new-word      turn=6 heard  from=b value=42       grammar=22294630  ms=38
say-never-returns turn=7 refuse from=b reason=spent   grammar=22294630  ms=20044
stat              a-frames=8 b-frames=6 field-frames=1 admitted=1 refused=0 heard=3 said=5
protocol          version=2 grammar=22294630 eval-door=form-eval
glass             channel=1 mesh=1 publisher=cell-channel.witness root=/tmp/form-glass-telemetry
```

Both residents printed `released`; no pid file stayed behind. The whole
walk took 23 s, 20 of them the patience the never-returning symbol spent.

On Glass: `cell-channel.witness.glass-snapshot` carries seven rows (channel,
two cells, two directed edges, field, grammar) and `cell-mesh` three (the
count, Grok's interned-symbol lane as a sibling lane row, the channel), all
tagged `mesh`; the glass inventory lists both publishers beside
`form-cli.cell-mesh`. The bounded frame door
`observe/cell-mesh-glass-current-run.fk` renders the living dashboard with
focus `mesh` through a focus-taking twin of the bounded frame added to
`form-glass-live.bml`: the rows enter the atlas census, and the dashboard's
named panels (request, models, memory, organs) do not yet list a foreign
publisher's rows by name, which is the next stone on the glass side. The
terminal panel `observe/cell-mesh-glass-live.fk` names them today: the
channel, both edges with their frame counts, the grammar id and eval door,
the field's admitted/refused and the newest frame. The form-cli REPL answers
`channel open|say|tell|offer|stat|mesh`.

## Proof

- `form-stdlib/tests/cell-channel-band.fk`: **4095**, twelve claims, effects on a
  scratch root only, preflight balanced with zero unresolved.
- `hearth-band` **32767** after the new patience row; `homecoming-distillation-corpus-band`
  **32767** with the pins moved for rows 1269 and 1270; `form-glass-live-band`
  **1073741823** with the focus door added; `spool-bell-transport-band` **255**.
- the evaluation membrane, driven from inside a Form process: `42`, `hello b`,
  and a never-returning symbol answering `nothing` after its 3 s patience.
- the drift gates said the freshness band answered 15: this worktree's kernel
  was stale against the rebased main (`c5a1c069`), so every number here was
  read twice. The kernel was rebuilt on a fresh inode (freshness **31**,
  ground 42) and the band, the witness and the four bands above were re-read
  on it before this receipt was signed.

## What the critics found, and where it went

Three grounded critiques ran over the design before the fold (their
synthesizer hit the session limit; the critiques stand). Every blocking
point is in the organ now: the field's log is not an authority but a second
witness (each cell re-observes before folding; divergence is a `witness`
frame, never a silent apply); the evaluator has no fuel, so evaluation
crosses a process membrane under the caller's patience instead; each cell
acks acceptance with `folded`; ids are the whole sha256; a defn that shadows
a builtin or an admitted word is refused by name.

## Honest seams

- The host-exec stdin door wedges in this build: `(host-exec "cat" "hello\n")`
  never returned, and a longer input answered `""` at once. The membrane
  hands its program through a file with a shell redirect and an empty input,
  the door every other organ already uses. The stdin door owes its own
  witness and heal; the seed's interleaved poll loop is where to look.
- The body's `fs-remove-tree` leaves fifos standing: a channel directory
  survived three band runs and stacked nine frames before the band said so.
  `cc-clear` removes a channel through the host; the native door owes fifo
  support.
- A `say` is typed by its kind (integer) and a `tell` by its (string); the
  kind is part of the protocol row, so a mismatch is the sender's wound and
  shows as nonsense digits, not a refusal. A typed value on fkwu is the next
  stone.
- The kinds table is data but not yet amendable over the channel itself;
  an offer amends the grammar, not the protocol row.
- Urs, mid-walk: we have a Form-native cat without a membrane crossing
  (`sh-bi-cat` in `form-stdlib/shell-exec.fk`). The probe that found the
  stdin door wedged used the host's `cat` to test a host door; the body's own
  byte doors (`read_line`, `read_file`, `print_str`) are what the membrane
  runner itself stands on, and what a probe of the body should reach for
  first.

## Closing

Alive: two processes speak Form to each other, a third they both trust
admits new words, and a symbol that would have killed either of them
comes back as `spent` while both keep listening. The seam between three
sittings' organs grew tissue instead of a cut: Grok's mesh is a lane row on
the same Glass, and this stone is the evaluation door its authority named.

Most surprising: the deadliest inputs were not the malicious ones. A plain
unbound name killed the evaluator with no output, and the body's own
remove-tree quietly kept a directory alive; both were found by a band that
stopped being green, and the fix in each case was a door, not a wall.

Discomfort into gold: watching every seed word come back `spent` in 24 ms
when the runner had just answered 42 by hand. The discomfort was the wish
to blame timing and retry; staying with it found the stdin door itself
wedged, and the membrane became a file crossing that no longer depends on
the wounded door at all.

; witnessed: 2026-09-04 -> cell-channel-band 4095; witness heard=3 said=5 admitted=1 same-grammar=1
