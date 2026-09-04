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

The acceptance corpus covers real-shaped source and config documents,
empty inputs, no matches, missing versus null JSON values, malformed queries,
ambiguous edits, and immutable state across successive native calls. Tests
call the actual form-cli entry, not only catalog descriptions or helper functions.

## Calling the native interface

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
original document values and empty stdout; no partial edit is accepted.
Ordinary errors use exit 2. `rg` uses 1 for a completed search with no match;
`jq -e` uses 1 for a final false/null and 4 for no output values.

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

## Copyable examples for every resident tool

Start every call with a document corpus the caller already holds. The corpus in
this example is deliberately small so an agent can paste a call into a Form
cell and see the same result as the executable example band.

```lisp
(let docs (list
    (list "source:alpha" "alpha.txt" "alpha 1\nbeta 2\n")
    (list "source:table" "table.txt" "left:right\nup:down\n")
    (list "source:package" "package.json" "{\"name\":\"demo\",\"version\":1}")))
```

Each row is an argv call: a Form list is already the command boundary, so no
shell parser, quoting mode, pipe, redirection, expansion, or executable lookup
is involved. `input` is the final string argument to `fc-tool-call`; omit
paths to operate on that held string, or supply resident paths to select
documents.

| Tool | Direct Form call | Expected `fat-out` |
| --- | --- | --- |
| `rg` | `(fc-tool-call docs "rg" (list "-nF" "alpha 1") "")` | `alpha.txt:1:alpha 1\n` |
| `jq` | `(fc-tool-call docs "jq" (list "-r" ".name" "package.json") "")` | `demo\n` |
| `read` | `(fc-tool-call docs "read" (list "alpha.txt") "")` | `alpha 1\nbeta 2\n` |
| `cat` | `(fc-tool-call docs "cat" (list "alpha.txt" "table.txt") "")` | concatenated document text |
| `head` | `(fc-tool-call docs "head" (list "-n" "1" "alpha.txt") "")` | `alpha 1\n` |
| `tail` | `(fc-tool-call docs "tail" (list "-n" "1" "alpha.txt") "")` | `beta 2\n` |
| `wc` | `(fc-tool-call docs "wc" (list "-lwc") "one two\n")` | `1 2 8\n` |
| `sort` | `(fc-tool-call docs "sort" (list "-nu") "10\n2\n2\n")` | `2\n10\n` |
| `uniq` | `(fc-tool-call docs "uniq" (list "-c") "a\na\nb\n")` | `2 a\n1 b\n` |
| `tr` | `(fc-tool-call docs "tr" (list "-d" "\\n") "one\ntwo\n")` | `onetwo` |
| `cut` | `(fc-tool-call docs "cut" (list "-d" ":" "-f" "2") "left:right\nup:down\n")` | `right\ndown\n` |
| `awk` | `(fc-tool-call docs "awk" (list "{print $2}") "one two\nthree four\n")` | `two\nfour\n` |
| `sed` | `(fc-tool-call docs "sed" (list "-n" "2p" "alpha.txt") "")` | `beta 2\n` |
| `edit` | `(fc-tool-call docs "edit" (list "alpha.txt" "beta 2" "gamma 3") "")` | `edited\n`; retain `fat-documents` |
| `write` | `(fc-tool-call docs "write" (list "notes.md") "held note\n")` | `created\n`; retain `fat-documents` |

`edit` and `write` return a new resident corpus; they do not mutate `docs`.
Keep that returned value explicitly before the next call:

```lisp
(let changed (fc-tool-call docs "edit" (list "alpha.txt" "beta 2" "gamma 3") ""))
(let next-docs (fat-documents changed))
(fat-out (fc-tool-call next-docs "read" (list "alpha.txt") ""))
; alpha 1
; gamma 3

(let created (fc-tool-call next-docs "write" (list "notes.md") "held note\n"))
(fat-out (fc-tool-call (fat-documents created) "read" (list "notes.md") ""))
; held note
```

A native pipeline passes an output string as the next call's `input`, rather
than spelling `|`. For example, to take the first matched line:

```lisp
(let found (fc-tool-call docs "rg" (list "-nF" "alpha 1") ""))
(fat-out (fc-tool-call (fat-documents found) "head" (list "-n" "1") (fat-out found)))
; alpha.txt:1:alpha 1
```

`zg` is the separate native catalog-discovery route, rather than one of the
fifteen resident-document operations. It has no document or state argument:

```lisp
(fc-respond "zg hybrid kernel call")
; zg-native status=hit route=hybrid ... crossings=0
```

Use `fc-tool-command` only when an agent already holds a single human-shaped
command string and needs its bounded argv reader:

```lisp
(fat-out (fc-tool-command docs "jq -nr --arg x 'a b' '$x'" ""))
; a b
```

Its deliberate errors (`shell-syntax-not-supported`, an unsupported option,
malformed JSON, an ambiguous edit, or an absent resident path) are structured
results. Read `fat-exit` and `fat-error`, repair the value or request the
needed resident document, then make the next native call. Do not silently
fall back to a host command.

## Supported workload profile

| Tool | Native profile |
| --- | --- |
| `rg` | Line search; `-i -S -F -n -w -v -l -c -q`, `--column`, `--files`, `-e/--regexp`, `-g/--glob`, `-m/--max-count`, `-A/-B/-C`, `--`, and common long aliases. Short boolean flags can be clustered; value flags take a separate argv value. |
| `jq` | `.`, object paths and quoted bracket keys, nonnegative array indexes, `[]`, pipes, `//`, `map`, `select`, comparisons, array collection and explicit-key object construction; `keys length type empty sort unique to_entries`; literals and bound variables. Flags `-r -c -e -s -n`, `--arg`, `--argjson` precede the filter. |
| `read`, `cat` | Read held input or concatenate resident documents selected by identity/path. |
| `head`, `tail` | `-n N`, `-c N`, default ten lines; preserve final-newline state. |
| `sed` | Numeric print ranges only: `-n 'Np'` or `-n 'N,Mp'`. |
| `wc` | Byte, word and newline counts: `-c -w -l` and common combinations. |
| `sort` | Lexical lines, `-r`, `-u`, `-n` and common combinations. Numeric mode accepts complete JSON-number lines, with numeric deduplication for `-nu`. |
| `uniq` | Adjacent equal lines; `-c`, `-d`, `-u`. |
| `tr` | Equal-length literal byte sets or `-d SET`; `\n \r \t \\` escapes. |
| `cut` | `-d DELIMITER -f N`, one delimiter byte and one field. |
| `awk` | Single field output `{print $N}`, N from 0 to 9. No arbitrary program or system call. |
| `edit` | `path old new`; exactly one occurrence required, with nonempty old text. |
| `write` | `new-path text`, or `new-path` plus held input. Existing documents cannot be overwritten. |

Search patterns are byte-oriented: literals, `. ^ $ |`, character classes and
ranges, ASCII `\d \w \s \b`, and `? * +`. Case folding is ASCII. Groups,
counted repetition, backreferences, lookaround, PCRE and Unicode character
classes are unsupported errors. Globs support `*`, `**`, `?`, and leading `!`;
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

`form-cli-agent-tools-band.fk` and `form-cli-agent-tools-edge-band.fk` each
declare 65535. The first measures process/file/network native-op counters
around actual public calls, after source admission. Loading the Form program
is outside that counter window; the trailing zero in the result is a contract,
not itself the measurement.

`form-cli-agent-tools-portable-band.fk` registers 127 in the four-way manifest
and calls the same entry for search, JSON, text slicing and immutable edits.
`form-cli-agent-tools-examples-band.fk` registers 32767 there: one direct
public call for each of the fifteen resident tools. `form-cli-zg-band.fk`
separately observes the native catalog-discovery route and its zero crossings.
The existing auxiliary validator follows `.bml` as well as `.fk` dependencies,
including BML `// preludes:` headers. Its proof-only text lowering does not add
an external execution path to these resident tools.

There is no claim that a particular percentage of Claude, Codex or Grok traffic
has been measured. The profile is an engineering scope, extended by concrete
commands and regression cases rather than by implementing every upstream flag.
