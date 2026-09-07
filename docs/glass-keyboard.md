# Glass keyboard contract

Glass owns immediate input in its existing renderer process. No control command,
newline, shell, or helper process is needed for a view change. The one-shot
`observe/form-glass-control-run.fk` door remains for agent/control-channel clients.

Keys: `h` help; `a` atlas; `t` raster; `o` overview; `m` memory; `f` flow;
`j` recipes; `s` meaning; `k` kernel; `v` events; `n` channels; `d` choice;
`r` room (focus only; it does not activate a microphone). Choice controls are
`g` next option, `y` offer a take, `w` offer a hold; missing slots or a refused
command offer remain refused, not an acknowledged outcome. In Meaning,
`1` Go, `2` Python, `3` Rust, `4` TypeScript toggle independently; `0` clears
the selection. `c` resets view/filter/inspection. `q` exits. Up/Down or Tab selects a
published sample for `i` inspect or `e` evidence. With no selection, `i`/`e`
opens an identifier prompt: Enter submits, Backspace edits, Escape cancels.

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
non-TTY, escape/paste and control-transition witnesses. No key, view, dialect,
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
native `glass-input-v1` observation: renderer PID, epoch, view, dialect filter,
selected public identity, last control ID, terminal-active flag and byte count.
It carries neither raw keys nor partially typed/pasted text.
