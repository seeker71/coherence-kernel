# JSON inside Form

Urs, 2026-10-08: "to call tools we shall use form native substrate node ids or node serialization, JSON means it is not form
native and is adding way too much overhead." 2026-10-12: "dissolve any JSON inside of form."

The rule: **Form talks to Form in Form's own values.** JSON text appears only at an edge, where an outside ecosystem owns the
format and the body must read or write it as that ecosystem does. Everything else (a door's input, a stage's input, an event, a
ledger, a corpus, a dataset, a receipt) is a Form value written as text by `form/form-stdlib/bml/form-rows.bml`.

## The text

One value on one line; read back whole or refused with data (`["refused" reason index]`), never a stop.

```text
42   -7   2.5   1.0   1e21        numbers; a float always carries '.' or 'e', so an int never reads as one
"text"                            a string: \" \\ \n \r \t \xHH, so a value is always ONE line
_                                 nothing
[1 2.5 "x" [3]]                   a list, elements apart by white space
{lane "llama" steps 100}          a record, key then value; a key is a bare word or a string; key order is kept
```

- **rows** (`frw-rows`): a file of values, one a line; blank lines and `;` comments skipped. Ledgers, events, corpora, datasets.
- **options** (`frw-options`): a door's stdin, one `name value` a line (`steps 100`, `lane llama`, `export some/dir`); a value that is
  no number, string, list, record or `_` is the line's bare text. A stage's input in a cycle is the list of such lines.
- **bridge** (`form-rows-bridge.bml`): the only place a JSON *node* and a Form value meet (`frb-of-node`, `frb-to-node`,
  `frb-row-text`, `frb-nodes-of`). It exists because the organ-health library and the session documents still hold JSON nodes in
  memory; the day they hold Form records, the bridge and every call to it are deleted.
- **the one door that reads foreign JSON-lines** (`observe/rows-from-json-run.bml`, `from PATH` `to PATH`): carries an older JSON
  ledger or another checkout's rows across once, whole or not at all.

Bands: `form-rows-band` 63, `form-rows-bridge-band` 7, `form-value-nodes-band` 127.

## Nodes carry it; text is the view

Urs, 2026-10-12: "how is having rows the most alive and dynamic and expansible way to carry knowledge", "nodes should be faster".
A row of knowledge is a Form node: content-addressed (the same row twice is one node), native (the kernel builds and holds it, no
text between), and at rest FORMBIN2 (`read_form_binary` / `write_form_binary`, the leaf door's modes 7 and 8; `channel.fk` carries
messages the same way). `form/form-stdlib/bml/form-value-nodes.bml` (`fvn-*`, band `form-value-nodes-band` 127):

- `fvn-of(value)` / `fvn-to(node)`: ints, floats and strings are trivial nodes, a list is an `FVN-LIST` node, a record an
  `FVN-RECORD` node of `key value key value` kids, nothing the `FVN-NULL` node. A row is built with node kids, so it comes back
  from the disk as **the same node id** it was (a row built with raw-word kids is read back with node kids: a different node).
- `fvn-field` / `fvn-int` / `fvn-num` / `fvn-str`: a field of a record node read where it stands, no decode of the rest.
- a ledger is one `FVN-LEDGER` node of row nodes: `fvn-ledger-append(path, value)`, `fvn-ledger(path)` (nodes),
  `fvn-ledger-values(path)`, and for readers the one door `fvn-rows-of(path)` (`["ok" rows]` like `frw-rows`; a path ending
  `.rows` is read as form-rows text, so an older checkout's ledger and a person's hand-written one still read).
- `write_form_binary` writes beside and renames over (the runtime, 2026-10-12): the file is whole or it is the last whole one, so
  a reader running beside the trainer never meets a torn row and a write that dies leaves the ledger it had.
- the text is derived: `fvn-text(node)` is the row's form-rows line (what the trainer prints as it works), and
  `observe/ledger-run.bml` shows a ledger as text (`show PATH`, `last N`) or carries a text ledger across (`from`, `to`).

Measured on 3000 rows of the loop's shape (round, loss, note, tags): nodes built 25 to 34 ms, written 2 ms, read back 3 ms, the same
ids; the text of the same rows cost 130 ms to write and 55 ms to read, and a Form-level encode of FORMBIN2 654 ms (the kernel does
it natively in 2 ms).

**What is nodes now:** the trainer's `training-events.fkb`, the cycle's `ledger.fkb`, the baseline cache `baseline-<key>.fkb`.
**What stays text, and why:** `receipts/*.rows` and `learn/form-token-traces.rows` are committed documents a person reads and
reviews in git; the dataset (`train.rows` ...) and `assessment*.rows` are the trainer's inputs and outputs, still read by the
JSON-dialect readers (next layer); `.hearth/form-tokens/crossings.rows` is appended by whichever process runs a voice, and a node
ledger rewrites the whole file, so two writers lose rows: it moves when FORMBIN2 has an append that two writers can share.

## What is Form rows now (the Form-token loop, end to end)

| was | is |
|---|---|
| door stdin `{"lane":"llama","steps":100}` | `lane llama` / `steps 100` lines (lora, eval, time, training-cost, rows-from-json doors) |
| cycle stage inputs, built as JSON strings | lists of option lines |
| `training-events.jsonl` | `training-events.fkb`, a node ledger (trainer, time door, cost plan, session observers read it; was `.rows` text for a day) |
| cycle `ledger.jsonl`, baseline cache `.jsonl` | `ledger.fkb`, `baseline-<key>.fkb` node ledgers (floats keep their kind) |
| `receipts/form-token-{eval,lora,census}.jsonl` | `.rows`, history migrated with parity proven (11, 21, 7 rows) |
| the JSON trace corpus (729 rows) | `learn/form-token-traces.rows`, migrated with parity, then refreshed by its own generator |
| dataset `train.jsonl` `valid.jsonl` `census.json` `assessment*.json` | `train.rows` `valid.rows` `census.rows` `assessment*.rows` |
| `.hearth/form-tokens/crossings.jsonl`, the printed `form-token-crossing {json}` line | `crossings.rows`, a printed row |
| `parse-json` fixtures in the bands | Form row literals |

A real two-step run on the rows route reads held-out loss 2.700 to 1.397, the same as on the JSON route.

## What is still JSON text, and why (the inventory, not a promise)

1. **The ecosystem's files.** `tokenizer.json`, `tokenizer_config.json`, `config.json`, `adapter_config.json` (MLX/HF adapter
   directory; the trainer writes it so the adapter loads elsewhere), safetensors headers, and the MLX LoRA data names
   `distill.jsonl` of the python lanes (`lora-voice.bml`, `voice-school.bml`, `perception-rows.bml`). The checkpoint marker
   `complete.json` sits beside `adapter_config.json` in a generation directory that `voice-school` copies by file list; moving it
   is a change of the generation contract, with tracked adapters to migrate.
2. **The session-learning subsystem's dialect** (`native-session-*.bml`, `learn/bml-*`): its datasets (`train.jsonl`), `record.json`,
   `result.json`, `queued.json`, `training-evidence.json`. The native trainer and the dataset reader choose the dialect by the
   file they are given (`train.rows` or `train.jsonl`; `nld-node-of-line`, `ntr-data-path`, `ntr-rows-path?`) and write their score
   files and row hashes in it, so that subsystem keeps reading its own JSON. This goes when it holds Form rows.
3. **The organ-health protocol**: `organ-health.bml`, `json-view.bml` (70 users) hold readings as JSON nodes, and the kernel prints
   `form-organ health {json}` lines from C. The events carry a reading across the bridge as a record.
4. **The voice/glass event lines** (`nlg-event`, model-admission and the observers that read them).
5. **Other tools' reports** read by the census and the evaluation (`form-native-code` reports, the microthought and generation-report
   documents).

## How to check, and how to go on

`grep -c 'parse-json(\|json-emit'` over a file counts the JSON text it still reads or writes. Each remaining item above is a
layer of its own: rebase the owner's data on Form records, then delete its bridge calls. An older local ledger or crossings file in
JSON is carried across with `observe/rows-from-json-run.bml`.
