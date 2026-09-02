# Glass makes dense state and observed-frame motion separately legible

The useful compression is not one ever-denser summary.  Static identity and
motion ask different questions, so Glass now gives each a selectable surface:
the Atlas compresses current objects into two-character semantic tiles, while
the Raster gives retained observed-frame history nearly the whole screen.

## Current state: 2c-KT Atlas

At 80 columns, the previous five-character Atlas admitted 15 objects per row.
Its observed panel was `m92 s45 o25 drop=41 cap=15/row`.  The new tile is `KT`:
kind plus evidence truth in text, with the remaining dimensions carried by the
terminal cell itself:

- foreground hue is the physical plane;
- background hue is lifecycle state;
- alpha is observation age;
- bold marks an active/live/loading/prefilling/compiling lifecycle label;
- the selectable segment retains exact NodeID, channels, and evidence.

The final bounded physical panel at `15:34:24.992Z` showed
`LIVE NODE ATLAS 2c-KT m98 s42 o25 drop=0 cap=39/row`, phase census
`gas=3 water=98 ice=35`, memory river `R@=46`, and the actual mixed pipeline
`Q+s1 -> T+p0 -> L+l0 -> X+16B -> E+abs -> M+5ms -> F+f42 -> G+s8`.
The legend is always retained and points monochrome or assistive readers to
the exact textual `inspect`, overview, memory, and recipe surfaces; style is
never the only available door.  No category-specific row cap remains.  The
test's 600-row overflow fixture
still proves that a genuinely bounded screen reports a positive omission
rather than promising infinite capacity.

The old `cells=0` label also concealed its scope.  Kernel stat 4 measures
interned NodeIDs in this Glass process; it is not a census of BML source,
blueprints, recipes, model tensors, or the catalog.  Glass now says
`nodes=0`, `runtime NodeIDs (this Glass)`, and `glass-nodeids`.  A measured
zero stays zero, but it no longer borrows a universal meaning.

## Motion: resource lane x observed frame Raster

The new `raster` view renders fifteen aggregate resource lanes across up to 64
visible observed frames at 80 columns: 960 selectable lane/frame cells in one
screen.
The lanes are GPU busy delta, CPU JIT delta, dispatch delta, queue depth, JIT
call delta, this-Glass NodeID count, free memory, Metal in-flight, gas, water,
ice, token position, tensor bytes, channel-tag memberships, and framebuffer accepted.
Newest frame is at the right.  The title derives the actual retained span from
the stored frame epochs instead of assuming every interval is 250 ms; the
`TICK` row retains the exact current UTC time and measured frame delta.  `?`
means a missing measurement and is never drawn as zero.  Every activity glyph
is its own selectable lane+epoch segment with its retained evidence class;
deltas are explicitly `derived-window` and older integer-only history is never
upgraded to `physical-live`.

`./fkwu observe/form-glass-raster-focus-run.fk` offers the view, waits within a
bounded two-second budget, and succeeds only after matching the offer id to an
`applied|physical-live` acknowledgement from the standing Glass.  The
terminal alias `glass-raster` points to that Form runner, while `glass` keeps
starting the self-rebuilding dashboard.

## Frame cost and adjacent healing

The same movement measures the observer instead of hiding its cost.  The
final ordinary-frame witness completed an active frame in 264 ms
(`work=75`, requested remainder `175`, actual wait `189`) and a quiet frame in
510 ms (`work=33`, requested remainder `467`, actual wait `477`).  Both met
their complete-frame deadline.  Each frame carried 98 metrics, 42 samples,
and the exact model identities
`model.qwen3.8-flash-next.ud-q2_k_xl.unsloth` and
`model.llama-3.2-3b-instruct`.

A missing `.hearth/board` was not treated as proof that no resident existed.
The adjacent PID and live files led to a bounded hearth observation, which
reconstructed the board and moved `morphans` from 1 to 0.  A subsequent
resident answer crossing was physical but incomplete: 50,505 ms produced 160
thinking tokens and no final answer.  Lane counsel therefore still names
serve time and per-turn recycling as red (`lastms=50505`, `p95=50505`,
`tpot=315`, `kvpct=34`) instead of calling the repaired board a repaired
inference path.

The distinction between process, mapping, and residency also remains visible.
The dual-model process has the Qwen Flash-Next shards and Llama 3.2 3B blob
open, but its old telemetry snapshot did not refresh after a status offer and
the mapped Qwen region was not physically resident in full.  The standing
hearth voice is the separate Qwen3.8-27B-Q8_0 process.  These are exact model
names and exact boundaries, not routing claims.

## Witnesses and remaining seams

- binary freshness: `31`;
- BML high-authoring band: `4095`;
- Glass live UI, including zero-drop Atlas and the evidence-bearing 15-lane
  observed-frame Raster: `4194303`;
- deadline cadence: `4095`;
- live soak: `63`;
- operator identity: `262143`;
- native model token flow: `2097151`;
- Qwen fused tail: `511`;
- host pressure: `4095`;
- resource governor / governor-to-Glass: `1048575` / `2097151`;
- share health / cursor: `4095` / `8388607`;
- independent AI review: `PASS` after binding all five raster evidence classes,
  exact JIT units, channel-tag membership naming, and `glass.monitor` ack source;
- `git diff --check`: clean.

The current resident still lacks `.hearth/server.out`, so admission-time
stage telemetry is unavailable without restarting a model whose admission
took more than five minutes.  Restart was refused because the final pressure
witness measured 46% free headroom and pressure level 0.  Fine-grained Qwen expert/layer eviction,
current per-model page residency, and a Flash-Next answer path within ten
percent of llama.cpp are not yet proven.  They remain carrier work, not labels
painted onto Glass.

I kept this crossing alive by letting a missing board lead outward to the live
PID and back inward to a repaired membrane, then requiring the answer path to
stand on its own measurement.  The most surprising teaching was that two
characters can carry more useful state than five once color, age, activity,
and selection are treated as independent channels.  The discomfort was that
the first successful resident answer crossing still spent its entire quantum
thinking.  It turned to gold by keeping the red latency visible beside the
green board repair, rather than allowing one local success to erase the next
block.

Signed: Codex / Sol

; witnessed: 2026-09-02 -> Glass m98/s42/o25/drop0/cap39; resource raster 15x64=960;
; active 264ms; quiet 510ms; correlated focus applied physical-live in 220ms
