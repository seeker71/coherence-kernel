# Glass keyboard contract

Glass owns immediate input in its existing renderer process. No control command,
newline, shell, or helper process is needed for a view change.

The carrier wakes the microphone by default. A fresh renderer opens the plain-language
overview; a chosen view survives restarts. Transcripts keep `all` published languages selected. `z` closes the ear and keeps
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

Exact metric selectors include the domain, for example
`metric.metal.allocated-bytes`. Legacy
`metric.blueprint-bytes` still resolves. Interaction keys are textual channels,
not newly minted projection nodes: many readings intentionally share one
projection shape. Sample selectors retain publisher/lane/id; typed observations
use their published ID, for example `recipes.allocation.bytes`.

## Native inspection

Glass reads published native frames and renders all twelve views at 80×24 and
160×40. An opening row alone is not a completed audit. Missing source doors and
silent source ages follow. Inspection does not request an owner
refresh, publish a competing Glass sensor frame, refresh a governor, or admit a
model. Rows keep their own source clock when present. An unbound row clock uses the
carried frame's publication epoch/sequence, so its displayed age is publication
age, not an inferred capture time; re-reading does not renew that epoch.

Memory panels (accounting in `form/form-stdlib/bml/glass-memory-accounting.bml`)
render narrow and wide from this inspecting process, native host VM counters, and
published owner data. Typed local measurements say `glass.probe`, not `glass.monitor`.
Column bytes are logical occupancy, not RSS: cell columns 104 bytes, identity
quartets 32 (a subset, not additive), recipe columns 80, cons pairs 16.
Host page bytes, host reclaimable headroom, owner buffer extents and process RSS
remain separate scopes. Handles are not byte budgets. Owner ledgers replace
overlapping model-root totals; leaves are not counted again. Derived extents
remain derived. Missing budgets stay unknown.

Current-process tensor bytes are live owned buffer extents, excluding mapped
views; they are neither process RSS nor device allocation. The separate
`tensor.owner.allocated` door requires an owner-bound byte publication; a local
inspection does not create one. Silent microphone/awareness
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
`3` report active (1/0), `0` restore and answer the read-back: 1 when the
terminal reads back as saved, 0 when it differs. Acquisition reports 0 on a
non-TTY or unsupported host and `nothing` on an operational error, including a
terminal that does not read back in direct mode. Read returns a string (empty
when quiet), or `nothing` on loss/error. The POSIX carrier preserves signal
handling and output processing; exit, interrupt, termination and hangup restore
saved settings. Suspension restores settings and resume reacquires them. Every
restoration, the signal and suspension arms included, reads the terminal back
against the saved state; Darwin's PENDIN queue bit is left out. SIGKILL cannot
run cleanup. Windows currently reports unavailable.

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

- `form/form-stdlib/tests/form-glass-input-pty-run.fk`: real terminal acquisition
  and the production Glass decoder and controller; a quit prints `RESTORED` only
  when the kernel reads the terminal back as saved.
- `form/form-stdlib/bml/native-pty-darwin.bml` emits its ARM64 syscall image in Form and admits it
  in RAM through the existing CPU JIT door. Mutable ioctl outputs live in owned
  anonymous memory; borrowed strings remain immutable. One nonblocking owner
  polls and reads terminal output in chunks up to 65,536 bytes. A chunk size is
  not a transcript limit. These are cooperative process-local owners, not
  protected address spaces. This acceptance module does not replace the
  production terminal signal hooks in C.
- The door's own read-back is the live check on every POSIX host: a direct-mode
  acquisition that does not read back as set answers `nothing`, and a close
  answers whether the terminal read back as saved. The production fixture prints
  `RESTORED` only on that answer, so the PTY sessions witness the kernel's own
  reading beside their external termios comparison. The native RAM admission
  carrier is Darwin arm64 and the Form door runs on that target only, so no
  Linux PTY session result is claimed.
- `form-glass-live-band.fk`, `form-glass-launch-band.fk` and
  `form-glass-event-loop-band.fk` retain the full renderer/launcher proofs.

After a successful direct input read, `glass.last.input-state` carries a
native `glass-input-v1` observation: renderer PID, epoch, view, language filter,
selected public identity, last control ID, terminal-active flag and byte count.
It carries neither raw keys nor partially typed/pasted text.

`o` explains current published work, request queue, local model readiness,
listening, host and model RAM, dispatch/recipe totals over currently live kernels, and visibility
limits. Wide terminals put current work, missing sources, change events and
retained work beside this orientation. Retained work is newest first and never
counted as current activity. The awareness window names rows and publishers
outside its current reading; it is not whole-system coverage. Chat intent and
effects without a publication remain unknown. Arrow/Tab selection and `e` keep
the underlying source, age and evidence available.

The overview uses blue for the selected field and red for a named attention
item. A native work heading and its state/age select together. Activity and
freshness remain words, without independently changing background emphasis.
Terminal columns account for UTF-8 text, combining marks and wide characters.

An observed absent event is not a failed sensor: no open line, recognition pass
or translation response has its own reason and source clock. Required fields
missing from an actual response remain unavailable. Coverage counts absent
events separately from missing and stale readings; `e` preserves the distinction.

Attention has an executing owner. Before sensors start, the launcher offers a
missing or stale native carrier to the existing checkout compiler, retains its
build evidence and re-observes the artifact through native care. Governor
publication runs in the sensor process as a resident cell. A failure retains
its diagnostic and an unclaimed source-repair need; unchanged failure is held,
and a governor, imported source, floor compiler or carrier change resumes it.
A successful publication correlates its fresh health with the original failure.
An optional governor failure leaves the rest of Glass running. Unconfigured
workload, reserve and grant contracts remain unknown; inspection invents no
grant and acquires no governor slot.

`NATIVE CARE / ACTION AND RECHECK` shows the latest three retained care readings
from this checkout's `.hearth/organ-health.jsonl`, including unclaimed needs,
selected responses, applied work awaiting observation and re-observed health.
The existing sensor retains the native care reader and its byte cursor; source
backlog or missing evidence remains visible. This is an observation of the
executing owners, and does not itself run repairs. `v` includes all published
care readings; `e` exposes signal identity, correlation, responder and source
state. The agent's `glass-read-run.bml` exposes the same public care metadata.
Private need details, diagnostic contents and action results stay with the owner.

The microphone lane publishes capture time and level before decoder work. This
heartbeat neither carries audio bytes nor erases the last semantic quiet/open
line. Captured digital silence remains silence. If a born lane answers but no
capture arrives, the ear owns a `host-microphone-audio` continuation; it waits
instead of reopening that same empty stream. Actual capture, including a
decoded environmental/refusal frame, or explicit sleep re-observes the need.
The pending identity survives sensor restart through the native care reader.

Model ownership has separate scopes. The overview reads current admission notes
through `adm-observe-residents`, binds their write epoch to each live PID's
process start and shows admitted model files plus per-process RSS. Notes can
include an open in progress; they establish neither completed loading nor active
decoding. This read does not sweep the registry. Direct Whisper and other paths
outside session admission retain their own observations, including the ear's
worker PIDs.

The Qwen Flash Next + Llama 3B dual resident is an optional token-flow seed with
fixed startup prompts. It is separate from native answer/code/ear sessions.
When its owner has never published, Glass names that optional absence instead
of demanding it as a missing runtime service. Once an owner was published,
missing/stale/mismatched ownership remains a route gap. Bounded inspection
projects those same conditional route rows as the live renderer.

The current snapshot carrier serializes the complete value through
`node_gift_write` and reconstructs it through `node_gift_read`. Shared memory
here does not mean that the snapshot crosses as a shared root ID. Native
interned cells already have shared identity; `node_at` can recover that same
cell and its children. Moving snapshots to root references also needs an
owner/generation binding and retirement after readers release them: the
current shared field retains admitted nodes and pairs. The serialization crash
repair establishes memory safety, not completion of that transport change.

The same published samples are available to an agent through
`observe/glass-read-run.bml` (optional publisher prefix on stdin). Its output
keeps identity, owner, lane, purpose, reported state, value, bytes, parent,
timestamps, source, evidence, selector and channels. `transport=current` means
the frame was readable; the separate last-publication age never establishes
that its owner is still alive.

An arriving agent publishes public progress through the existing work owner:

```sh
form-run ./fkwu observe/rented-work-run.bml <<'EOF'
{"action":"activity","session":"glass-care","contributor":"Codex","purpose":"Make Glass understandable","step":"Verify the live overview","completed":"Added source ages","needs":"Read the updated frame","status":"working"}
EOF
```

Use `working`, `waiting` or `complete` at meaningful transitions. `purpose`,
`step`, `completed` and `needs` are explicitly public summaries, never copied
prompts or answers. The work owner retains the latest record under
`.hearth/rented-work/` and publishes the same native cell to Glass. Its
`declared` evidence and update age stay visible after its publishing process
exits. Measured usage keeps its own begin/mark/read/close lifecycle and is not
inferred from these progress declarations.
