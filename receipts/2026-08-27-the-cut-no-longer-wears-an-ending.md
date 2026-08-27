# The cut no longer wears an ending — the partial-as-whole cousins heal

2026-08-27, the day's fifth movement, from Urs's "next". Signed: Sema,
through the visiting mind.

## What healed

The sharpest survivors of the family: organs that handed back a truncated
whole because the cut wore the shape of an ending. Row 1161 names the shape:
**shortwhole** (0-hit fresh).

- **socket_recv, both arms**: a dead handle or a read ERROR answers nothing;
  "" is reserved for asked-for-zero and the peer's orderly close. Before,
  a mid-stream failure read as end-of-response and a truncated reply passed
  as complete. The Go arm also stopped DROPPING bytes that arrived together
  with io.EOF — a second quiet loss found in the same ten lines.
- **read_file_slice, both arms**: a file that never was answers nothing
  (matching read_file); a read error answers nothing instead of a silent
  zero-length "slice"; an EOF-short slice of a real file stays honest bytes.
- **fs_list (fkwu)**: the listing GROWS — the old static 512 silently
  dropped a directory's 513th entry, a partial listing wearing a whole one's
  skin. Witnessed: 600 files in, 600 listed.
- **str_line_at (Go)**: past-end answers nothing — an absent line is not an
  empty line, so scans no longer stop early at a blank row.
- **Callers armed**: http-client's accumulator propagates a died-mid-stream
  nothing to the request boundary, where it becomes one honest full retry;
  storage-port-db's key scan gains the absence stop while keeping its
  documented ""-stop contract.

## Witnessed

- fkwu probe: dead-handle recv → nothing; slice-of-never-was → nothing;
  real slice → its 4 bytes.
- `partial-absence-honesty-band` → **15** (including the 600-file listing);
  read-absence band still 127; launch band still 15; ground 42, freshness
  31; `TestPartialAsWholeOrgansAnswerNothing` and every prior Go twin green;
  the edited http-client surface lens-compiles clean, no flatten run.

## Found, named, not touched

Go's `str_line_at` is BYTE-indexed (the line containing byte i) while
storage-port-db's comment believes line-number semantics — its key scan
would cons each key once per byte of the key. Whether that cell has ever
run against a live pg answer is unwitnessed; the mismatch is recorded here
for its own movement rather than fixed blind.

Per the standing word on flatten: these heals live in the runtime C seed
and the Go arm. The emitted mirror's copies (its socket_recv / slice /
fs_list arms) are OWED and wait for the source-native emission path — the
committed emitted artifacts stand as of stamp 50bec183606a9c0c, and the
form-cli source identity has moved since; the stamp gate will say so, and
it will be right.

## Most surprising teaching

The Go socket_recv heal found a loss nobody had named: `err != nil || n <= 0`
discarded real bytes whenever the peer's close arrived in the same read —
the healed organ now speaks those bytes. Healing the absence shape uncovered
a presence being thrown away.

## Where discomfort turned to gold

The store scan's comment argued its own mask was safe ("keys are never
empty, so "" is the honest stop") — and it was almost persuasive enough to
skip the site. Reading the native instead of trusting the comment surfaced
the byte-vs-line mismatch above: the discomfort of double-checking a
documented contract turned into the movement's realest finding.
