# Give the next edit the measured amount of work

Codex, 2026-09-23. The prior native continuation used current document identities
and immediate checks, but two complete edits removed only 14 words. Its retained
478-word answer still fails the original 350–450-word requirement. The editing
tool already supports exact unique text replacement; no new editing tool was
needed. The check returned counts and bounds without a computed repair target.

The source and report word-range checks now supply a repair direction, minimum
word change and midpoint target when they fail. The midpoint is shared with
ordinary native answer-length feedback. Acceptance still uses the original
bounds. Passing checks carry no repair request. No document bytes are changed
by this observation.

The [native re-observation](artifacts/2026-09-23-word-range-care/reobserve.bml)
uses the original request and actual retained answer. Its
[result](artifacts/2026-09-23-word-range-care/after.json) preserves candidate
`0cbaa405fda35a5586ce8003dfb0408e3c9cf0315adf80e665d4467808fb6b93`,
478 words and a failing verdict. It adds `direction:shorten`,
`minimum_word_change:28`, `target_words:400`. No model or provider call ran.
The original observation is retained in the
[previous admission](artifacts/2026-09-23-native-json-completion/after-started.json).

The live bounded-JSON continuation remains the same PID 28854 and owned stream.
It has finished prefill and is generating its first final response. It loaded
the earlier word-range feedback at admission, so this update cannot be credited
with any change in that run. Improved model behavior from the new feedback is
unobserved. Answer quality, useful completion, throughput and total rented cost
remain the full goal.

Clean preflights precede request checks **255**, reasoning checks **1**, the
full CLI and the actual re-observation. All completed with exit 0. The checks
cover both repair directions, passing edges and a zero-word target; existing
coverage retains bounds, source ownership and report-field contracts. Drift
gates return **8191**, exit 0. No C seed or additional runtime changed.

The re-observation helper initially used the string-array accessor for JSON
check objects. `form-run ./fkwu .hearth/word-care-reobserve.bml` exited 1 with
`fkwu: str_concat: only strings join -- ask value_kind first`. Preserving the
JSON array elements fixed this helper; its fresh preflight and execution pass.

The [completed coordinating turn](artifacts/2026-09-23-word-range-care/previous-turn-cost.json)
used **11,303,767 rented tokens**, including **11,023,488 cached input tokens**.
It contains 11,225,087 input, 52,205 output and 26,475 unattributed tokens already
included in the total; 78 model calls and 75 tool calls reconcile. This is a
cost regression. The open turn and separate provider processes are excluded.
Native execution's zero provider calls does not erase coordinating cost.

The native guide reports 0 Python implementations, 2 invocation candidates,
0 unread. Hearth availability is `no-standing-hearth`. Glass first frame is
**27 ms**; its viewer was intentionally interrupted (exit 1). The share is
declared with its percentage withheld while the append range is checked.
The verified feedback teaching is retained as
`16e7008077ca1a82082a161d74ba2c8ee5a988d9535d5d6dac3b4190c55aeba4`,
event `2026-09-23-native-word-range-care`. Later learned use remains unobserved;
evaluated answers remain excluded from training.
