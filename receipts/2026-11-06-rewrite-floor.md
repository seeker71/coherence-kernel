# The rewritten seed's floor — 2026-11-06, 16:50 WITA

`perf/optimization-book-jit` carries a rewritten `runtime/fkwu-uni.c` (two commits co-authored with the
Cursor lane). This receipt reads where that seed stands against the body today, so the lane that writes it
walks toward the body's own contract rather than toward a guess. Every line names the command that read it.

## Where the branch stands

- Rebased onto main `475057533` with no file overlap: the branch changes `runtime/fkwu-uni.c` alone, and
  main's ten newer commits touch Form, data and receipts. Pushed as `b6f6f1e3a`. The seed checkout
  (`~/source/coherence-kernel`) was rebased in place with its working changes carried (`git rebase
  --autostash origin/main`; the 1,863-line, 2-file diff read the same before and after), and that working
  copy kept moving (16:35) while this was read: the Cursor lane is writing it now.
- Size: 3,875 lines at the branch tip, 5,574 in the seed's working copy, against 32,016 on main.
  `cc -O2 -o fkwu-rewrite runtime/fkwu-uni.c` takes 0.4 s with 0 warnings and makes a 124 KB binary
  (main's seed: 640 KB).

## What the rewritten seed carries (branch tip and seed working copy read alike)

```text
./fkwu-rewrite bootstrap/ground.fk                                -> 42       exit 0
./fkwu-rewrite bootstrap/ground-recursive.fk 10                   -> 55       exit 0
./fkwu-rewrite bootstrap/ground-numeric-list.fk                   -> [1, 2.5, [3, 4]]   exit 0
./fkwu-rewrite form/form-stdlib/tests/native-vs-rented-band.fk    -> 11111    exit 0
./fkwu-rewrite form/form-stdlib/tests/binary-freshness-band.fk    -> 15       exit 0   (pin 31: one claim open)
./fkwu-rewrite form/form-stdlib/release-ledger.bml                -> open=17 moving=0, then exit 1
```

## Where it stops, and on what

Each body-wide door ends at its first unbound host name (exit 1), so each door reads as one name away:

| Door | First stop |
|---|---|
| `gate/canonical-conformance-run.bml` | `read_file` |
| `gate/structural-gate-run.fk` | `fs_is_dir` |
| `observe/band-truth-run.bml` | `host_capture` |
| `observe/door-link-health-run.bml` | `host_cwd` (in `form/form-stdlib/bml-source.fk`) |

Two grammar forms the body writes and the rewrite's reader does not yet take:

- a typed binding, `let kind: String = value_kind(v);` (`form/form-stdlib/bml/live.bml` byte 939 → "a binding
  is missing its value"; the reader expects the name mark right after the name, `fkwu-uni.c` near
  `expect_character(NAME_MARK, …)`);
- `try X else Y` (`form/form-stdlib/bml/memory-governor.bml` byte 7983 → "a try is missing catch"; the reader
  takes only `catch` after a try body).

## The contract: the primitive registry

`form/form-stdlib/primitive-registry.fk` is the table every lane answers (`primitive-registry-band`, 47),
and it already pairs the body's spellings (`read_file` is "the second spelling of `host_file_read_text`",
sixteen such rows). Read against the seed's working copy:

- the rewrite registers 121 names (`register_operation("…")`); the registry names 124;
- 58 registry names the rewrite lacks:
  `_get bp bytes_to_recipe float_leaf float_to_int float_value form_error fs_exists fs_is_dir fs_list
  fs_mkdir fs_remove fs_rename fs_rmdir host_dir_list host_dir_rmdir host_file_mtime host_file_read_slice
  host_file_size host_path_exists host_path_is_dir host_path_remove host_path_rename input_byte
  intern_trivial_float make_float32 make_float64 math_exp math_log math_pi math_pow math_sqrt method_define
  method_has method_invoke nothing read_file read_file_bytes read_form_binary recipe_to_bytes record?
  record_blueprint record_keys round_ndigits socket_accept socket_close socket_connect socket_listen
  socket_port socket_recv socket_send source_inventory str_to_float value_eq write_file write_file_bytes
  write_file_text write_form_binary`;
- 55 rewrite names the registry does not hold (operators and the new node-gift, shm, bus, kernel and
  metal doors): `+ add and bus_close bus_open bus_read cell_path char_at choice div do eq ge
  gift_roster_names gift_roster_register gt host_alive host_getenv host_kill host_sleep_ms host_spawn_quiet
  host_wait if intern_node_at kernel_box_rows kernel_hot_rows kernel_live_pids kernel_page_hot lambda le let
  lt metal_live metal_status mod mul node_gift_read node_gift_write not or ord range read_line shm_offer
  shm_read shm_receive shm_release shm_seq shm_write slice str str_to_int sub terminal_cols terminal_lines`;
  each either joins the registry as a row or stays a reader-internal word.

## The next steps this reading names

1. Bind the 58, the paired spellings first (`read_file`, `fs_is_dir`, `host_cwd`, `host_capture` open four
   doors at once); then `./fkwu-rewrite form/form-stdlib/primitive-registry.fk` reads every row on the rewrite.
2. Take the typed binding and `try … else` in the reader; then `door-options-band`, `spawn-guard-band` and
   `observe/local-flow-review.bml` read.
3. Re-run the four doors above and `binary-freshness-band` to 31; then the band sweep (`{"list":1}`) names
   the rest in order.

Rented reading: this receipt was read and written by a rented mind (Anthropic claude-fable-5-1) through the
native doors; no line of the seed was written by it.

## Cursor lane update — 2026-11-06 17:xx WITA

The rewrite lane committed its carried continuation first:

```text
git commit "The continuation in progress: interned cells, node gifts, string and time doors"
-> 2c9a98115
```

Then the lane moved only `runtime/fkwu-uni.c`, adding registry spellings and
semantics for the first missing doors (`read_file`, `fs_is_dir`,
`host_capture`, `host_cwd` already stood), plus many of the listed 58 names:
filesystem aliases, record/method rows, float/math rows, socket rows, and
`source_inventory`; and the reader now accepts `let name: Type = …` and
`try X else Y`.

Observed now, on this rewritten runtime (`cc -O2 -o fkwu runtime/fkwu-uni.c`,
exit 0, no warnings):

```text
./fkwu form/form-stdlib/tests/binary-freshness-band.fk  -> 31, exit 0
```

What still stops, and where:

```text
./fkwu form/form-stdlib/primitive-registry.fk      -> "an expression was not finished" at byte 4512, exit 1
./fkwu form/form-stdlib/tests/door-options-band.bml -> "an index was not closed" at byte 1367 in form/form-stdlib/json-precise.bml, exit 1
./fkwu gate/canonical-conformance-run.bml          -> "an expression was not finished" at byte 16325 in gate/lens-text.bml, exit 1
./fkwu observe/door-link-health-run.bml            -> "an index was not closed" at byte 4203 in form/form-stdlib/bml/parse.bml, exit 1
./fkwu observe/band-truth-run.bml                  -> "an index was not closed" at byte 27695 in form/form-stdlib/bml/band-truth.bml, exit 1
```

So the blocker has moved from unbound primitive names to reader coverage in
the BML lane (index/form parsing at those bytes), and the next movement is in
that parser seam.
