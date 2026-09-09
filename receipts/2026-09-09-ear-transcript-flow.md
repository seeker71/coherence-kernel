# Transcript continuity and native worker ownership

Codex — 2026-09-09. The request was steadier, flowing live transcripts, with local
quality and latency attention. All changes remain Form/native; C is unchanged.

The production ear frame was sequence **281338**, age **70 ms**, with current
workers 12368/12370 under sensor 12362. OS parent and cwd observations also
showed six orphaned model workers in that same saved checkout: 5263/5264,
81513/81514, 93527/93528. Each pair wrote the same spool. A bounded correlated
framebuffer exchange selected their exact release; all six changed from
`host_alive=1` to `0`. The current pair and other checkouts were left alone.

The native owner lease now travels with each worker. Idle waits are bounded,
in-flight translation chunks check retirement, and retired recognition workers
do not append a successor-stopping `done`. Detached model-worker shell bells
and patience timers are removed. The older standalone ear viewer follows the
same spool with a bounded wait; its legacy launcher is not claimed migrated.

Glass retains the last text through quiet, preserves actual source age, accepts
corrections and rejects older source updates. A language keeps one five-row
display slot through growing/final and wrapping transitions. A corrected source
also releases its old forced translation prefix.

Checks, all exit zero: flow **65535**, meaning UI **8191**, transcript bytes
**1023**, ear axes **131071**, tongue **131071**, live UI **4294967295**.
Flow preflight: balanced, zero errors/warnings/unresolved. Effectful worker
preflight explicitly refused execution; it is not counted as a compile pass.
Actual worker invocation with a retired lease compiled and refused admission
before touching a model. Drift gates: **8191**, refused **0**.

An isolated temporary spool, with synthetic English and the existing local
Llama translation weights, exercised source revision and final translation.
The first cold open took **32235 ms**; the final cached repeat took **2066 ms**.
Nonempty translated text arrived in **361 ms**, and the corrected-source final
frame in **281 ms** in that repeat. Native
owner-retirement reached the worker's final stop line in **20 ms**, then its
parent reaped exit **0** without a fallback kill. These are bounded samples,
not a latency distribution or a before/after model benchmark.

Two checks improved because the observation disagreed: `kill(pid,0)` sees an
unreaped zombie as present, so cooperative exit is witnessed by the final stop
line plus actual child status, not that probe alone. An empty `t_pt=` field also
is not a translation; the final repeat requires nonempty translated bytes.
The model mistranslated one synthetic sentence (floor as wall); general
translation quality remains open, not hidden under a fluent stream.

The surprising teaching: a quiet text lane and a dead process need different
clocks. The duplicate-writer discomfort became explicit lifetime ownership,
while the model error kept the quality claim honest.

Live renewal then exposed a display-budget gap: five published languages in an
80×24 projection only showed three complete texts with five-row slots. The
follow-up makes slot height follow available body rows and language count,
orders languages by the shared catalog and keeps only the current original
language slot. Its flow band now returns **524287**; meaning UI remains **8191**
and live UI **4294967295**. No captured text was copied into this receipt.

Both changes landed on origin/main and fast-forwarded the saved live checkout.
After the second automatic renewal, old sensor 62328 and workers 62369/62370
were gone without manual release. New sensor 67047 owned workers 67052/67053;
renderer 67082 stood. Live ear frame **300132** was **55 ms** old, `stands=1`,
and all **four** current transcript texts were present in the production
80×24 projection. This observes native frame data and the production renderer's
projection, not human terminal pixels or long-term recognition accuracy.
