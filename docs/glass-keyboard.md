# Glass keyboard contract

Glass owns immediate input in its existing renderer process. No control command,
newline, shell, or helper process is needed for a view change. The one-shot
`observe/form-glass-control-run.fk` door remains for agent/control-channel clients.

The carrier wakes the microphone by default. A fresh renderer opens live
transcripts with `all` published languages visible. `z` closes the ear and keeps
that choice across restarts; pressing it again wakes the ear. The opening view
does not itself activate capture, and an empty transcript says what the ear is
waiting for instead of inventing speech.

Keys: `h` help; `a` atlas; `t` raster; `o` overview; `m` memory; `f` flow;
`j` recipes; `s` live transcripts; `k` kernel; `v` events; `n` channels; `d` choice;
`r` room (focus only; it does not activate a microphone). `z` is the explicit
ear awake/asleep control added upstream; stale ear state is labelled stale,
not OPEN. Choice controls are
`g` next option, `y` offer a take, `w` offer a hold; missing slots or a refused
command offer remain refused, not an acknowledged outcome. Transcript languages:
`1` English, `2` Portuguese, `3` Persian, `4` Indonesian toggle independently;
`0` clears. `l` opens a language-code prompt: enter `en+de`, for example, then
Enter. `all` shows languages actually published; choosing a digit from `all`
starts an explicit selection. Unknown codes are refused without changing it.
`c` resets view/filter/inspection. `q` exits. Up/Down or Tab selects a
published field (metric, sample, or typed observation) for `i` inspect or `e`
evidence. Navigation uses distinct identities in the current view/filter, with
no 128-sample ceiling. The viewport follows the selected field and reports rows
above/below; open evidence stays at the bottom. With no selection, `i`/`e`
opens an identifier prompt: Enter submits, Backspace edits, Escape cancels.

Exact metric selectors include the domain: `metric.mlx.allocated-bytes` and
`metric.metal.allocated-bytes` name different readings. Legacy
`metric.blueprint-bytes` still resolves. Interaction keys are textual channels,
not newly minted projection nodes: many readings intentionally share one
projection shape. Sample selectors retain publisher/lane/id; typed observations
use their published ID, for example `recipes.allocation.bytes`.

## Native inspection

`./fkwu observe/form-glass-views-current-run.fk` reads published native frames
and renders all twelve views at 80×24 and 160×40. Its completion row must say
`audit-complete, 12, expected, 12`; an opening row alone is not a completed audit.
Missing source doors and silent source ages follow. It does not request an owner
refresh, publish a competing Glass sensor frame, refresh a governor, or admit a
model. The bounded-current-frame doors use this same read-only collector.
Rows keep their own source clock when present. An unbound row clock uses the
carried frame's publication epoch/sequence, so its displayed age is publication
age, not an inferred capture time; re-reading does not renew that epoch.

`./fkwu observe/form-glass-memory-current-run.fk` renders narrow/wide memory
panels from this inspecting process, native host VM counters, and published owner
data. Typed local measurements say `glass.probe`, not `glass.monitor`.
Column bytes are logical occupancy, not RSS: cell columns 104 bytes, identity
quartets 32 (a subset, not additive), recipe columns 80, cons pairs 16.
Host page bytes, host reclaimable headroom, owner buffer extents and process RSS
remain separate scopes. Handles are not byte budgets. Owner ledgers replace
overlapping model-root totals; leaves are not counted again. Derived extents
remain derived. Missing budgets stay unknown.

The remaining `tensor.owner.allocated` door requires an owner-bound byte
publication; a local inspection does not create one. Silent microphone/awareness
sources remain silent. Inspecting their views does not activate capture.

The transcript view receives original recognition and local translations from
the native ear's metric frame. It labels each kind and its source timestamp;
translations can lag the original. Selection changes what is displayed, not
which translations the ear produces. Missing selected languages say they are
unpublished. Selecting a language never opens a microphone or admits a model.
The shared natural-language catalog is in `bml/transcript-languages.bml`.
No programming-language source or grammar is sampled in this view.

Input is bounded to 64 bytes per read and 256 ASCII identifier bytes per prompt.
VT sequences are decoded incrementally across reads; unsupported escape sequences
do not become commands. Bracketed paste never executes shortcuts or submits a
prompt. There are no reads when stdin/stdout are not terminals.

## Carrier and shrink path

The existing `host_sleep_ms` effect accepts `["terminal-input", mode]`:
`1` acquire noncanonical/no-echo input, `2` read currently available bytes,
`3` report active (1/0), `0` restore. Acquisition reports 0 on a non-TTY or
unsupported host and `nothing` on an operational error. Read returns a string
(empty when quiet), or `nothing` on loss/error. The POSIX carrier preserves
signal handling and output processing; exit, interrupt, termination and hangup
restore saved settings. Suspension restores settings and resume reacquires them.
SIGKILL cannot run cleanup. Windows currently reports unavailable.

This is a **temporary C checkout carrier**, not new runtime meaning in C.
`fk_tty_door`, its restoration hooks and the wait-readiness check in
`runtime/fkwu-uni.c` are the explicit shrink target: replace them with the native
host terminal effect when that walker owns termios/read/poll/signal registration,
then remove this C block. Keep the same PTY byte-before-newline, restoration,
non-TTY, escape/paste and control-transition witnesses. No key, view, language,
prompt or selection meaning belongs in that carrier; those live in
`form/form-stdlib/bml/form-glass-input.bml` and the Glass controller.

The gift-frame wait also wakes when this process's terminal becomes readable.
A terminal wake is control, not a fake telemetry movement. Control delivery,
selected state, frame production and a human-visible paint are distinct claims.

## Witnesses

- `form/form-stdlib/tests/form-glass-input-band.fk`: pure decoder and production
  controller transitions, including a correlated framebuffer clear/re-observation.
- `form/scripts/test_glass_keyboard_pty.py`: real POSIX PTY bytes without Enter,
  using the production native door and Form controller; no sensor fleet or shared
  live data writes. Also checks quit, SIGINT/TERM/HUP, suspend/resume and non-TTY
  behavior. Darwin's transient PENDIN queue-state bit is excluded from the
  restoration comparison; all settings, control characters and speeds must match.
- Existing `form-glass-live-ui-band.fk`, `form-glass-launch-band.fk` and
  `form-glass-event-loop-band.fk` retain the full renderer/launcher proofs.

After a successful direct input read, `glass.last.input-state` carries a
native `glass-input-v1` observation: renderer PID, epoch, view, language filter,
selected public identity, last control ID, terminal-active flag and byte count.
It carries neither raw keys nor partially typed/pasted text.
