# Resident coding-agent tools

Scope: the search, JSON inspection, text-reading and exact-edit operations used
in ordinary coding-agent work. This is a bounded compatibility profile, not a
claim of complete ripgrep, jq, POSIX shell, Git or agent-product parity.

The native entry accepts documents as `(identity, source-lens, held-text)` Form
values, an explicit tool name, argv values and held input. It returns an exit
code, output, error, and the next document values. A caller retains the returned
documents after an edit. Reading the host workspace, writing files and sending
Git changes are separate effects; none is hidden inside this tool interface.

New runtime meaning is executable BML. Existing Form parsing and text organs are
reused. No executable lookup, subprocess, Bash, FFI search/JSON library, network,
or implicit host fallback belongs to this execution path. Unsupported syntax is
an error even when the input is empty. Native pipelines pass result values,
not shell commands.

`form/form-stdlib/tests/form-agent-tools-band.bml` is the running witness. It
sends every copyable command below as one JSON request to `fat-wire-call`, the
function form-cli's text face hands a raw JSON object to (`fc-tool-wire` in
`form/form-stdlib/form-cli.fk`), and reads the response back through the wire's
own JSON reader: the sixteen tool rows, the edit → write → read handoff, a
pipeline through held input, a pipe refused as `shell-syntax-not-supported`
with the corpus unchanged, `jq --arg`, and grep's proof: selection, output
shape, context, the regular-expression dialects, `-F`, several documents, exit
codes, held input, the old `grep | head | grep -o | tr | tr` shell pipeline
reproduced as five calls, the library door and the catalog row (`; Verdict
4294967295`).

## Agent wire — the normal calling boundary

Agents should send one JSON request, not construct a Form expression.  A raw
JSON object entered at the form-cli text face is dispatched to the native wire:

```json
{
  "command": "rg -nF 'alpha 1'",
  "input": "",
  "documents": [
    {
      "id": "source:alpha",
      "path": "alpha.txt",
      "text": "alpha 1\nbeta 2\n"
    }
  ]
}
```

It returns JSON only:

```json
{
  "schema": "form-agent-tool-wire-v1",
  "exit": 0,
  "stdout": "alpha.txt:1:alpha 1\n",
  "stderr": "",
  "documents": [{"id":"source:alpha","path":"alpha.txt","text":"alpha 1\nbeta 2\n"}],
  "crossings": 0
}
```

`command` is a familiar bounded argv-style string; it is parsed by Form, not
by a shell. `input` and `documents` are optional (both default to empty).
Each supplied document needs `id`, `path`, and `text` strings. Pass the
returned `documents` array into the next JSON request after `edit` or `write`:

```json
{"command":"edit alpha.txt 'beta 2' 'gamma 3'","documents":[...]}
```

Then send the response's `documents` to `{"command":"read alpha.txt",...}`.
There is no hidden session state, host workspace read, subprocess, or fallback.
Malformed JSON, missing fields, unsupported command syntax and tool failures
are ordinary structured results with `stderr`; inspect them and repair the next
request rather than switching to a host command.

## form-find — the source tree, indexed and resident (the agents' normal lookup)

The wire above carries the documents it is handed: at most 512 documents and one MiB, and a cold `./fkwu` for every call. It cannot answer "who
defines X" over a tree of 2,100 files. `form-find` can. It is a resident service (`observe/form-find-run.fk`, started by launchd as data:
`docs/launchd/earth.hati.form-find.plist`; the body never starts it) that holds the checkout in memory and answers the same wire JSON:

| command | answer |
| --- | --- |
| `find [-i -F -w -x -v -l -c -m N] PATTERN [PATH...]` | indexed search, ERE or `-F`, rows as `rg -n`: `path:line:text` |
| `defs NAME...` | where NAME is defined (`def` `defn` `class` `field` `thought`): `path:line:text` |
| `callers NAME` | the lines using NAME as a whole name, definitions left out: `path:line:text` |
| `bands [NAME]` | the band files (a `; Expected:` or `; Verdict` header) that name NAME: `path expected= verdict= mentions=` |
| `imports UNIT` | `UNIT -> raw` for what it imports (`import` lines, `preludes:` headers), `UNIT <- path:line:text` for who imports it |
| `rows FILE [FIELD[=VALUE]]` | a `.jsonl` census or ledger (a `.hearth/*.jsonl` too) as a keyed table: the rows holding FIELD=VALUE, an array field matching any element; with a bare FIELD its values and counts |
| `status`, `refresh` | the index's counters; a refresh now |
| `read cat head tail sed wc jq grep rg` | the wire's own tools over the indexed files the command names (`rg` and `grep` over the whole tree are `find`) |

Anything else is a named refusal (`unknown-command`, `form-find-reads-only` for `edit` and `write`, `find-needs-a-literal-or-paths` for a
search with no case-exact literal to narrow it, `find-context-not-supported`), never a host fallback. A request that carries its own `documents`
goes to the wire untouched. The answer is the wire's JSON (schema `form-agent-tool-wire-v1`, `crossings` 0); a miss is exit 1, an empty stdout.

**What it holds.** The file table is `source-tree.bml`'s walk (every `.gitignore` honoured); a file's bytes are held when it is at most 2 MiB
with no NUL (the rest are in the table, not searchable). Over that: a definition index, the import edges, the band headers, and a trigram
posting set (built the first time a search needs a trigram or a literal and kept; a literal search reads only the files its posting names). There is
no watcher: every query first stats each file (one `host_file_identity` each, about 15 ms for 2,100) and lists again any directory whose mtime moved,
so a file that changed is read again and only that one, a new file is classified by the ignore rules in force there, a changed `.gitignore` walks the
tree again. Postings follow a changed file.

**Measured** (`form/form-stdlib/tests/form-find-band.fk`, 32767 of 32767; one busy machine, 2026-10-04, tree of 2,162 files). Index: 2,145 files held,
31.8 MB of text, 55,727 definition entries, 5,628 import edges, 371 band files; built in 2.2 to 3.6 s (an independent walk alone is 1.3 to 2.4 s), not
kept on disk. A quiet refresh costs 11 to 18 ms and reads nothing; a touched file is read once and no other. Each answer equals the answer of the unindexed scan
on twenty fixed patterns, twenty names for `defs`, `callers` and `bands`, and eight units for `imports`. Five questions, milliseconds (unindexed = the engine over files read afresh;
first = the first time a fresh index is asked, which builds the postings it needs; warm = the same question again, of which the refresh is most):

| question | unindexed | first | warm |
| --- | --- | --- | --- |
| `defs fat-wire-call` | 829 | 15 | 14 |
| `callers fat-wire-call` | 142 | 48 | 16 |
| `find -F 'def fat-wire-call'` | 944 | 45 | 12 |
| `bands fat-wire-call` | 2,496 | 13 | 14 |
| `imports form-stdlib/bml/form-agent-grep.bml` | 109 | 13 | 13 |

The cold door (no service: an index of its own, then the answer) took 1.9 s in process and 2.5 s as a whole `./fkwu` process; the redirect round trip to a
standing service measured 45 to 77 ms from zsh (the poll interval and the refresh are most of it). The wire's own cold call, a fresh `./fkwu` loading its closure
and handed documents, was not measured here.

**The operator starts it once** (the body never does): `launchctl bootstrap gui/$(id -u) docs/launchd/earth.hati.form-find.plist`
(restart: `launchctl kickstart -k gui/$(id -u)/earth.hati.form-find`; take away: `launchctl bootout gui/$(id -u)/earth.hati.form-find`). The plist names this
machine's checkout and uid (501); copy it and change `WorkingDirectory` and `FORM_FIND_SPOOL` for another.

**The agent's lookup costs the client no process.** The spool is `/tmp/form-find/$UID`: write `ask.<n>.json` with a plain redirect, read `ans.<n>.json`
(written whole and renamed, so it is never half an answer; the service removes the ask). In zsh, which the agents' Bash is:

```sh
SP=/tmp/form-find/$UID; n=$$.$RANDOM; zmodload zsh/zselect
printf '%s' '{"command":"defs fat-wire-call"}' > $SP/ask.$n.json
until [ -s $SP/ans.$n.json ]; do zselect -t 2; done; cat $SP/ans.$n.json; rm -f $SP/ans.$n.json
```

The service looks every 20 ms while asks are coming and every 100 ms after ten quiet seconds. Fallbacks, in order: (1) the service stands (`$SP/status.json`
exists and is rewritten every 2 s): the index answers, in about twenty milliseconds; (2) no answer in about three seconds, or no `status.json`: the cold
door, `printf '%s' "$json" | ./fkwu observe/form-find-ask.bml`, which asks the service itself if one beats and otherwise builds an index of its own, answers, and
ends (seconds, once per call: it is the cost the redirect avoids). Neither falls back to a host `grep`, `find` or `ls`; a polled directory is used, not
a fifo bell, because a plain redirect into a fifo with no reader blocks the agent's shell.

 for a Form organ. It is not the
recommended boundary for an agent or external tool caller:

```lisp
(let docs (list (list "source:app" "src/app.py" "def hello():\n    return 1\n")))
(let found (fc-tool-call docs "rg" (list "-n" "^def") ""))
(fat-out found)
; src/app.py:1:def hello():

(let edited (fc-tool-call docs "edit" (list "src/app.py" "return 1" "return 2") ""))
(let next-docs (fat-documents edited))
(fc-tool-call next-docs "read" (list "src/app.py") "")
```

Result layout: `[schema, exit, stdout, stderr, next-documents, crossings]`, with
schema `form-agent-tool-result-v1`. `fat-exit`, `fat-out`, `fat-error`,
`fat-documents`, and `fat-crossings` are the accessors. Failure returns the
original document values; no partial edit is accepted. Failure at a replacement
within a multi-edit returns JSON in stdout naming the one-based `failed_edit`
and `documents_changed: 0`. Invalid batch shape and unchanged final text use
the ordinary empty-stdout errors.
Ordinary errors use exit 2. `rg` uses 1 for a completed search with no match;
`jq -e` uses 1 for a final false/null and 4 for no output values. `grep` uses
grep's own: 0 when a line was selected, 1 when none was, 2 for an error (bad
flag or pattern, an absent document); `-q` with a selected line is 0 even when
another operand errored. A grep error with partial output keeps that output in
`stdout` and names the error on `stderr` (`grep: nope.txt: No such file or
directory`).

### grep as a function

Any BML cell can call grep over held text, with no process and no documents
wrapper, by importing `form/form-stdlib/bml/form-agent-grep.bml`. One implementation
(`fag-run`) sits behind this, the catalog tool and the JSON wire:

```lisp
(grep-text (list "-n" "alpha") "Alpha one\nbeta two\nalpha three\n")
; [0, "3:alpha three\n"]                    [exit, stdout]
(grep-lines (list "-v" "b") (list "a" "b" "c"))
; [0, ["a", "c"]]                           lines in, lines out
(grep-text-error (list "-E" "a(") "")
; "grep: unmatched-group-open: a(\n"        the stderr
```

The body's one grep sits behind the shell executor's `grep` builtin too (`sh-bi-grep` in
`shell-exec.fk` answers `[stdout, exit]` from `grep-text`). Over the tree's files,
`form/form-stdlib/bml/source-tree.bml` walks the working directory under its `.gitignore` files
(`tracked-files()`, the seam the native `git ls-files` will answer) and runs the same grep over each
file's text: `stw-grep-rows(args, paths)` gives `path:line:content` rows, `stw-grep-files(args, paths)`
the paths with a selected line, `stw-pick(paths, suffixes)` the `*.fk` / `*.bml` pathspecs.
prelude-reach, band-cover and the spawn-guard census read the tree this way.

The `head` and `tail` tools also read the older `-N` spelling (`head -2`) as
`-n N`, so a shell pipeline's stages become calls with the same argv.

`fc-tool-command(docs, command, input)` is a convenience argv reader, not a
shell. Single/double quotes and backslash quoting are supported. Unquoted
pipeline, redirection and semicolon syntax is rejected; variables, substitutions
and executable lookup are never evaluated. Pass intermediate output as a Form
value to the next call. For `rg`, nonempty held input replaces the search corpus;
pass the explicit `-` path to search an empty held input without selecting the
resident corpus. The returned document state always remains the caller's corpus.

The existing `fc-respond` text face dispatches read-only tools against the tool
catalog already resident in form-cli. It does not load a workspace. It refuses
`edit` and `write` because this stateless face cannot retain returned state.
In-process clients use `fc-tool-call` with their own resident source documents.

## Copyable commands for every resident tool

Start every JSON request with a document corpus the caller already holds. The
corpus in this example is deliberately small so an agent can paste it into the
wire and see the same result as `form/form-stdlib/tests/form-agent-tools-band.bml`.

```json
[
  {"id":"source:alpha","path":"alpha.txt","text":"alpha 1\nbeta 2\n"},
  {"id":"source:table","path":"table.txt","text":"left:right\nup:down\n"},
  {"id":"source:package","path":"package.json","text":"{\"name\":\"demo\",\"version\":1}"}
]
```

Each row is the value of JSON `command`. Where the row shows held text, send
it as JSON `input`; a command without paths operates on it, and resident paths
select documents. The parser supports quoted argv values but rejects pipes,
redirection, semicolons, expansions and executable lookup.

| Tool | JSON `command` | JSON `input` | Expected `stdout` |
| --- | --- | --- | --- |
| `rg` | `rg -nF 'alpha 1'` | | `alpha.txt:1:alpha 1\n` |
| `grep` | `grep -n beta alpha.txt` | | `2:beta 2\n` |
| `jq` | `jq -r .name package.json` | | `demo\n` |
| `read` | `read alpha.txt` | | `alpha 1\nbeta 2\n` |
| `cat` | `cat alpha.txt table.txt` | | `alpha 1\nbeta 2\nleft:right\nup:down\n` |
| `head` | `head -n 1 alpha.txt` | | `alpha 1\n` |
| `tail` | `tail -n 1 alpha.txt` | | `beta 2\n` |
| `wc` | `wc -lwc` | `one two\n` | `1 2 8\n` |
| `sort` | `sort -nu` | `10\n2\n2\n` | `2\n10\n` |
| `uniq` | `uniq -c` | `a\na\nb\n` | `2 a\n1 b\n` |
| `tr` | `tr -d '\n'` | `one\ntwo\n` | `onetwo` |
| `cut` | `cut -d : -f 2` | `left:right\nup:down\n` | `right\ndown\n` |
| `awk` | `awk '{print $2}'` | `one two\nthree four\n` | `two\nfour\n` |
| `sed` | `sed -n 2p alpha.txt` | | `beta 2\n` |
| `edit` | `edit alpha.txt 'beta 2' 'gamma 3'` | | `edited\n`; retain response `documents` |
| `write` | `write notes.md` | `held note\n` | `created\n`; retain response `documents` |

`edit` and `write` return a new resident corpus; they do not mutate a hidden
session. Keep the response `documents` explicitly before the next call:

```text
{"command":"edit alpha.txt 'beta 2' 'gamma 3'","documents":[...the corpus above...]}
response.stdout: "edited\n"
response.documents[0].text: "alpha 1\ngamma 3\n"

{"command":"write notes.md","input":"held note\n","documents":[...the edit response documents...]}
response.stdout: "created\n"

{"command":"read notes.md","documents":[...the write response documents...]}
response.stdout: "held note\n"
```

A native pipeline passes a response's `stdout` as the next request's `input`,
rather than spelling `|`. For example, to take the first matched line:

```text
{"command":"rg -nF 'alpha 1'","documents":[...the corpus above...]}
response.stdout: "alpha.txt:1:alpha 1\n"

{"command":"head -n 1","input":"alpha.txt:1:alpha 1\n","documents":[...the search response documents...]}
response.stdout: "alpha.txt:1:alpha 1\n"
```

`zg` is the separate native catalog-discovery route, rather than one of the
sixteen resident-document operations. It has no document or state argument:

```text
zg hybrid kernel call
zg-native status=hit route=hybrid ... crossings=0
```

## Native cell mesh

`mesh-demo` is the compact form-cli door for the bundled two-cell example:

```text
mesh-demo
; mesh-demo events=3
```

The reusable no-crossing organ is `form-cli-cell-mesh-sovereign.bml`. A channel is a Form data
value, not a resident authority: both endpoint cells explicitly name the same
observer and shared field; the observer's grounding is accepted only when it
satisfies `core-grounding.fk`; and every open, send, adaptation, or refusal is an event
row carried with the returned channel value. Cells keep their own state and may
decline a proposal without losing the prior channel.

The initial grammar streams admitted symbol strings and can select only the
fixed native primitives `identity`, `head`, `count`, and `join`. A grammar and
protocol revision is accepted only when it comes from the jointly named
observer, names that exact shared field, is valid, and advances both revisions
by one. It cannot stream arbitrary source or evaluator code. This keeps
adaptation inspectable and local while leaving room to add a new, separately
tested primitive deliberately.

The demo's `open`, `send`, and `adapted` rows are current-process
observation: they do not claim cross-process persistence. A durable mesh owner
needs to carry the returned channel data through its own native residence.
`mesh-demo` and the JSON agent wire use only the in-memory sovereign organ above.

The wire's command reader also handles a single human-shaped command such as
`jq -nr --arg x 'a b' '$x'`; its stdout is `a b\n`. Its deliberate errors
(`shell-syntax-not-supported`, an unsupported option,
malformed JSON, an ambiguous edit, or an absent resident path) are structured
results. Read `exit` and `stderr`, repair the value or request the needed
resident document, then make the next native call. Do not silently fall back to
a host command.

## Supported workload profile

| Tool | Native profile |
| --- | --- |
| `rg` | Line search; `-i -S -F -n -w -v -l -c -q`, `--column`, `--files`, `-e/--regexp`, `-g/--glob`, `-m/--max-count`, `-A/-B/-C`, `--`, and common long aliases. Short boolean flags can be clustered; value flags take a separate argv value. |
| `grep` | POSIX/GNU grep over resident documents or held input; the flag table and the pattern dialects follow this table. |
| `jq` | `.`, object paths and quoted bracket keys, nonnegative array indexes, `[]`, pipes, `//`, `map`, `select`, comparisons, array collection and explicit-key object construction; object-key `has` (string key, including present null/false/zero; array-index `has` is outside this subset); string `contains` (literal substring, string input and argument; collection containment is outside this subset); `keys length type empty sort unique to_entries`; literals and bound variables. Flags `-r -c -e -s -n`, `--arg`, `--argjson` precede the filter. |
| `read`, `cat` | Read held input or concatenate resident documents selected by identity/path. `read path offset count [lines\|bytes] [sha256]` selects a one-based page; the default unit is lines. Byte pages must preserve UTF-8 boundaries. The optional hash refuses a changed source. |
| `head`, `tail` | `-n N`, `-N` (the older spelling of `-n N`), `-c N`, default ten lines; preserve final-newline state. |
| `sed` | Numeric print ranges only: `-n 'Np'` or `-n 'N,Mp'`. |
| `wc` | Byte, word and newline counts: `-c -w -l` and common combinations. |
| `sort` | Lexical lines, `-r`, `-u`, `-n` and common combinations. Numeric mode accepts complete JSON-number lines, with numeric deduplication for `-nu`. |
| `uniq` | Adjacent equal lines; `-c`, `-d`, `-u`. |
| `tr` | Equal-length literal byte sets or `-d SET`; `\n \r \t \\` escapes. |
| `cut` | `-d DELIMITER -f N`, one delimiter byte and one field. |
| `awk` | Single field output `{print $N}`, N from 0 to 9. No arbitrary program or system call. |
| `edit` | `path old new`; exactly one literal occurrence, or no literal occurrence and `old` equals the current document's SHA256. Several literal replacements: `path old1 new1 old2 new2 ...`. Explicit whole-document replacement: `path sha256 digest new`. |
| `write` | `new-path text`, or `new-path` plus held input. Existing documents cannot be overwritten. |

Hash replacement uses the `resident_sha256` supplied by native coding reads or
guard evidence. In the three-argument form, a literal match takes precedence:
one match replaces that substring, multiple matches report ambiguity. Only a
missing literal equal to the complete current document's hash selects whole-document
replacement. A stale or malformed shorthand leaves the document unchanged.

For several focused changes, one `edit` can carry multiple old/new pairs for
the same document. Replacements run in order, so a later pair may address text
created by an earlier pair. Each old text must be nonempty and occur exactly
once at that step; hashes in this form are ordinary literal text. Every pair
must change its matched text, and the final document must differ from the
original. Any failure returns the entire original corpus and identifies the
failed pair. A batch that restores the original document returns the existing
`edit-unchanged` error with empty stdout. Caller write permissions and checks
apply to the complete result as they do to a single edit.
Role, writable-path and caller-check requirements remain in the coding owner.

### grep

`grep` reads the documents named as operands, the resident documents under a
directory prefix with `-r`, or, with no operand, the held input (`-` names it).
No process, no host file and no second regular-expression engine: it is the
engine `rg` uses (`form/form-stdlib/bml/form-agent-pattern.bml`, one parser,
three dialects, one matcher) over the documents the caller holds. Output is
grep's: a file prefix when there are several operands, `-H`, or `-r` over a
directory; `file:line:text` with `-n`; context lines after `-`, groups that do
not touch split by `--` (also between files); `-o` one match per line; `-c`
per file; `(standard input)` for held input under `-H`.

| Flags | Meaning |
| --- | --- |
| `-G` (default) `-E` `-F` | basic regular expression, extended, fixed string. The last one given wins. |
| `-e PAT` (repeatable) `-f FILE` | patterns; `-f` reads a resident document. A pattern holding a newline is several patterns. |
| `-i` `-y` `-v` `-w` `-x` | ignore ASCII case, invert, whole word (a shorter match at the same place is tried, as GNU does), whole line (`-x` wins over `-w`). |
| `-c` `-l` `-L` `-q` `-s` | count per file, files with a selected line, files without, quiet, no file errors on stderr. `-l`/`-L` win over `-c`, `-q` over all. |
| `-n` `-o` `-H` `-h` | line numbers; each match alone on a line, leftmost-longest, empty matches skipped; force or drop the file prefix. |
| `-m N` `-A N` `-B N` `-C N` `-N` | stop after N selected lines (context after the last is kept); lines of context; `-2` is `-C 2`. A value may be attached (`-A2`, `-m1`, `--max-count=1`). |
| `-r` `-R` `-a` `-I` `-U` | recurse over the resident documents under a directory prefix (`.` and no operand mean all); the last three are accepted and change nothing, since input is read as text. |
| `--` and clustered short flags | `grep -inE -- 'a|b' notes.txt`; options may follow operands. |
| Long names | `--regexp= --file= --extended-regexp --fixed-strings --basic-regexp --ignore-case --no-ignore-case --invert-match --word-regexp --line-regexp --count --files-with-matches --files-without-match --line-number --only-matching --max-count= --after-context= --before-context= --context= --quiet --silent --no-messages --with-filename --no-filename --recursive --dereference-recursive --text --color=never\|auto --binary-files=` |

Refused by name, exit 2, never a silent different answer: `-P`
(`perl-regexp-not-supported`); `-b -T -z -Z -u -V`, `--include`, `--exclude`,
`--null` and every other letter or long name (`unsupported-grep-option: -b`);
`--color=always` (`color-always-not-supported`); `-o` with a context option
(`context-with-only-matching-not-supported`); a bad number for `-m` or a context
flag (`invalid-max-count`, `invalid-context-length-argument`); no pattern
(`grep-needs-pattern`); `-f` on an absent document. The exit codes are
grep's: 0 a line was selected, 1 none, 2 error. `-L` follows GNU 3.5 and later:
0 when any line was selected, not when a file was listed. An
operand that names no resident document is `grep: NAME: No such file or
directory`; a directory prefix without `-r` is `NAME: Is a directory`; both end
the run at 2 unless `-q` already selected a line.

Pattern dialects. All three share literals, `.`, `^`, `$`, classes and ranges,
negation, POSIX classes (`[[:alpha:]] [[:digit:]] [[:alnum:]] [[:upper:]]
[[:lower:]] [[:space:]] [[:blank:]] [[:punct:]] [[:xdigit:]] [[:cntrl:]]
[[:print:]] [[:graph:]]`), groups, alternation, `? * +` and the intervals
`{m} {m,} {m,n} {,n}` on an atom or a group. `-G`: the metacharacters are
`\( \) \| \{ \} \+ \?` and `* . [ ^ $`; a bare `+ ? | ( ) { }` is a letter, `*`
at the start of an expression and `^`/`$` anywhere but the ends are literal,
`\1`..`\9` match the text of an earlier group. `-E`: `( ) | { } + ?` are the
metacharacters, and `\1`..`\9` work too. Both read the GNU escapes `\w \W \s \S
\b \B \< \>`. `rg`: the extended shape with `\d \D \w \W \s \S \b \B`, `\n`,
`\t`, no backreference, and a backslash in a class is an error.

Unsupported pattern syntax is an error naming itself, whatever the input:
`stacked-repetition` (`a**`, `a+?`, `a*+`: a lazy repetition is outside the
profile), `quantified-anchor` (`^*`), `quantified-backreference`,
`repetition-without-operand` (`*a` in `-E`), `unmatched-group-open`,
`unmatched-group-close`, `invalid-interval-expression`,
`interval-too-large` (over 32767), `invalid-interval-order`,
`invalid-back-reference` (`\3` before group 3 opened), `unknown-character-class`,
`collating-element-not-supported` (`[.x.]`, `[=x=]`),
`unclosed-character-class`, `invalid-character-range`,
`class-escape-not-supported` (`rg`), `unsupported-pattern-escape` (`\d` in grep,
`\0`, any other letter), `unsupported-pattern-syntax` (`(?` in `rg`),
`trailing-pattern-escape`, `pattern-limit` (256 bytes). Outside the engine
entirely: lookaround, lazy repetition, Unicode classes and characters as units
(matching is by byte, so `.` is one byte), non-ASCII case folding, binary-file
detection, and the host tree (`-r` walks only the resident documents).
Backtracking carries the 50,000-step budget per line; exhaustion is
`pattern-work-limit`, exit 2.

Search patterns for `rg` are byte-oriented: literals, `. ^ $ |`, groups,
character classes and ranges, POSIX classes, ASCII `\d \D \w \W \s \S \b \B`,
`? * +` and counted repetition `{m,n}`. Case folding is ASCII. Backreferences,
lookaround, lazy repetition, PCRE and Unicode character classes are unsupported
errors. Globs support `*`, `**`, `?`, and leading `!`;
the last matching glob wins. Basename globs apply at any depth. Paths select
resident exact names or directory prefixes. There is no host traversal,
ignore-file loading, file-type registry or binary-file detection. Search output
always carries its source path, even for one document.

JSON input is a whitespace-separated stream of syntactically admitted values.
Missing keys become null; incompatible input types error. Object equality is
key-order independent; repeated keys keep their last value. Ordered comparisons
require numbers. Object-constructor fields require exactly one output value.
Sorting/unique support scalar arrays. Output is compact JSON even without `-c`;
`-r` emits raw strings. Negative indexes, slices, recursive descent, arithmetic,
assignment, user functions and upstream modules are outside this profile.
JSON scalar encoding/precision follows the existing `json.fk` codec.

These are value tools, not byte-for-byte terminal emulations: counts are
unpadded, and multiple text-file inputs are concatenated without filename
headers. Sorting is deterministic, without host locale collation.

## Bounds and evidence

Admission limits: 512 documents with nonempty, unambiguous identities/paths;
128 argv values; one MiB each for resident document bytes (including identity
and path), argv bytes and held input. Output/state over one MiB is rejected.
Patterns/globs are limited to 256 bytes, queries to 512 bytes, JSON nesting to
64, and sorting to 1,024 items. Matching carries a 50,000-step budget per line
or glob attempt; exhaustion is an explicit error, not a negative result.
These bounds do not promise upstream-tool throughput or a wall-clock deadline.

The tools live in `form/form-stdlib/bml/form-agent-tools.bml`, with
`form-agent-tool-values.bml` carrying values and `form-agent-tool-wire.bml` the
JSON request/response shape: every tool, state handoff, malformed request
refusal, and direct form-cli JSON dispatch. Native-op counters (process, file,
network) are read around actual public calls, after source admission. Loading
the Form program is outside that counter window; a trailing zero in a result is
a contract, not itself the measurement. The sovereign mesh
(`form/form-stdlib/bml/form-cli-cell-mesh-sovereign.bml`) carries shared
observer admission, send/eval, refusal immutability, observer-only adaptation,
and the `mesh-demo` dispatch.
The existing auxiliary validator follows `.bml` as well as `.fk` dependencies,
including BML `// preludes:` headers. Its proof-only text lowering does not add
an external execution path to these resident tools.

There is no claim that a particular percentage of Claude, Codex or Grok traffic
has been measured. The profile is an engineering scope, extended by concrete
commands and regression cases rather than by implementing every upstream flag.
