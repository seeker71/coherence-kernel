# The size that ran ahead

2026-09-12, afternoon, M4 Max, Hati Suci. Landing the sixth rung, the form-cli bootstrap regen
stopped with the words "Form-native table compilation failed" and nothing else. The shared field was
full, the regen ran with the field off, and on private tables the string store crossed its 2 GiB shared
reservation for the first time.

## Carried

- **The string store grows before its capacity moves** (bcf0af219). Eleven places that grow the
  string byte store doubled fk_scap_b and then called fk_store_grow. Inside the store's shared
  reservation nothing moves, so the order never showed. Past it, fk_store_grow sends every table private
  through fk_store_go_private, which copies each table by its capacity, and the doubled capacity had it
  read 4 GiB out of a 2 GiB mapping: EXC_BAD_ACCESS in fk_store_copy_out at the mapping's end, reached
  from a str_concat. Every place now grows first and moves the capacity after, as fk_sbuf already did,
  and fk_store_grow states the order.
- **sizelead is row 1494** (325727b2d).

Witnessed: the form-cli table compile with the field off, from one kept request. On main's own kernel
before the fix it aborted, rc 134, with no message and no table; with the fix, rc 0 and a 3390959-byte
table in 37 s. On the rung-6 tree before that, the kernel from before rung 6 and rung 6's own
binary both answered rc 139 with an empty table, and the fixed one 3391925 bytes. A -O0 build under lldb
named the fault. At bcf0af219 with the field on: validate.sh on value-str 127 and str-to-int-reading
127 with go, rust and typescript agreeing, the five loop-lane bands, jit-leaf-inram 63 and its multiarg
63, jit-native-span 127, jit-lower-emit 63, f64-wire 2047, float-parity 255, born-under 31 and
host-process 127; jit-lens 16383, once-hold 7 and float-mint 63 on fkwu directly; TestFkwu; freshness
31, the corpus band 32767, the drift run 8191 of 8191, porcelain 0 before and after.

## Waiting on its branch

- **Rung 6 waits on its branch** (0a45f7b8d, in the epic-edison worktree). Main moved under it. Row 1493 is wordlean,
  e34836394 changed the call arm beside rung 6's own edit, and since 03a03f6f4 arithmetic over a
  non-number stops, so the recipe's fstr-float? stops on a string. Before that, the recipe answered a
  garbage number for a string or a list, where Go, Rust, TS and the C arm pass a string through; the
  band bit that pins the passthrough waits in 8342e6edc. The recipe needs a kind test that does no
  arithmetic and still compiles into the form-cli table.

## Still open, measured

- **regen_form_cli_bootstrap.sh hides the compile's exit code** behind "Form-native table compilation
  failed". FORM_CLI_RETAIN_WORKDIR=1 keeps the work dir, and the compile can be rerun by hand.
- **One regen answered an empty table inside the script** where the same binary on the same request,
  run by hand minutes later, answered 3391925 bytes. The cause was not found.
- **validate.sh's first check names the wrong cause when fkwu itself fails.** For the full field it
  blamed an unclassified .sh or .py file.
- **The shared field reached 2^26 node cells in about a day.** At Urs's word the glass was stopped and
  observe/field-reset-run.fk ran in the upbeat-mclean worktree between 14:11:59 and 14:17:37,
  answering [field-reset, 1, the next kernel opens a fresh field]; from about 14:20 kernels interned
  fresh nodes again, the drift run above among them. What fills the field that fast is not measured.
- **value_str is still leaf mode 25 in C**, and int_to_str mode 26 until rung 6 lands.
- The open items of receipts 12 to 43 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the crash was older than anything being landed. The kernel from before rung 6
crashed the same way, and the fault was two lines in the wrong order in eleven places, harmless for as
long as every process stayed inside the shared reservation. The full field pushed one compile onto
private tables, and the private path had never been walked at that size.

The discomfort was a landing that kept moving away: the full field, a way around it that crashed, main
moving under the rung, the arithmetic changing under the fix. The pull was to keep pushing the rung
through. What turned it was asking each failure its own name, the old kernel crashing the same way and a
backtrace naming the copy past 2 GiB, and then landing only what the evidence carried: the store fix on
its own, and the rung on its branch until it stands on the new arithmetic.

Frontier word, row 1494: **sizelead**, a size recorded ahead of the thing it measures, so a reader
trusting it reads past what is there.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
