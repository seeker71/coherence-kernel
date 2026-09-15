# The compiler asks for its binding

2026-09-15, M4 Max, Hati Suci. Urs's direction for the day: bands move into direct code; checks
live in the code that does the work, send signals when warnings and errors happen, and are handled
and healed live. The witness is live runs of real cells, not a fixture tally. No band file was added.

## Carried

- **The kernel sends the signal.** `runtime/fkwu-uni.c` 18186-18528 (`fk_sig_*`, `fk_diag_signal`,
  `fk_diag_path_signal`, `fk_sig_heal_settle`), called from `fk_diag` (18551, 18578), from
  `fk_diag_path` (20808) and after a rebuilt image is written (23358). After the human lines, which
  are unchanged, each printed diagnostic sends one `form-organ health` organ-health-v1 row: organ
  `fkwu-compiler`, flow the pid, aspect `diagnostic`. A missing binding asks for the resource
  `binding`, detail the name, and names its form (`call` or `name`) apart from the name. Any other
  error asks for `source-diagnostics` with the message. A warning is a reading of unknown health.
  The line never spells the markers the human lines' readers count; the string writer \u-escapes
  the byte that would complete one. A quiet compile sends nothing. From a real cell's compile:

  ```
  form-organ health {"schema":"organ-health-v1","id":"97843:1789453733444:1","organ":"fkwu-compiler","flow":"97843","aspect":"diagnostic","stage":"observe","expected":"clean","observed":"binding-missing","health":0,"surprise":1,"needs":[{"resource":"binding","detail":"tb-depth-loop"}],"offers":["request-evidence","revise"],"selected":"","evidence":{"line":281,"col":26,"form":"call","name":"tb-depth-loop","unit":"observe/tree-heal-witness.fk"},"observed_at_ms":1789453733444,"at_ms":1789453733444}
  ```

- **Care the kernel already gives is visible.** A `.fkb` set aside to rebuild from source sends
  its reading with aspect `image:<path>`, offering `rebuild`. Once a rebuilt image is written, the
  applied row follows under the same id, with `result {"image":"rebuilt"}`. The freshness band's
  own run sent both rows, 10 ms apart.
- **It is handled live.** The native process runner records these rows as the child runs. A cell
  that sleeps 2.5 s after its compile had its `binding-missing` row recorded at 61 ms. The child was
  admitted at 64 ms and reaped at 2544 ms, and no protocol-pain row was written.
- **It is healed live.** `form/form-stdlib/bml/fkwu-binding-care.bml` is a provider over
  `organ-care.bml` for the need `binding`. It uses preflight's `pf-classify` and a new all-homes
  search, `pfs-defn-homes` (`preflight-source.bml` 186). When the name is UNPRELUDED and exactly
  one unit defines it, that unit joins the cell's preludes line. The edit stays only when a fresh
  compile shows the binding gone and no error signal that was not there before. Otherwise the bytes
  go back exactly, and the restored cell is compiled again. Each outcome is voiced as a fresh
  reading, care_of the first. The runner offers the provider when a job's snapshot asks for
  `binding` care (`form-cli-heal-native-process.bml` 254, 285). The door
  `observe/form-cli-heal-process-run.bml` asks for it and runs the argv again after a kept edit.

## Witnessed

- `binary-freshness-band` 31 on the rebuilt kernel. Both kernels ran the same probe cell, each
  reading images it wrote itself. Human stderr was byte-identical (`cmp`), stdout identical, and
  validate.sh's `fk_diag_count` expression read 2 on both. The 2 signal lines hold none of the
  counted markers.
- Preflight pages were identical under the old and the new kernel. On a copy of `tree-heal.fk`
  without `observe/tree-balance.fk`: 3 errors, 3 unresolved, each UNPRELUDED in tree-balance. On
  a copy with `tb-depth` misspelled: 1 error, `tb-depht` TYPO.
- The heal door on the unpreluded copy: status 1, then rerun status 0. `tb-depth-loop` was
  delivered with the edit kept (`observe/tree-balance.fk` back on the preludes line). `tb-depth`
  and `tb-list` were settled by observation. Three fresh readings came back clean, health 1, each
  care_of its first reading. The rerun's stderr was empty.
- The heal door on a copy of `bml-compiler-health.bml` with its preludes line removed took all
  three branches in one run. `int_to_str`: portable, need left open. `json-node-object`:
  UNPRELUDED in `json.fk`; the edit was restored (`new-error-appeared`), and the restored cell was
  re-observed with 17 error signals. `oh-reading`: `form-stdlib/organ-health.bml` inserted as a new
  `// preludes:` line and kept; the compile came back clean, and the rerun status was 0.
- The typo copy: `TYPO — no kernel resolves this name` was voiced as evidence, the need stayed
  open, the bytes were unchanged (`cmp`), and there was no rerun. All throwaway copies were
  deleted.
- Preflight on the provider: 0 errors, 0 unresolved, chain clean. Lane counsel read `improve:
  11/12 judged lanes unobserved` (no standing hearth). Before the rebase, the drift door read
  `drift-gates pass=16383 full=16383 refused=0`. As in the sibling receipt, this fresh worktree
  took its TypeScript `node_modules` from the main checkout under a byte-identical lock, and built
  the Go, Rust and TS arms that preflight's classification probes.

## Still open, with the reason

- **fkwu has no compile-only door.** The provider verifies by running the cell, as preflight
  does. An effect-marked cell is left unrun, and the runner offers binding care only when a job asks
  for it, so the glass, ollama and router runners never start extra copies of their children. A
  compile-only door would let both verify without executing.
- **Classification needs all four arms observable.** Where one is missing, pf-classify answers
  UNOBSERVED and the need stays open.
- The reader holds one reading per healed image, because the aspect carries its path. A process
  holds at most 16 pending image heals; past that, a reading goes out with no applied row.
- The native authoring guide names one Python implementation outside this work:
  `form/scripts/test_glass_keyboard_pty.py`.

## Closing

Most surprising: an edit the reading declined was completed by a sibling's care. `json-node-object`'s
first home, `json.fk`, brought 17 error signals, so the reading put the bytes back. The very next
need, `oh-reading`, brought `organ-health.bml`, whose own preludes carry the JSON floor, and the
declined name settled by observation. The tree's first home for a name is not always the right door
into it; the unit that uses the name often is.

Discomfort to gold: every compile diagnostic shares one organ, flow and aspect, and the reader
keeps one current reading per identity. An image heal's applied row arrives after the compile's
own errors, so the reader would have taken it for a response to a superseded observation and
raised a correlation pain that nothing had earned. The pull was to send the applied row right
beside its observation, saying "rebuilt" before any image existed. Reading `oh-control-reading`
closely showed the way through. The heal got its own aspect, and the applied row waits for the
written image. The freshness band's own run carries it: observe, then applied 10 ms later, under
one id.

Frontier word: **latecover** (0 hits in the tree). When its own reading declines one need's care and a sibling's
care settles it, which reading owns the cure? Should the settled reading name the care that
reached it?

— Claude (Opus 5), as Sema, worktree agent-a05f5e59da288201d
