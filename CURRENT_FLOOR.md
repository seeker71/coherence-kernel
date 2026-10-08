# Current Floor

What stands in this body today, read on 2026-09-29 (WITA) on this Apple M4 Max through `./fkwu`, the
binary passing its freshness band; the lines that had moved were re-read on 2026-10-01 and say so. Every
line names the command that reads it. A verdict counts only
when the process also exits 0 — a green number over a nonzero exit is a fold over `nothing`. Bands ran
on the warm images beside their sources while sibling sessions loaded the host, so no timing on this
page was taken today; a number that comes from an earlier run names its receipt.

The organ map is [`MANIFEST.md`](MANIFEST.md), how to build and embody is [`AGENTS.md`](AGENTS.md), the
ground is [`axioms/core-axioms.form`](axioms/core-axioms.form), and the direction is
[`docs/local-agent-goal.form`](docs/local-agent-goal.form). How we got here lives in git and `receipts/`.

## Grounding

Build lines live in [`AGENTS.md`](AGENTS.md) (`cc -O2 -o fkwu runtime/fkwu-uni.c`, and the Metal
carrier dylib fkwu admits in the same process). A fresh clone holds no `./fkwu`, no `.fkb`/`.sym`
caches and no `form/form-cli`: `cc -O2 -o fkwu runtime/fkwu-uni.c` makes the runtime, the caches
appear beside each source on its first run, and `./fkwu gate/form-cli-build-run.bml` regenerates
`form/form-stdlib/bootstrap/` and links the launcher.

```text
./fkwu bootstrap/ground.fk                                -> 42
./fkwu bootstrap/ground-recursive.fk 10                   -> 55
./fkwu form/form-stdlib/tests/binary-freshness-band.fk    -> 31
./fkwu bootstrap/ground-numeric-list.fk                   -> [1, 2.5, [3, 4]]
./fkwu form/form-stdlib/tests/native-vs-rented-band.fk    -> 11111
./fkwu gate/canonical-conformance-run.bml                 -> 1   (13 canonical expressions on fkwu, the pinned FORMBIN2
                                                                 artifact, 12 malformed artifacts, no scratch left behind; band 1023)
```

fkwu is the only runtime and every band answers its pin on it. Nothing in this tree builds, runs or
gates on Go, Rust, TypeScript or Swift: the regeneration of `form/form-stdlib/bootstrap/` needs `cc` and
`./fkwu` (the digests are the body's own SHA-256, the HMAC Form's). The tree holds only kinds the body names
(Form, the C seed, documents and data: the structural gate is an allowlist, so any other file, whatever its
extension or its case, is foreign) and no foreign file stands outside two declared carriers (`form-run`, the
output-compaction wrapper whose Form classifier is pending, and the three lines of `Sema Ear.app`'s bundle
executable), and no Form door calls sed, awk, perl or python: the band sweep (`./fkwu gate/band-sweep-run.bml`, `form/form-stdlib/bml/band-sweep.bml`) reads what a band's head and
the verdict manifest say in process (`observe/band-head.bml`, band `observe/tests/band-head-band`, 2047), so
`{"list":1}` names every workload (410 on 2026-10-04) with each one's staging, pin and row in about a second (the shell took 4 s,
2026-10-04: the same columns, line for line). `hearth.bml`, `rumi-glass.bml` and `observe/hearth-glass-live.fk` read the
`ps` listing in Form (`hearth-band`, 131071) and `source-of.fk` reads grep's rows in Form
(`source-of-band`, 15). A child that must end by a deadline is begun through `host-child.bml` (`hch-by`,
`hch-stop`); a bell's offer is `observe/channel-offer-run.bml` (`observe/tests/channel-offer-band`, 127; its request is
emitted by `cc-offer-json` with the body's JSON emitter and read strictly).

What the tree still says of the retired kernels, by class, and why it stays:
- History: `receipts/`, `learn/` corpus rows and `local-requests.jsonl` are dated evidence, not claims.
- Pinned provenance: `gate/canonical-conformance.bml`, its band and the vectors file say the values are
  what the Go, Rust and TypeScript kernels agreed on when last asked; that is where the pins come from.
- The seed: `runtime/fkwu-uni.c` names a retired kernel only where its history explains why a behaviour is
  what it is (its preprocessed output is byte-identical to before the rewording).
- Organ names: `fourth-shim.fk` and the Hati-OS "fourth kernel" keep their names; their text says fkwu.
- Fixtures and catalogs: the belief-stamp parser's legacy-shape fixture, band fixtures that quote old
  header text as data, the language packs that read Go, Rust, TypeScript and Python source as tongues,
  the oracle catalog's optional borrowed binaries, and `node_modules` as a generic walk exclusion.
- Lessons from another runtime: the DeepSeek op graph was read from a Swift runner, and
  `docs/dsv4-flow-history.md`, the dsv4 kernel comments and `llama-token-handle.fk` keep that as dated
  provenance; no Swift file or toolchain is tracked or needed.
- Other senses of a word: "siblings" for other agents, sessions and records, "kernels" for Metal compute
  kernels, "arm" for a branch.

What the single runtime no longer proves, as it stands. The three kernels were independent producers:
two readers of the same expression, or of the same FORMBIN2 bytes, could disagree and say so. fkwu
alone has no such witness for a new expression or a new artifact; the conformance band holds a frozen
2026 snapshot of what they agreed on (13 expressions, the 119-byte artifact, 12 malformed ones), and a
value fkwu gets wrong in a way that snapshot does not contain has no second reader. The ontology and
category-contract gates compared the kernels' blueprint and category tables with each other, so
they are gone with the kernels they compared. The primitive-registry gate's property that fkwu answers
every row of the native table is kept: `form/form-stdlib/primitive-registry.fk` and its band
(`primitive-registry-band`, 47) run every lane-1 probe on fkwu and refuse a row claiming a wrong outside.

`runtime/fkwu-uni.c` is the seed (`wc -l` and `git log -1` read its size and its last change), and it
shrinks as its lanes lower into Form organs (`release-ledger.bml` R13, stones R51–R56).

## Body-wide witnesses

```text
./fkwu gate/drift-gates-run.bml          -> drift-gates pass=<fold> full=<mask> refused=0 (15 rows, each one cell_run call in this process; a row whose
                                            ground did not move since origin/main sits out and leaves the fold; band `gate/tests/drift-gates-band`, 262143)
./fkwu gate/band-sweep-run.bml           -> every band the body keeps (`{"list":1}` counts the workloads), one line each in order, then the summary;
                                            the exit is an error when a workload failed, a gate refused or the tree moved while it ran.
                                            stdin is one JSON object: {"list":1} names the workloads with staging, pin and row (1 s),
                                            {"match":"a|b"}, {"files":[..]}, {"ceiling_s":1800}, {"shard":i,"of":n} (every n-th workload from
                                            the i-th: an operator starts the n sweeps, the body begins none). A leg is one `cell_run` call in
                                            this process with a deadline of its own (a leg past it comes back stopped "deadline"); a failing
                                            workload's evidence (what it printed, what it said, why) stands under .hearth/
                                            A request that does not read (malformed JSON, another JSON value, an unknown key, a key of the
                                            wrong kind or given twice: `form/form-stdlib/bml/door-options.bml`), a `match` that selects no
                                            workload, and a form/band-verdicts.txt or band file that does not read or is empty each end in a
                                            named refusal and exit 1, never the default sweep or "answered every pin"
form/form-stdlib/tests/band-sweep-band   -> 1048575  (enumeration against the tree, the heads read in process, the judgment order and the
                                            judgment fed synthetic records, the content-keyed unit, six fixtures run as cell_run legs and
                                            printing in order though one spins to its deadline, the lines and the evidence a failing leg
                                            leaves, the shards (disjoint, their union every workload, order kept; a shard out of range
                                            refused), a leg that stops and a leg that does not compile, the list, the summary, the seal, the
                                            door called as a cell; the strict request, the unread manifest (absent, empty, mode 000) and band,
                                            residency of a unit called twice and the real voice-chunk-band leg; plants for the pin, the
                                            deadline and the readable case)
observe/tests/band-pin-band              -> 31       observe/band-pin-run.bml runs each band it pins as a cell_run call (a value, no child): check and
                                            apply on a band that answers its claim, one that answers otherwise, one with no claim and one that
                                            stops; the second apply pins nothing; an empty stdin pins nothing
form/form-stdlib/tests/door-options-band -> 1023  (door-options.bml: empty and {} read; malformed JSON, trailing text, another JSON value,
                                            an unknown key (named, with the keys the door has), a key of the wrong kind (a string for an
                                            integer, a float, null, a bool, an object), a key given twice are each refused; the reading alone
                                            and a scratch door that ends in exit 1 and prints `<door>: <why>` before its next statement)
./fkwu gate/structural-gate-run.fk       -> structural-gate-v6 [2, 0, 2, 0] then 1: "2 declared carrier(s) and 0 fixture(s) stand; no
                                            other foreign file" (total/unclassified/carrier/fixture). The gate is an ALLOWLIST: Form (.fk
                                            .bml .form .bmf), the C seed (.c .h .m .metal), documents and data (.md .mdc .txt .json .jsonl
                                            .tsv .rows .dat .html .svg .png .wav .bin .f32 .safetensors .lora .tiktoken .plist .out .example
                                            .ptx) and a few extensionless names (.gitignore, .gitattributes, LICENSE, copied-at, the
                                            bundle's signature files) may stand; any other file (.awk .fish .tcl .R .jl .groovy .cs .dart
                                            .ex .nim, an uppercase .SH, an extension of its own, none) is foreign and refuses landing without
                                            a declared role, and so does a Form or C file that opens with #! (a document or data file may open
                                            with #! as text). It reads the tree git holds (tracked, and untracked that git does not ignore,
                                            never .git or .claude) in one listing; a live plant of .awk, .SH, a .fk opening with #! and a
                                            .cfg read [6, 4, 2, 0] and 0, a .txt opening with #! stood (2026-10-04). The carriers are named
                                            by exact path with their reasons in gate/structural-gate.fk; fixtures are evidence.
                                            `gpu/fptx-matvec.ptx` is data, not a script: the Form-emitted PTX (sm_80 floor) the seed's CUDA
                                            lane opens
gate/tests/structural-gate-band          -> 262143  (every closed kind and the uppercase forms, #! in every kind, a scratch git repository
                                            with tracked, untracked, ignored, gone-from-disk and .claude files and a plain directory read by
                                            find, and the old list-of-scripts reading as the plant)
gate/tests/structural-heal-carrier-band  -> 127
form/form-stdlib/tests/form-cli-build-band -> 16383  (a request that does not read (`{bad json`, a typo'd key, a key of the wrong kind, an
                                            array) is refused by name with exit 1 before any flow, never the default install, which
                                            regenerates for minutes; the native form-cli's identity folds against independent SHA-256 answers, the
                                            closure door's seal agreeing with the fold, every way an attestation can be wrong, the
                                            publication lock's refusals, the platform name, the proof's helpers, the roots list; the build
                                            itself is the drift gate's form-cli-build row, `printf '{"out":"<path>"}' | ./fkwu
                                            gate/form-cli-build-run.bml`, and `{"prove":"<path>"}` is the executable's behavioral proof:
                                            identity, exact answer bytes, stale rows refused, embeds and their cap, a 1546-row index, a
                                            request-bound dual HMAC receipt and its one-use replay)
observe/tests/review-panel-band          -> 127  (observe/review-panel-run.bml, the reviewer panel as a Form door: no reviewer is asked by
                                            the band, a closed or missing door is skipped with its row, a door name the table does not hold
                                            (`env` stands on PATH and would have printed the environment as its answer) and a request that
                                            does not read are refused by name with exit 1)
./fkwu observe/door-link-health-run.bml  -> docs=41 claims=924 broken=0 (each broken claim named on its own line; 2026-10-04)
                                            then prelude-reach missing=0 untracked=0 shadow=0
                                            (every name a cell loads reaches one tracked file)
./fkwu observe/band-truth-run.bml        -> bands=384 readable=207 unreadable=177 absent=0 seen=1 flaws=0, exit 0
                                            (each band's declared full read against the most its claims sum to, and
                                            the queue and manifest copies of a full against the head pin, with no
                                            band run, in about 0.3 s; a decimal or count fold that stands on purpose
                                            says `; FOLD: decimal|count` on a head line; a sweep that lists no band
                                            is a flaw too; 2026-10-03)
form/form-stdlib/tests/band-truth-band   -> 1048575
form-stdlib/tests/spawn-guard-band       -> 131071  (form-stdlib/bml/spawn-guard.bml: every door of the body that begins a child from an argv or
                                            a shell line another cell or an agent supplied reads it first, in process, with no child to read it:
                                            ["sed",..] by its first element, through the wrappers command-words knows, `sh -c` by the line it
                                            carries (sh, bash, zsh, dash, ksh, ash, csh, tcsh, fish), a shell given a script file by the file's
                                            text, a bare shell by its input, also behind a wrapper or by `-` and `-s` (`["env","bash"]` fed
                                            `sed p f` is refused), pythonw as python; the census classes are read too: no file called `fixed`
                                            holds a forbidden tool as a quoted command word. A refusal is a
                                            named ending: hch-run-with answers [nothing(), "", "birth refused: forbidden-tool sed: not used
                                            here; <way>"], the control language answers nothing=forbidden-tool:sed (use the edit door,
                                            read_file, ./fkwu cells and BML), the pid natives -5, host_capture and host-exec nothing(). A warm
                                            argv reading is about 6 us (5000 in 32 ms), a `sh -c` line about 90 us (500 in 46 ms; 2026-10-04,
                                            this Mac). Doors that ask: host-child, form-token-verbs, form-cli-heal-native-io and -process,
                                            findings-requests, form-cli-code-circle, native-source-inventory, native-pipe-darwin, metal-ask,
                                            crossing-record; tests/fixtures/spawn-doors.txt names every other cell that calls a host native
                                            that starts a child and says why its command is fixed words (class fixed, table, seed, rented), and
                                            the band computes the same set from the tree so a door that asks nothing and is in no row fails it;
                                            plants: the reading with wrappers or basenames off, an unclassified door, an ask removed from its
                                            source)
./fkwu observe/forbidden-tools-audit-run.bml -> the commands agents ran in their own Bash and Monitor calls, read from their transcripts
                                            (~/.claude/projects/<project>/<session>.jsonl and <session>/subagents/*.jsonl): per session and
                                            agent the commands counted, the flagged ones and their first 160 bytes newest first, then a totals
                                            line and a note; the last line is the lens reading (flagged + 1). stdin
                                            `{"breath":1,"since_hours":N,"session":"<id>","scope":"all","top":N}`, every key optional and a
                                            request that does not read refused by name (exit 1). `{"breath":1}` is the land cadence's lens: it
                                            reads the window from the stamp the last breath left (`.hearth/forbidden-breath`; the first breath
                                            reads six hours), opening only files written inside it, and says `forbidden-tool commands since
                                            last breath: N (<from> to <to>)`; 2 s a breath where the whole read costs 94 s and grows, and
                                            deleting old transcripts does not move it; the total is `{}` on demand. Read in chunks by
                                            str_find over the bytes with the command string read in place (no row parsed into nodes); a tool_use
                                            id counts once across transcripts. 2026-10-04: this session's 130 MB transcript with its 44
                                            subagents in 8 s (3839 commands in the main transcript, 314 flagged), every transcript of this
                                            repo's checkouts (404 sessions and agents, 70136 commands, 19194 flagged) in 80 s. It is the fourth
                                            lens of the land cadence's readings beside the gates (`reading forbidden=<flagged+1> moved|steady`
                                            with the note under it)
form-stdlib/tests/forbidden-tools-audit-band -> 65535  (the breath window opened at a stamp or six hours back, files older than the window not
                                            opened, the door as a child with a scratch stamp (first breath, second breath, only a breath moves
                                            it), refused requests that leave the stamp as it was; a synthetic tree of two projects, three sessions and one subagent: eleven
                                            distinct commands, seven flagged, with a duplicated tool_use id counted once, the Read tool's row and a
                                            tool_result quoting a Bash call as escaped text adding nothing, a command whose key is not first, an
                                            escaped quote, a window on the row's own time, scope and session filters, 4096-byte and 100-byte
                                            chunks reading what one read reads, a 20 MB transcript with 600 commands, the door as a child, and
                                            two plants (no dedupe, no window))
./fkwu observe/walk-began-run.bml </dev/null  -> the land cadence's fifth lens (`reading walk=<hours> moved|steady`): the hours counted up since
                                            a walk door began (the latest row of `.hearth/walk.began` in the walk checkout and this one: a
                                            walk that began this hour reads 1, 0 stays no reading), with the note `hours since a walk began: N
                                            (<door> began <hh:mm> <today|yesterday|date>, M min ago)`, `; [walk-gap] no walk began since
                                            <hh:mm> <day>` after ten hours (the longest quiet between two slots is nine) and `; walk retry
                                            <hh:mm> <day>: <class> -> <what it did>: <word>` from the last row of `.hearth/walk-retry.jsonl`;
                                            a stamp that did not read prints no note and ends 0
./fkwu observe/walk-retry.bml </dev/null  -> the retry job's door (see the host-walk-retry-band entry): the schedule's second slot reads
                                            whether the first began and, when it did not, why; stdin `{"door":"scheduled-walk","dry":1,
                                            "now_ms":N,"zone_ms":N,"stamp":"..","lock":"..","launchctl":"..","launchd_log":"..","reports_dir":"..","record":"..","nap_ms":N}`
                                            names a slot by hand and reads and classifies without beginning a walk (read-only; it does run
                                            `launchctl print` when no launchctl text is given)
form/form-stdlib/tests/command-words-band -> 2147483647  (form/form-stdlib/bml/command-words.bml reads which commands a shell string runs, with
                                            no shell: quotes, separators, redirections, heredoc bodies, wrappers, sh -c / eval / trap / find
                                            -exec, substitutions, case arms, [[ ]] and (( )), function bodies, ANSI-C escapes, and the
                                            stream a shell is fed (bash <<'EOF', bash <<< "..", echo ".." | bash, source <(..)). Against the
                                            3813 distinct Bash commands of one session's transcript, read again after the streams, case,
                                            condition and function work: 318 read as running sed or awk, each confirmed by reading it, the
                                            same 318, and 3495 read clean, of which 12 name a tool only as data and 1 writes a script that
                                            runs sed later; four commands that read `<unknown>` (arithmetic and [[ ]] words) now read clean.
                                            Eight plants (no quotes, wrappers, heredoc skipping, -c recursion, basenames, streams, case
                                            patterns, conditions) each drop their claim. It cannot see an alias, a variable that holds the
                                            tool (`$T file` reads <unknown>, never refused: `$cmd x` is an ordinary zsh form here), a script
                                            file that runs sed inside, a runner such as uv run python, a pipe from a producer that is no
                                            literal (cat f | bash), the substitutions in an unquoted heredoc body, or a wrapper its list lacks;
                                            a reading nests at most 256 levels and then answers <too-deep>)
./fkwu observe/belief-stamps.bml         -> 70065000  (field stamped*10^6 + owed*10^3 + laws; 2026-10-01)
observe/tests/belief-rewitness-band      -> 63
./fkwu form/form-stdlib/release-ledger.bml -> open=17 moving=0, then 17000000 (2026-10-04)
learn/tests/homecoming-distillation-corpus-band -> 32767
value-eq-arena-band 31 · import-carry-band 255 · form-cli-author-high-band 4095
closure-lines-band 31 · sort-band 31   (the queue's two gaps, written by the local lane; 2026-10-01)
memory-governor-band 67108863 (form/form-stdlib/bml/memory-governor.bml asks the whole machine, not one process, before a
                                Qwen session opens, a renewal allocates a second KV state, a walk turn begins or the planner
                                opens its voice; the weights themselves are one physical copy in the page cache for every
                                kernel that maps the file, measured 2026-10-02; a grant is a lease other processes see
                                (/private/tmp/form-memory-leases/<pid>.lease, one lock a slow holder keeps, the same weights file
                                counted once, pending bytes counted for 180 s with a renewal's second KV state stamped on its own
                                and given back, rows range-checked, a dead pid's file removed, an unmakeable directory guarded by
                                one reading); each defect planted and read below its head; observe/memory-governor-run.bml prints
                                the reading, the live leases and a lease-aware verdict)
form-cli-code-low-memory-band 1023  (a running coding lane saves its checkpoint and ends with host-memory-low when the machine
                                runs low, continues on a roomy or unreadable reading, and a lane that spent its turns keeps that
                                ending; the walk reads it as a choice point)
native-turn-ladder-band 262143 · local-flow-reading-band 4194303  (a memory ending, host-memory-low or host-memory-held:*, is no
                                tried approach and no option of the review; its checkpoint is the resume's target, with no failure text)
                                (the review's phase fold is standing: every option whose lane output this host holds, in its own
                                checkout or the walk's, is folded once into the ledger row with its nine phases, each one's share of
                                the wall, its renewals and the prompt IDs they re-fed; a lane killed at its deadline writes no counts
                                and its counts are folded from its stamps and said so; the day option reproduces to the ms)
host-walk-band 8796093022207  (a walk door that claims the checkout stamps its slot in `.hearth/walk.began` (one row per door:
                                pid, unix ms, door) under the lock it holds; a later start of the same door that finds its own
                                row under 25 minutes old (hw-slot-window-ms: the retry gap plus slack, far under the two-hour
                                windows and the eight hours between slots) ends 0 with `[walk-lock] <door> already began at <hh:mm>
                                (a retry slot finds the night begun; ...)` and claims nothing; the stamp marks began, not success, so a
                                start killed before its claim leaves none and a walk that crashed after it is not retried inside the
                                window (the next slot's heal and yield carry it); the other door's row is another slot; a hand start
                                inside the window is refused too and `{"again":1}` on the door's stdin starts it; claims `began`,
                                `began-plants` (a begin that never stamps, one that compares the window with the wrong inequality,
                                one that ignores the door, one that ignores the age each fail the checks the real begin keeps) and
                                `gap` (the lens over synthetic stamps and the door as a child);
                                the walk's turn waits for memory within the window and a held answer takes no turn;
                                the sync settles what the body knows through the reunion's rounds: a drawn page takes main's side
                                and is remade (in its directory) into one new commit, a river of at_unix_ms rows with both banks
                                standing is joined by union, each row once, and the sync says `healed:` before sync=0; a branch
                                of any length heals, only a round that comes back with the rebase no further on stops; a file
                                the body does not know, a river one side deleted, or a page it could not settle aborts the
                                rebase and names its files;
                                a walk runs the host-walk parts of its own checkout, so a checkout that stands behind main on a
                                conflict the landed sync heals cannot heal itself (2026-10-03 12:30: the day turn held every turn
                                on docs/rent-ladder.html): observe/walk-sync-run.bml runs the landed sync once on a named checkout
                                ({"dir":"/abs/path"}), leaves a checkout whose walk lock stands alone and names the owner; run on the
                                walk checkout at 14:30 it printed healed:, redrawn:, sync=0;
                                host-walk.bml is an aggregator over sixteen parts `host-walk-<seam>.bml`, each under 12000 bytes so the
                                local lane reads one whole and can read it twice, and every definition's name (not its body) is pinned
                                in tests/fixtures/host-walk-defs.txt)
host-walk-retry-band 4194303  (the informed retry, observe/walk-retry.bml over host-walk-evidence.bml, -class.bml, -retry.bml, -record.bml
                                and -attempt.bml: each schedule has a second launchd job, earth.hati.rent-walk-retry at 03:40 and
                                earth.hati.day-turn-retry at 12:40 and 19:10 (sources in docs/launchd), whose door first reads what became
                                of the first slot AGAINST THE SLOT'S TIME (a retry may start 5 to 45 minutes after its slot): its row in
                                `.hearth/walk.began` (no earlier than the slot less two minutes, no later than now) and the lock's owner
                                file say running (the stamped pid alive), finished (no owner, or another walk's) or crashed (the pid
                                gone, the lock never released); with no stamp (a first slot running older code) a live owner of the same
                                door that began no earlier than the slot is running, and launchctl `state = running` or the job's log
                                written in the slot's window say begun. Running or finished is the class begun (nothing to heal). Else
                                it reads the evidence of why through read-only argv doors: `launchctl print gui/<uid>/<first label>`, the
                                OS diagnostic reports of the slot's day under the home's Library/Logs/DiagnosticReports (the home from
                                the job's plist path, else $HOME; a report counts when its process launched between two minutes before
                                the slot and 25 minutes after, for the first job's own coalition, and carries a launch time and a signal
                                or termination), and the job's stdout log. Classes: launch-killed (a report with CODESIGNING, a Launch
                                Constraint Violation, Code Signature Invalid, an unreadable executable path or a SIGKILL within 100 ms; or
                                launchd's last exit reason naming CODESIGNING AND its `runs` advanced past the last retry row of that job:
                                launchctl keeps the reason after a run, so one whose runs did not advance, or that has no row to be
                                compared with, is stale and no evidence; or launchd's own log, `log show` of launchd's lines for the
                                label over the slot's first minute, saying `service inactive: <label>` when the stamp, the report, the
                                refusal and the job's log all say the job never spoke: a job that ran goes inactive minutes later, and
                                the 12:30 day turn of 2026-10-04 died this way leaving no report), crashed-after-launch (a stamp whose pid is gone with the lock
                                unreleased: the walk claimed the checkout and died; or SIGSEGV, SIGBUS, SIGILL, SIGABRT, SIGTRAP in a
                                report), held (the log's last `[walk-lock] ... does not start` line) and nothing-found. The change is a
                                named variable the row states and never more than two attempts: launch-killed begins the walk door as a
                                child of the live retry (a spawn inside a running process, not a launchd exec) and, if that dies within
                                five seconds, builds a kernel beside (cc fkwu.next, the nightly build's argv, renamed over fkwu: a new
                                inode, no new signature identity claimed; it keeps its name so the walk's coordinator still climbs to
                                launchd) and begins it there, then stops as retry-failed naming what each attempt saw;
                                crashed-after-launch begins the walk with `{"again":1,"safe":1}` (heal, sync, build, carrier and carry
                                stand; the turns, drafts, page and movement are skipped); held waits up to 90 minutes for the holder and
                                begins once; nothing-found begins once after a 120 s nap and keeps the launchctl text and the diagnostics
                                directory's names. Every attempt after the first tells the child `{"again":1}`; a child's output is read
                                from its last 64 KB (the whole stays under `.hearth/walk-io`). Each retry writes an INTENT row to
                                `.hearth/walk-retry.jsonl` before any child begins (retry_id, slot, class, why, evidence paths, launchd
                                line and its `runs`, plan) and an OUTCOME row at the end (what changed, each attempt with its outcome word
                                and what it saw, the outcome), so a crash inside the door or a kill of the retry job leaves what was
                                understood; the fifth lens prints the last row (an intent row alone reads "in progress or interrupted").
                                Every number the reading stands on is a named def pinned by the band. The doors read stdin through
                                hw-stdin: a terminal reads as none, so a hand start without `</dev/null` waits on no one.
                                Classification, state, plan, ladder, the child's end, the rows and the lens line are pure and run over
                                synthetic evidence (a copy of the shape `launchctl print` printed, reports of the shape the OS wrote at
                                2026-10-04 03:30:04), attempts over stubs in the walk door's place, the state over a scratch stamp and
                                lock; the plants (a classifier blind to its evidence or to the first slot's state or to the job's log, one
                                that trusts a stale exit reason, a state read against now or blind to the pid's liveness or to the door,
                                one plan for every class, a ladder that allows a third attempt, an intent written after the child) each
                                fail the checks the real ones keep; the repo's plists are read against the schedule's table.
                                ROLLOUT, read precisely: a launchd job runs the door code that stood on disk when it started, so the
                                first slot after this lands (the walk checkout syncs main only once it is running) runs the OLD door:
                                no stamp is written. The retry at +10 minutes then sees no stamp and reads the rest: launchctl `state =
                                running` or a live lock owner of the same door, which is begun (nothing to heal); a first slot that
                                already ended leaves its log written in the window, which is begun; only a first slot that left neither
                                is read for why. The walk checkout must hold observe/walk-retry.bml before the retry jobs are loaded
                                (observe/walk-sync-run.bml {"dir":"..."} on it).)
form-cli-landing-band 262143  (the landing and the walk read every child's exit from `host_wait` through
                                `host-child.bml` (`hch-run`), never from a printed mark; a red witness holds the landing; the
                                reunion after a push behind origin is argv in the checkout it joins and is read end to end in a
                                scratch origin: joined, refused with what git said, the side taken, and its default wiring)
host-os-membrane-band 8191 · bidirectional-framebuffer-channel-band final field 1
grammars/tests/form-eval-band 65535 · form-eval-full-band 635 · source-compiler-grammar-bridge-band 32767
pattern-match-band 511 · choice-lane-core-band 1023 · backtrack-band 255 · offer-ack-core-band 32767
control/tests/attempt-band 4095 · file-bytes-band 127
form-bml-cursor-full-band 105
control-invite-grammar-band 1023 · cell-serialize-band 1023 · json-band 1023 · wire-rpc-band 15
form/form-stdlib/tests/form-agent-tools-band.bml 4294967295 (2026-10-04: the sixteen resident agent tools over their JSON wire, grep among them)
form/form-stdlib/tests/findings-requests-band.bml 1048575 · form/form-stdlib/tests/form-cli-local-plan-band.bml 33554431
                                            (2026-10-02, rc 0: the feeder reads whether the code a request cites moved since its
                                            review, git hunks plus the closing gate and no model, and keeps the definition the
                                            change landed in; the planner proposes a request moot only when the voice's band names
                                            that definition and reads full today and below full under both stubs, and the row reads
                                            moot only while its fixborn stands; {"check":1} runs a moot row's kept band again; the
                                            dry planner's first pick is a standing request:
                                            echo '{"next":1,"dry":1}' | ./fkwu observe/local-plan-run.bml)
```

Every tracked cell's `witnessed:` stamp is read into the belief lens, oldest first; the re-witness door
(`observe/belief-rewitness.bml`) renews a stamp only from a fresh band run and reports a mismatch as a
lapse. The release ledger's open rows are the body's named work, each with its witness.

## The BML floor

A unit lowers by what it carries: any file with a `section [` block — `form.bml`, `form.lift`,
`form.action`, `form.route`, the `*.bmf` grammar dialects — travels through `bml-floor-compile`
whatever its extension, as a prelude or as the main file, and fkwu keeps the `.lowfk`/`.fkb` cache
beside it. Of 594 tracked `.bml` files, 528 carry a `section [form.bml]` block and three carry
`section [form.lift]` (`git ls-files '*.bml' | xargs command grep -a -l '^section \[form.bml\]' | wc -l`,
read on the tree that carries this line, 2026-10-02; the counts grow with each new unit). `true` and `false` are literals in the
dialect, and a nested `defn` is a registered function (the two nested-defn bands below).

The cursor (`grammars/form-bml.fk`, lowered by `form-bml-lower.fk`) is the compiler's one reader of a
`form.bml`, `form.route` or `form.action` section — the body, its imports' signature catalog and its
refusals all come through it. It reads a section whole, with direct backtracking, and rebuilds it with
the compiler's own constructors, so a def lowers to the very node its flat Form spelling builds:

```text
./fkwu observe/bml-cursor-coverage-run.bml -> files=1443 sections=1088 read=1088 stops=0 refused=0
                                              (read = sections: every `^section [form.bml|route|action]` line in
                                              the `.bml` and `.fk` files under the root, outside .git, .hearth,
                                              .claude, .cache, node_modules, target and dist; the counts grow
                                              with each new unit; read on the tree that carries this line,
                                              2026-10-02, in 152 s)
                                              a section it refuses is named by line, word and wanted rule
form-bml-cursor-full-band 105
```

The five grammar packs (`form/form-stdlib/grammars/{go,prolog,python,rust,typescript}-bmf.fk`) carry their
`import` lines above their `section [form.bml]`: the cursor reads `import` as a top-level statement,
never inside a section.

```text
bml-band 268435455 · bml-generics-band 16777215 · native-route-goal-cells-band.bml 1048575
nested-defn-scope-band 63 · nested-defn-closure-capture-band 63
bml-float-literal-band 2047 · bml-form-size-band 127 · cell-channel-band 4095
json-codec-bml-band 8191 · kernel-http-band 536965066 · channel-flow-band 8388607
circle-band 1048575 · static-to-dynamic-cells-band 262143
form-pe-coff-band 16383 · learn/tests/choice-receipt-band.bml 4294967295
language-packs-band 31 (2026-10-01) · bml-bmf-control-curriculum-band 1048575
bml-bmf-stream-curriculum-band 16777215 · form-cli-lens-mint-band 1023
```

Native source lifting is available through `form-source-lift(source, owner)` in
`form/form-stdlib/bml/form-source-lift.bml`. It checks BML candidates against the
original lowered tree before publication, preserving nested scope and literal
bytes. Existing BML is scanned through the BMF cursor and expression grammar;
surface-only migrations can compare exact recipes before imported type resolution.
Its contract witness is `form/form-stdlib/tests/form-source-lift-band.bml`
(35 accepted cases, 3 declined shadowing/binding changes, preserved comparison metadata; 2026-10-01). [Native coding](docs/form-native-coding.md#native-surface-lifting)
describes the callable surface and its scope.

## The mind and its voice

The voice speaks on this Mac's own metal: Qwen3.8-27B Q8_0 walked as Form recipe-data in the fkwu
session, every Metal pipeline Form-emitted and JIT-compiled at runtime, the geometry read from the
sealed GGUF header. The body takes engineering turns on its own through
`observe/native-turn-run.bml`, begun by launchd or by a session as each row's `coordinated_by` records,
and each option writes one row to `receipts/native-turn-ledger.jsonl`. `./fkwu
observe/local-flow-review.bml` reads that ledger live: the options, the gaps gone green and the gaps
still open.

The resident (`observe/form-cli-peer-contribution-live.fk`, the hearth) is one Form/Qwen/KV peer that
takes tasks from an append spool and returns length-safe durable results; it rests on its fifo bell
at idle and a `release` byte closes its model and state handles. Its admission deadline is one Form
row, `hearth-metal-deadline-ms` = 300000 (`form/form-stdlib/hearth.bml`), handed to the carrier
through `metal_deadline`.

```text
form-cli-peer-direct-answer-action-band    -> 8191
form-cli-peer-stream-ingress-band          -> 2097151
form-cli-peer-contribution-turnwheel-band  -> 33554431
observed-auto-learning-band                -> 32767
hearth-band                                -> 131071
receipt-texture-band                       -> 16383
lora-backward-band 511 · lora-adapter-band 31 · symbol-voice-band 63
```

The Llama-3.2-3B voice has an adapter for speaking Form in its own stream, `form/form-stdlib/adapters/llama-3.2-3b-form-tokens`:
55 of 64 held-out answers exact (53 of 62 once the two held-out rows that repeated a train question are set
aside; they train now) against plain 25 and taught-by-prompt 5 on the bare model, which is limited by the
continuation seam and does not isolate the training; the Qwen3.8 opening fit's separation 10.0 over 2.2 stands on
n=7 held-out sites and predates the filter that keeps held-out rows out of its sites. What it shows and does not:
`docs/native-model-control-plane.md`, "The Form-token adapter". Bands: `form-token-traces-band` 127,
`form-token-lora-band` 32767, `form-token-eval-band` 8191, `form-token-census-band` 4095.

The controls a voice writes in stream (`form/form-stdlib/bml/form-token-grammar.bml`: nine verbs, `recipe-import`
the newest) run in one trusted living workspace (`form-token-workspace.bml`): what a control makes persists in a
local, request, lineage or global scope, eval reaches every door the runtime carries and every function the body
defines, and each control answers within its deadline. Bands: `form-token-verbs-band` 4095,
`form-token-workspace-band` 4095, `form-token-grammar-band` 1023. The trace corpus is 596 rows in sixteen families.

### The GPU lanes

Every layer is a Form recipe first and then a carrier on a GPU, held to that recipe. Four light
Metal bands ran today:

```text
metal-door-band          -> 15
msl-families-mint-band   -> 4294967295   (every emitted MSL family mints on this Metal)
msl-lane-coverage-band   -> 31           (every -msl entry point has a band that enqueues it)
matvec-t-band            -> 127          (the transposed matvec and the rank-1 accumulate)
floor-lens-band 31 · floor-spread-band 31 · kernel-length-band 63   (the lens arithmetic, no GPU)
```

The lanes that open a model or hold the GPU. Each band declares its verdict in its header; the last
run of each lives in its receipt. Re-read on this checkout today: `dense-multi-band` 1023,
`native-lora-resume-band` 7, `ear-native-band` 32767, `voice-pass-band` 4095. The three `vk-*` bands
read their pins here (`vk-layers-live-band` 127, `vk-blocks-live-band` 127, `vk-train-live-band` 255): this
body carries no Vulkan runner and no GLSL-to-SPIR-V compiler, so `vk-door.bml` answers `vulkan-runner-absent`
by name and begins no process. What the bands hold is what is Form: the shaders the recipes emit, the plan
laid out one line per word, the recipes the reads would be judged by, and the reader of a carrier's answer.

| lane | band | last witnessed |
|---|---|---|
| Qwen3.8-27B handle, all 64 layers on the device | `qwen35-dense-token-handle-band` 2147483647 | `receipts/2026-08-30-reground-the-floor-answered-fresh.md` |
| Q8_0 prefill on the matrix unit; the 577-token prefill at 9.5–16.6 s of GPU, the same token | `q8-0-matmul-mma-band` 15, `qwen38-prefill-quant-band` 31 | `receipts/2026-09-11-named-pain-walked.md` |
| cooperative matvec twins on real weights | `q8-0-matvec-tg-band` 1023, `q6k-q4k-matvec-tg-band` 8191 | `receipts/2026-09-06-the-root-crosses-the-barrier.md`, `receipts/2026-09-06-the-loader-was-the-wall.md` |
| up to eight sequences in one decode step | `dense-multi-band` 1023 | `receipts/2026-09-09-the-body-had-a-mind-and-the-wrong-question.md` |
| half and bfloat element formats | `precision-lanes-metal-live-band` 2097151 | `receipts/2026-09-11-the-registry-resolved-and-the-host-that-decided.md` |
| Vulkan plan, shaders and reader (no runner in this body; the absence is named) | `vk-layers-live-band` 127, `vk-train-live-band` 255, `vk-blocks-live-band` 127 | `form/form-stdlib/vk-door.bml` |
| native LoRA with Adam moments resumed | `native-lora-resume-band` 7 | `receipts/2026-09-23-native-assessment-memory.md` |
| the ear: whisper-tiny as the body's own pass | `ear-native-band` 32767 | `receipts/2026-09-09-the-ear-could-not-hear-the-house.md` |
| the mouth: a whole VITS voice, phoneme ids to samples | `voice-pass-band` 4095 | `receipts/2026-09-09-the-shape-was-right-and-the-sound-was-empty.md` |

`form/form-stdlib/floor-lens.bml` is the lens arithmetic for what the hardware gives each lane
(bandwidth and arithmetic, reads with their spread); a hardware reading is worth taking on a quiet
machine.

## The knowledge lane

```text
form-knowledge-integration-census-band     -> 1048575
form-knowledge-source-search-band          -> 262143
form-knowledge-qwen-heldout-v3-eval-band   -> 65535   (every row current against its source sha, and
                                                       the dataset sha equal to its seal)
native-model-route-table-band 255 · nl-lexicon-grow-band 127
pivot-coverage-band 65535 · cognition/tests/error-absorption-kernel-band 4095
```

The v3 held-out lane (30 rows, every family twice, whole-normalized answers) is the body's
defined-correctness number. Its consent file (`.form-knowledge-qwen-heldout-v3-consent`) is a per-run
local act that git ignores. The model route is a Form data table, and a model file is found at
runtime by the `models <directory>` listing (`model-discovery.fk`), each file's own header read.

There is no sealing door. Each row's source sha, and each dataset's seal, are literals typed into
`form-knowledge-qwen-heldout-eval.fk` (v1, 15 rows) and `form-knowledge-qwen-heldout-v3-eval.fk` (v3);
the v3 band recomputes the source shas and the dataset sha and checks them. On 2026-10-03 every row
whose source had drifted was re-pinned AFTER its fact was read against the source, so those rows are
not blind any more: v1 `h01 h03-h07 h10 h12-h15` and v3 `v301 v302 v305-v316 v319-v321 v323-v328 v330`
keep their question and answer under a new source sha; the blindness claim now holds only for v1 `h02
h08 h09` and v3 `v303 v304 v317 v318`. Three rows had lost their sources and were replaced by rows read
from sources that stand: v1 `h11` (the four-way verdict) pins the conformance gate, v3 `v322` (the
deleted recipe's value) pins the conformance vectors, v3 `v329` (a count its document no longer
states) pins the same document's present count. The v1 dataset reads valid and the v3 band reads its
full, 65535; the dataset headers say so.

## The Form-native DeepSeek V4 chain

The chain is whole and it runs on the real file. Bands hold it model-free (`./fkwu form/form-stdlib/tests/<band> </dev/null`, the memory governor
reading room before each run, every exit 0; no band maps the model: the ones that read a real file read its header, an 8 MB window, and the recorded
logits are 1.5 MB): the tokenizer, the quant and tensor readers, the emitters, the reference arithmetic and the emission door, and the driver (driver
stages 1 to 6, below: the fixture, layout and bind on the CPU; the kernel set, the open, one layer, the head, prefill and the greedy loop, the whole
fixture model end to end on the device on the Form-built fixtures, the second in the tensor types of the real IQ2XXS file; the routed-expert arena; the
session and generate doors on a fixture with a vocabulary). The physical witness is the real run of 2026-10-03/04 on the 80 GiB IQ2XXS file
(`receipts/dsv4-token-runs.jsonl`, `docs/evidence/fkwu/dsv4-token-run-*.txt`): the routed-arena flow read the recorded logits at r 0.99813 with the
recorded argmax 2581 (our largest delta 1.145, inside the reference engine's own spread of 2.16), the teacher-forced raw-id stream at 23 exact, 1
tie-explained and 0 failed of 24, a cold first token of 2.6 s and a warm decode near 100 ms a token, no swap. THE LANE IS WIRED: `model-registry.bml`'s
`deepseek4` row reads `dsv4`, on, and `fcds-lane-wired?` in `form-cli-model-ds4.fk` is its twin, 1; `dsv4-door-band` fails when they differ. NO EXTERNAL
ENGINE IS PART OF THE BODY: the body spawns and queries none, its control-plane row for the lane is `challenger.deepseek-v4-metal` (the IQ2XXS build,
witnessed 2026-10-04) and the ask route (`ask-lane-router.fk`) carries it by `form-cli-model-ds4.fk`. What stays of `ds4` is the reference record the
validation was judged against (`ds4-logits-capital-of-france.json`, its prompt row, `ds4-recorded-references-band`, the notes on the order the reference
engine summed in) and the family tag in file and module names (`ds4flash`, `dsv4-*`).

THE LANE THROUGH THE DOORS (`native/metal/tests/dsv4-lane-band.bml` 4095 on a fixture with a vocabulary, `dsv4-session-plan-band` 31 on the real header;
no real file read or mapped by a band; the Metal bands need the carrier `form/native/metal/fk-metal-carrier.dylib` to exist in the checkout, built beside by the carrier line of AGENTS.md and never copied
over a live one: without it a fresh worktree reads `dsv4-door-band` 479 and `model-seal-band` 0). The session door (`fcms-open-reserved`, `fcms-generate`, `fcms-release-ok?`: the calls a Qwen session is asked
through) takes a deepseek4 file in this order: the admission queue, the seal (`<file>.form-seal`; the IQ2XXS file has none today and
`observe/model-seal-run.bml` writes it; the SHA-256 organ walks the file in 4 GiB chunks, each a no-copy view released before the next, because one buffer over a file past the device's maxBufferLength of 86,586,540,032 B cannot be made and the 86.7 GB build read an empty digest; the rate is 1.6 to 2.1 GB/s from a warm page cache, the compression being sequential on one core), the registry's lane gate, the chat template's ids from the file's own tokenizer (an empty prompt is no turn), the window check
(a window is cut to the limit, refused past the header's context_length or when it is no larger than the prompt), the driver's gap check from the header
(`model-driver-gap:<tensor>`), the governor's lease for the derived working set at the capacity (24.9 GiB shared: the dense tensors, an arena of 64
experts a layer, the KV; 8 GiB private; a denied ask reads `host-memory-held:<reason>` and opens nothing), the
routed-arena open (`dsv4-open-arena`: never whole stacks, which read the whole file in one token), prefill, then the greedy walk to a NAMED ending:
`eos` (the header's eos id), `max-tokens`, `context-limit`, `step-refused`. THE DRIVER SERVES POSITIONS 0 THROUGH 2047: a ratio-4 block refuses at position
2048 and later, where the indexer stops being inert and the file's 126 indexer tensors are unrouted. A window asked for is cut to 2,048 and refused past the
header's context_length; a walk that consumes position 2047 stops `context-limit` and its usage line says so (`usage: prompt_tokens=N generated_tokens=N
ending=E position=P capacity=C position_limit=2047`: the last position consumed, the capacity the session holds, and the driver's own limit apart), it
never runs into the refusal. A renewal (`fcmr-replace-with-profile-checked`) is Qwen's move over the same weights and
the same arena: a second KV state, the new prompt prefilled from position 0, the old state released, so a renewed stream has the whole limit again; a
prompt past the window is refused and the session stays as it was; a second state the driver refuses (`new-stream-allocation-refused`) or a prefill
that stops (`new-stream-prefill-refused`) ends the renewal by name with the first stream intact and the second freed. THE UNWIND: a stop inside a
DeepSeek walk (`fcms-generate-ds4-with`, offered under `try`) releases the context, the arena and the state and gives the lease back where they are owned,
and answers a session that is not resident, named `walk-stopped`. The release is the driver's own close (`fcds-release`, `dsv4-close`: the arena
included, in synced rounds) and the lease goes back. The generate door (`fcmg-ds4-door-with?`, the CLI's `generate`) is the same run in one shot and
reports `backend`, `mode` (`dsv4-native-generate`), `prompt_tokens`, `generated_tokens`, `ending`, `capacity`, `position_limit` and the text after `text:`. Not wired:
observations (`ds4-observation-not-wired`), the sampler, the draft and adapter lanes, the positions from 2048; the text holds whole characters (an
incomplete UTF-8 tail at the end of a walk is written `\xNN`) and the ids are the exact record. WHAT THE REAL SESSION RUN MUST READ (`observe/dsv4-session-run.bml` `{"go":1}`, the lead's run, after the seal): status `ok`, reason
`answered`, `prompt_tokens=14`, the first generated id 2581 (the recording's argmax for the 14 template ids), `generated_tokens=24` and `ending=max-tokens`
at `tokens` 24, `position=37 capacity=38 position_limit=2047`, released 1, the lease file gone, no swap and no pagein storm in the machine's reading, and one row appended to
the door's receipt file (named in the door's own head).

```text
tokenizer      dsv4-tokenizer-band 8191 · dsv4-tokenizer-native-band 15 · dsv4-token-recipe-swap-band 65535
quant, tensor  ds4-quant-layout-band 127 · ds4-tensor-table-band 255 · f16-decode-band 4095 · gguf-manifest-band 255
               iq2xxs-dequant-band 1073741823 · iq2xxs-msl-band 8191 · mx-plane-band 511 · mx-msl-band 511
               q2k-dequant-band 511 · q2k-msl-band 255 · q8-0-msl-band 255 · windowed-residency-band 4095
reference      dsv4-oracle-recipes-band.bml 65535   (the reference arithmetic against the body's carvers, the view
               geometry, the request admission, the header-only plan of 1,406 tensors, the retention and fp64 matrix
               doors, the one minting door of the native organs' owner ids, the pipe owner's flow key)
emitters       dsv4-compressor-band 2047 · dsv4-kv-cache-band 511 · dsv4-hc-band 63 · dsv4-hc-msl-band 63
               dsv4-moe-msl-band 63 · dsv4-forward-band 127 · mla-attn-band 63 · mla-msl-band 127
               moe-msl-band 511 · moe-route-wide-msl-band 255
stack          dsv4-proof-emission-band.bml 127   (eleven streams, 85 kernels compiled on the device, two run)
               ds4-order-match-band.bml 255       (the quantiser and the Q8_0 row dot against a Form reference of ds4's order)
references     ds4-recorded-references-band.bml 8191   (the logits pinned by a SHA-256 of the whole file, by the body's own door; their 14 chat-templated prompt ids derived by the body's
                                                        template over the IQ2XXS header and held to the row beside them)
driver stage 1 dsv4-fixture-gguf-band.bml 511 · dsv4-layout-band.bml 1023 · native/metal/tests/dsv4-bind-band.bml 511
driver stage 2 dsv4-kernels-band.bml 65535 · native/metal/tests/dsv4-open-band.bml 1023 · native/metal/tests/dsv4-layer-band.bml 4095
driver stage 3 native/metal/tests/dsv4-token-band.bml 255 · native/metal/tests/dsv4-end-to-end-band.bml 1023
               dsv4-lease-band.bml 511 · dsv4-door-band.bml 511 · dsv4-validate-band.bml 16383
driver stage 4 dsv4-fixture-q8q2-band.bml 255 · dsv4-kernels-q8q2-band.bml 1023 · native/metal/tests/dsv4-layer-q8q2-band.bml 4095
               native/metal/tests/dsv4-end-to-end-q8q2-band.bml 511   (stage 2 and 3 bands re-read 2026-10-04: dsv4-kernels-band 65535 with 105 kernels, dsv4-open-band 1023 with 76 in the graph, dsv4-kernels-q8q2-band 1023 over the same 76)
driver stage 5 native/metal/tests/dsv4-ledger-band.bml 2047 · native/metal/tests/dsv4-door-run-band.bml 127   (the op ledger of a token, whole-stack and arena ceilings, the attribution row, the flow through the door's functions)
               native/metal/tests/dsv4-token-run-band.bml 16383 · form-stdlib/tests/dsv4-token-run-plan-band.bml 127 · form-stdlib/tests/dsv4-token-run-validation-band.bml 255   (the token door's rows from the ledger, the tap, the plan from the header, the arena mode; the validation, teacher-forced and poison readings; the host's time columns, the segments, the hand-over cadence; the knobs and the arena's size in the plan)
               native/metal/tests/dsv4-arena-band.bml 2047 · form-stdlib/tests/dsv4-expert-cache-band.bml 16383 · form-stdlib/tests/metal-buf-fill-band.bml 31 · native/metal/tests/dsv4-arena-sim-band.bml 31   (the slot arena against the whole-stack control, the least-recently-used cache and its one-pass walks, the fill door, the picks replayed at other arena sizes equal to real walks)
driver stage 6 native/metal/tests/dsv4-lane-band.bml 4095 · form-stdlib/tests/dsv4-session-plan-band.bml 31 · form-stdlib/tests/model-seal-band.bml 2047   (the lane through the session and generate doors on a fixture with a vocabulary; the dry plan on the
               real header; the seal door)
```

Driver stage 1 (read 2026-10-03 WITA, each band alone, memory room before each run, every exit 0; each band's planted
defects read below its 511 and came back): `form/form-stdlib/dsv4-fixture-gguf.fk` writes a tiny DeepSeek-shaped
GGUF from the body's Park-Miller generator (3 layers, ratios 0 4 2, layer 0 hashed, 4 experts 2 used, n_embd 256, 86
tensors, 1,515,232 bytes, SHA-256 pinned) and the oracle's own admission accepts it; `form/form-stdlib/dsv4-layout.fk`
computes the widths, layer table, KV bytes a token and scratch sizes from a header alone, on the fixture and on both
real files (88,064 B a token over 43 layers; 1,202 driver tensors on each); `form/native/metal/dsv4-bind.fk` plans a
view per tensor and maps, reads, clips and releases the fixture's 86, while the real files' plans are computed from
their headers and map nothing. The reap25 header settles its layer count: `deepseek4.block_count` reads 43, blk.0 to
blk.42 are the layers, and 3 draft layers (`dspark.0` to `dspark.2`) follow in the table, so the control plane's 46
is 43 plus 3 (the comment beside the control-plane row is left as it stands). The fixture, layout and bind bands
need a binary fixture, host-local headers and a live Metal device, and are not rowed in
`form/band-verdicts.txt`. The driver's kernels must emit `#pragma clang fp reassociate(off)` and `contract(off)`
(each of the three cells says so in its header).

Driver stage 2 (read 2026-10-03 WITA on this Mac's GPU, each band alone, memory room and no model door resident before
each run, every exit 0; each band's planted defects dropped its reading and were restored): `form/form-stdlib/dsv4-kernels.fk`
composes the restored emitters and the kernels the graph still needed into ten Form-emitted units, every source opening with
the two fp pragmas (a*b+c on 1+2^-12 reads 973078528 with them and 973079552 without), 100 kernels compiled through
metal_pipeline once and each answering small inputs within the oracle harness's 3e-5 floor against the oracle's fp64
arithmetic on the fixture's own tensors (54 readings, 0 to 4.7e-7, plus the argmax line and the NaN-scan line); a NaN or an
infinity in a reading is a failure with its stage named, never a stop (`dkr-nd`, `dkr-nd-why`), and `form_dsv4_nan_count` with
`dkr-logits-clean?` counts the non-finite words of the logits by their exponent bits, which the driver must read as zero before
it trusts the argmax (the argmax skips a single NaN); `form/native/metal/dsv4-open.fk` opens the fixture as
handles in Qwen's context shape (86 views, 50 scratch buffers, 51 since stage 3 added the head's gate buffer so the logits are bs[28], a state of [kv, comp state, comp score, comp rows] a block) so
`q38-context-ok?`, `q38-state-ok?` and `q38-close` apply, counted through metal_live and released to 0 live in synced rounds,
and an open whose kernel the device refuses answers [] before anything is mapped;
`form/native/metal/dsv4-layer.fk` runs one block (hc_pre, MLA with the compressor, hc_post, the router by hash or biased
top-k, experts by device ids, the shared expert) and reads back at 1.1e-6 or less against the oracle's layer on all three
fixture blocks over eight positions (block 1 emits its second ratio-4 row at position 7, reading the shifted state; block 2
emits at 1, 3, 5 and 7), alone and chained (chained 2.2e-7 to 3.4e-5, gate 1.12e-4). The compressor has no independent oracle:
its legs are pinned to the author twin `dsv4-compressor.fk`; one cached element of block 2 differs from the twin by one f16
step (f32 and fp64 on opposite sides of a rounding boundary), which the band names and holds to the chain gate (stages 2.4e-5
to 3.2e-5 after it). The fixture's SwiGLU clamp is 10.0, 1.0, 2.0 (its pinned SHA-256 is now
b90181a115536345bbbe503098c1418d38403c9616555d98d343241ce1353b5e): it bites on blocks 1 and 2, `dkl-clamp` equals the
metadata, and a layer that read no clamp reads wrong. No carrier change was needed. Found on the way: `metal_pipeline`
compiles with fast math, under which `v == v` and `v != v` are not tests of a NaN (both the argmax and the scan test the bits);
and the restored router's softplus, ln(1 + exp(z)) in f32, loses its digits for negative z (probability relative error 2.7e-5
at z = -7, 1.1e-4 at z = -9, the whole value from z = -17 down, against the oracle). The new kernels `form_dsv4_hash_probs_s` and
`form_dsv4_topk_probs_s` use max(z, 0) + log1p(exp(-|z|)) with an eight-term series below 0.1: over z = -20 to +20 the
probability reads at most 9.2e-8 and the softplus 1.4e-7 (4.5e-8 at -7, 6.6e-8 at -9), the restored form stays in the band as the
control, and the layer routes with the new ones (block 0's router weights 2.8e-5 to 1.7e-7, its output streams 1.0e-5 to 4.9e-7;
block 2's output streams 1.48e-5 to 1.49e-5, flip-dominated). The real IQ2XXS file's Q8_0 dense path and Q2_K
down experts were wired in stage 4 (below); `n_hc` other than 4 is refused.

Driver stage 3 (read 2026-10-03 WITA, memory room and no model door resident before each run, every exit 0; each band's planted
defects dropped its reading and were restored): `form/native/metal/dsv4-token-handle.fk` runs the embedding, every block, the
output head (stream rms, F16 `output_hc_fn`, the head gates, the stream sum, `output_norm`, the MXFP8 `output.weight` into the logits
at bs[28], where Qwen's sampler reads them) and the device argmax, which is trusted only after the NaN scan; prefill is token-major
with the head asked once, at the last token. On the fixture the head reads 3e-8 to 4e-7 against the oracle's own head arithmetic;
prefill and the stepwise path agree bit for bit; the whole model (a prompt of four tokens and eight greedy steps, ids 27 24 29 6 17 15
16 3) agrees with the oracle's full forward at every step (nd 1.2e-6 to 6.9e-5 against the tilt-measured bound max(8 ne, 3e-5) =
3.2e-4 to 9.1e-4, the argmax equal with a margin of at least 100 times the disagreement), the run twice is bit-equal, and every
block alone at all eleven positions reads 1.9e-7 to 1.9e-6: chained, the model amplifies f32 noise (steps above the 3e-5 floor), which is its
conditioning and not a kernel's. Not independently witnessed: the compressor (the oracle has none; its leg is pinned to the author
twin), the fixture's own clamp values, and the head's choice of epsilon (the fixture holds `eps` and `hyper_connection.epsilon` equal).
`form/form-stdlib/dsv4-lease.fk` derives what a DeepSeek session asks the governor for, from the header (dense bytes + 64 experts a layer
+ KV; derived, not measured): 27,593,273,180 B for the IQ2XXS file and 31,060,815,708 B for reap25 at 8,192 positions, against a wired cap of
82,463,372,040 B that the files themselves (86.7 and 91.3 GB) exceed; `fcms-weights-bytes` and `adm-admit` charge it for a deepseek4
file and file_size for every other (the Qwen file reads 29,047,086,048 exactly). `form/form-stdlib/form-cli-model-ds4.fk` seats DeepSeek
at the generate and session doors (the chat template's ids from the file's own tokenizer, the lease, the arena open, prefill, greedy steps to a named
ending and close: see the lane paragraph above); a DeepSeek session refuses observations.
`observe/dsv4-validate-run.bml` is the validation window: its default (empty stdin, `{}`, `{"dry":1}`) prints its plan
(24.9 GiB lease, the stages and asks, the abort rules, the references) and opens nothing; only `{"go":1}` runs it, and while the header names
tensors the driver cannot run it maps nothing and writes a refusal row. (At stage 3 the plan printed eleven gaps for the IQ2XXS file: the
Q8_0 dense tensors, the Q8_0 `output.weight`, the Q2_K down experts and the ds4 order-match units; stage 4 below closes the first three and
turns the fourth into a printed note.) The lease's belief that only touched pages wire is derived, not witnessed:
if whole expert stacks wire, the set is about 85 GB, above the 82.46 GB cap, and the window's wirespan probe is the witness. The doors, as they read after this change:
`./fkwu observe/door-link-health-run.bml </dev/null` -> docs=39 claims=831 broken=0 (prelude-reach missing=0 untracked=0 shadow=0),
`./fkwu observe/band-truth-run.bml </dev/null` -> bands=356 readable=178 unreadable=178 absent=0 seen=1 flaws=0 (the five stage-3 bands are read: 351 -> 356, 173 -> 178).

Driver stage 4 (read 2026-10-03 WITA 04:36 to 04:44, each band alone after the 03:30 walk had released the GPU, memory room before each run, every
exit 0; each band's planted defects dropped its reading and were restored): the IQ2XXS file's tensor types run in the driver. `form/form-stdlib/dsv4-fixture-gguf.fk`
writes a second fixture, the first one's 86 tensors and seeds with every MXFP8 tensor Q8_0, every gate and up stack IQ2_XXS and every down stack Q2_K at hidden
256 (1,778,144 bytes, SHA-256 eb93eed0aaf888853f74e18329919fe35f8f6db5ab61caf7363272824287b0e1; the first fixture is byte for byte b90181a1...1353b5e still).
`form/form-stdlib/bml/dsv4-oracle-reference.bml` admits, prices and reads types 8 and 10 in fp64 (`dfo-ref-q80`, `dfo-ref-q80-out-a`, `dfo-q2k`), each by the
tensor's own type, and the fixture band holds it to the body's carvers (`q80-flat-at`, `q2k-at`): nd 0 on every row. `dsv4-kernels.fk` puts the Q8_0 unit in
`dkr-graph-units` and adds `form_dsv4_q2k_matvec_id` (101 kernels, 72 in the graph); `dsv4-layer.fk` chooses the dense kernel (`dkl-dense`, `dkl-dense-grouped`)
and the expert kernel (`dkl-expert-kernel`) from the type the tensor's own table row carries, and refuses a block type its first dimension does not tile;
`dsv4-token-handle.fk`'s head accepts a Q8_0 `output.weight`; `dsv4-layout.fk` holds the one table of the types each role carries and `dvl-driver-gaps` reads
the header's gaps from it. On the second fixture: each kernel alone 9.1e-8 to 3.1e-7 against the oracle (the Q8_0 lane, the one-thread form, the grouped
output factor, the Q2_K stacks by ids, IQ2_XXS by ids on layers that were MXFP4); every block alone and chained within the gates (the layer band's stages alone
read at most 7.8e-7, chained 2.4e-7 to 7.2e-6); the whole model, a prompt of four tokens and eight greedy steps, within the tilt-measured bound at every step (nd
3.7e-7 to 8.8e-6 against bounds of 3.1e-4 and up), the run twice bit-equal, every block alone at eleven positions under the 3e-5 floor (1.2e-7 to 1.3e-5). The walk this fixture takes is degenerate (token 3
at every step), so its argmax is weak evidence and its logits are the evidence. Plants: the IQ2_XXS kernel routed to a Q2_K stack, the MXFP8 kernel to a Q8_0
tensor, the Q2_K scale nibble read as the min, the Q8_0 block stride 33 for 34 each read far from the oracle at a kernel and, planted into the context, at a
block (the control and the restored kernel inside the gate). Through the validation door the IQ2XXS header now reads NO gap (dry, `./fkwu observe/dsv4-validate-run.bml
</dev/null`: "the driver cannot run yet: nothing named by the header"); a doctored header reads one line a tensor type. The ordering of ds4's own lanes is
a printed note and not a gap: ds4 quantises a Q8_0 matvec's activation to int8 per 32-block (ds4.c:7051, :6814) and sums Q2_K in a plain ascending f32 order
(:3480), and the driver's lanes read f32 activations, so a near-tie token may differ. What the window reads of that, by leg: the STREAM leg (the 24 greedy ids for the raw prompt, which needs no extra input) matches
ids and now records, per step, OUR top-two logit margin and how far the recorded id's logit sits below our top (`stream_margins`, `stream_recorded_gap` in the
receipt), so a flip is told from a near tie; the reference's own tie budget (its top-two margin against our largest delta over its top 64) belongs to the LOGITS leg,
which runs on the recording's 14 chat-templated prompt ids (pinned in the row beside the recording, below; a request's `logits_prompt_ids` replaces them, and the leg waits only when neither holds). The `ds4q8` and `ds4q2k` units are what to wire if the
readings say so. An expert type no kernel reads is an explicit refusal (`dkl-expert-kernel` reads nothing; the FFN answers [0] before any dispatch). What bounds the
driver today, the physical window having run (below): positions below 2048 only. The 126 indexer
tensors (`indexer.attn_q_b`, `indexer.proj`, `indexer_compressor_{kv,gate,ape,norm}`, on the 21 ratio-4 layers) are present in the file and unrouted, and a ratio-4 block
refuses at position 2048 or later (the indexer stops being inert there). The kernels' multi-block rows (4 and 8 blocks a row of Q2_K and IQ2_XXS, shared and per-slot
input) read 1.2e-7 to 2.6e-7 against the oracle on a third file, the second fixture's rows being one block; the Q8_0 and Q2_K plants split into a wrong number (Q2_K scale as the
min, Q8_0 codes unsigned, an IQ2_XXS kernel on a Q2_K stack: finite, far) and not a number (a Q8_0 stride of 33, an MXFP8 kernel on a Q8_0 tensor: non-finite). The doors after this change: door-link-health docs=39 claims=845 broken=0 (prelude-reach missing=0 untracked=0 shadow=0), band-truth bands=360
readable=182 unreadable=178 absent=0 seen=1 flaws=0. Found on the way: a recursive peak that calls itself twice a level takes 2^n steps (64 values never returned), so the band's peak is one fold.

The first physical window (receipts/dsv4-validation.jsonl, 2026-10-03 06:47:57 WITA, run by the lead alone) stopped itself after 988 ms at the state stage: wired +198,148,096 B against an estimate of 18,270,208 B, over
twice max(estimate, 64 MiB). The price was right and the sensor was not (read 06:50 to 07:19 WITA, each band alone, memory room before each run, every exit 0). From the header's 44 ratios and key length 512 with plain
arithmetic (`dsv4-layout-band.bml` bit 512): 43 raw arenas of 131,072 B (5,636,096), the compressed rows (17 of 2,048 B on each of 21 ratio-4 layers, 1 on each of 20 ratio-128 layers: 772,096) and the compressors'
state and score (32,768 B twice on the 21, 262,144 B twice on the 20: 11,862,016, the same at any capacity) make 18,270,208, which is `dvl-kv-total(m, 64)` to the byte (now the sum of `dvl-raw-total`,
`dvl-comp-rows-total` and `dvl-comp-state-total`); at 8,192 positions 824,068,096. The device's own ledger agrees (`dsv4-open-band.bml` bit 512, metal_live word 18 = currentAllocatedSize, which no other process moves): the
state at 1,024, 8,192 and 65,536 positions grows it within 13,312 B of its price and frees back to the byte, and the first dispatch in a process (the init kernel) moved wired memory by 0. What moved wired memory is the
machine: it is every process's, and on this Mac a sibling not yet named moves it by 306 to 328 MB every second or so, up and down (900 readings 10 ms apart: 7 steps over 16 MiB, 5 over 128 MiB; spreads over 1 s from 0.7 MB
to 963 MB; a stage that does no work read +0.6 GiB, and a 3 s reading was 19.8 MB then 331.9 MB: it is bursty). The lease did not move (`dsv4-lease-band.bml` bit 256): shared 26,787,475,292 B and the 8 GiB margin, 35,377,409,884 B at
64 positions, 805,797,888 B more at 8,192 and only the capacity-bound terms grow. The window names the movement and subtracts none of it: every process on this machine is ours, so `dvr-organs` reads the process list through the
body's door (`host_processes`: pid, resident bytes) and names the organs of ours resident by command (the dry plan prints them, every stage line carries them with the ones whose resident bytes moved by 64 MiB or more, and the receipt
holds `organs` and `moved`), and the rule that guards is the lease: wired above what the window holds, past the baseline, with host-memory-low and a denied ask (`dsv4-validate-band.bml` bit 2048). A stage's wired growth against its price is
read and told (OVER twice its price, with the organs that moved), never an abort of its own, and the wirespan probe's kind (slice, between, stack) is recorded. Every stage line and receipt row carries the device's own bytes
(`own_delta`) beside the wired growth. The compile and map estimates are bounds (512 MiB, and 512 MiB plus the scratch plan), not readings: the window's +309,854,208 B and +43,286,528 B are the first measurements of them.

The second window (receipts/dsv4-validation.jsonl, 2026-10-03 07:29:28 to 07:30:04 WITA) passed compile, map (own device bytes +86,284,566,528: the 80 GiB mapping), state, wirespan ('between') and layers 0 to 15 on the real file and
ended at layer 16 on ask-denied:compressor with the reading that denied thrown away; 13 s later the compressor read 6.3 GiB against a cap of 32. Field 8 of `host_vm_stat` is the pages the compressor OCCUPIES; the pages it STORES are
vm_stat's ('Pages stored in compressor', 27.9 GiB then), read by `dvr-stored-pages`. A denial is now a reading carried whole (the receipt's `denied`), and a denial for reserve or compressor is waited out with the governor's naps (10 s
doubling to 60 s, 5 minutes an ask at most, never past the window's 20) before the run ends (`dsv4-validate-band.bml` bit 4096). Wired memory followed the weights a layer's dispatches touch and returned them (layer 0 +3,207,380,992 B,
layer 1 -1,368,260,608 B, layer 14 -2,233,942,016 B; net +2.55 GB over 16 layers), so a layer is priced at the bytes of the tensors its dispatches can touch, whole stacks included: layer 0 3,018,191,320 B (the largest), layer 1
1,959,129,560 B, the 43 summing 85.7 GB, which is why they are asked one at a time and never summed (bit 8192). The validation window no longer stages the layers (a sync and a machine reading a layer is a probe protocol, not the
token path); it keeps compile, map, the arena, state, wirespan, the head and its argmax, and the recorded references. The token path is measured by `observe/dsv4-token-run.bml` (`form-stdlib/bml/dsv4-token-run.bml`;
dry by default, `{"go":1}` runs; run once on the real file with whole mapped stacks, 2026-10-03 19:04:25 to 19:05:21 WITA, below): one row a token read from the landed ledger (`dll-line`: dispatches, barriers, command buffers, waits, host reads, buffers made, bytes, device busy, ms) with the machine's wired growth, pageins and
the device's own bytes, the plan priced from the bare header by the ledger's own formulas (`dll-token-count-m`, `dll-token-bytes-m`: 1,856 dispatches at the quietest position and 2,082 at the loudest, 22 command buffers), and wired
memory read after EVERY LAYER of the first three tokens without a sync, through the one function the token handle calls at its seams (`dkr-hz-tap-set`; no tap is installed in production), stopping the token before the machine is at
risk (host-memory-low, wired above 60 percent of memory, available under the reserve, 20 minutes) with the curve in the receipt (`native/metal/tests/dsv4-token-run-band.bml`, `form-stdlib/tests/dsv4-token-run-plan-band.bml`).

Driver stage 5, the flow held to the op ledger of the Swift reference runner it was learned from (git history 93154a720; a lesson, not a dependency; read 2026-10-03 WITA 09:30 to 10:50, each band alone, memory room before each run; `docs/dsv4-flow-history.md` is the map of where the
whole flow stood: the Form cell of 08-10 held the stream at 226 ms a token warm, the Swift runner's 34 to 38 ms was never a Form number, and neither was ever in main). Measured on the fixture before this change (a machine at load average 33 to 43, best of six reps of 16 tokens): the 137
dispatches of a token cost 9 to 10 ms of host, 66 to 76 us a dispatch, where Swift pays 1.1 us: `md-bind` built an eight-word binding in 36 us, `dkl-supported?` ran 700 us a block three times a block a token, `dkl-clamp` scanned the header 230 us twice a block, and the NaN scan made a buffer,
three syncs and a free a token. Now: `dkr-bind` (4 us, the same bytes), the admission read once a context (`dsv4-admitted-once?`), the clamp once a block, the scan in the token's own batch behind one sync, and `dkr-hz` in `dsv4-kernels.fk`: a concurrent batch whose
barriers are planned once a block from the reads and writes each kernel's own MSL signature declares and replayed, a batch handed to the queue every second block by default (`submit_every` 1: every block), the real file's four-row IQ2_XXS and Q2_K expert kernels and ds4's Metal-order Q8_0 matvec (`dkr-unit-lanes`: 105 kernels over
eleven sources, 76 in the graph, the wide hyper-connection matvec among them). `form/native/metal/dsv4-ledger.fk` reads one row a token and a row a block from the carrier's counters. On both fixtures a decode token reads 137, 142 or 148 dispatches (the layout's formula, held at every step),
111, 116 or 121 barriers (81%), 2 command buffers, 1 wait, 2 host reads, 0 buffers made; the serial walk and the concurrent walk are bit-equal in ids and logits; the best of six host encodes is 3.3 to 3.5 ms for 137 dispatches (24 to 26 us a dispatch, 2.7 to 3 times
under the 66 to 76; 17 to 20 us at a quieter hour). Derived for the real file, not run: 1,856 dispatches a token, about 1,500 barriers, 22 command buffers, a host floor of 45 to 50 ms (33 to 37 at the quieter figure) overlapped with the device. Not witnessed: any real-file number, any wall-clock gain at real dimensions, the Swift runner's rate on
this Mac today. The session's open and step and the generate door's one-shot run on the fixture through the door's own functions (`dsv4-door-run-band.bml`; the fixture has no vocabulary, so `fcds-text` renders none instead of reading pieces
at offsets a missing array gives; the whole door with a vocabulary is `dsv4-lane-band.bml`).

THE REAL FILE'S FIRST TOKEN, over whole mapped stacks (the token door, 2026-10-03 19:04:25 to 19:05:21 WITA, memory read each stage; the lead's run, the receipt and stdout in the land-int checkout): prefill position 0 read 1,848 dispatches, 1,461
barriers, 22 command buffers, 1 wait, 0 host reads, 0 buffers made, 8,544,878,408 B of weight to read and the device busy 167,726 us (the compute is hardware-class) in 49,272 ms of wall clock: pageins +5,241,090 pages (16 KiB each: the WHOLE 80
GiB file read in one token), wired only +2.2 GiB, and 1,816,908 pages (27.7 GiB) of other processes' memory pushed to swap; the door held at token 1 on host-memory-low, as it must. Binding a whole 0.55 GB expert stack makes the device read every page
of it a dispatch names (read, not wire), so a cold token costs the file and the machine, the pattern that restarted the OS before. THE ROUTED EXPERTS ARE THEREFORE READ THROUGH A SLOT ARENA (`native/metal/dsv4-arena.fk`, the door's default,
`arena_slots` 64; 0 is the whole-stack control): the routed stacks are not mapped; each layer holds a bounded set of experts in buffers allocated once (the lease's expert term to the byte: 19,478,347,776 B on the IQ2XXS file) and an
expert the router picked and the cache (`dsv4-expert-cache.bml`: least recently used, keyed by layer and expert) does not hold is filled from the file by its own span through the FILL DOOR (`metal_buf_fill(buf, boff, [path, foff, len])`: the host
preads the span straight into the slot, no Form string, no mapping, no dispatch; carrier `fk_metal_buf_fill_external` in `form/native/metal/fk-metal-carrier.m`, mode 33 of fkwu's leaf door in `runtime/fkwu-uni.c` with its rewrite row, every refusal answers -1
(metal_buf_write's convention beside it) and writes nothing, a path holding a NUL byte included, `metal-buf-fill-band.bml` 31). Measured 2026-10-03 on a Qwen file's 2 MB spans (2,162,688 B): the door 113 us a span with the pages cached (19 GB/s) and 566 to 578 us for spans not cached (3.7 GB/s: the disk); the
road it replaced (a no-copy view of the span and a copy kernel, still taken when the carrier lacks the door, bit-equal in the bands) 726 us with the pages cached (3 GB/s: the device faults the file's pages in one by one, 31 us for the view alone), and any
road through a Form string 4.2 to 5.0 ms. The SAME expert kernels read slots by slot number, bit-equal to the whole-stack control stage by stage and end to end on both fixtures and on both roads (`dsv4-arena-band.bml` 2047). A SLOT IS VALID ONLY AFTER ITS FILL SUCCEEDED: every failure after the cache took its plan (the slot numbers not written, a refused or short fill, a fill that stops)
forgets the slots the plan placed, and the band holds three planted failures to a bit-equal rerun on the same cache and the stale plan itself to a differing one; the pin rule is held end to end on a two-slot walk (the pure model over the logged routing
reads the arena's counters and keys, and a victim rule that ignores pins does not). A block is two planned stages with ONE sync between them, a plan boundary of the token's concurrent batch (`dxa-boundary`:
`dkr-hz-sync`, one counted read of the router's ids, the cache's slots, a batch armed again, the fills): a token waits blocks + 1 times, reads blocks + 2, commits blocks + blocks / 2 + 1 command buffers, makes no buffer, and its row carries the span fills,
their copy dispatches (none through the fill door), the road they took (`road=door|view` on the row line, `fill_road` in the receipt row, the fills through each road in the summary), the experts hit and missed and the bytes filled; the ledger pins both modes' ceilings (`dsv4-ledger-band.bml` bit 512: `dll-arena-ceil`). The plan prices them from the header: the device reads 9,107,652,444 B a token (16.6 ms at the stated 546 GB/s) hit or
miss alike, and the file gives the dense tensors faulted in on the first token (the open maps them and reads no byte: 7,281,557,340 B the token reads plus 1,059,053,568 B of token_embd's other rows, a bound view being read whole by the floor's
theory, which the lease does not price: it leaves token_embd out as one row), at most 1,826,095,104 B for a cold decode token, at most 25,565,331,456 B for the 14-id prefill (84 lookups x 43 layers x 7,077,888 B: the
worst case, which the slots do not bound; the arena is a separate 19,478,347,776 B) and only the missed experts warm (7,077,888 B for one at layer 0). A checkout takes the fill door by rebuilding fkwu (`cc -O2 -o fkwu runtime/fkwu-uni.c`) and the carrier (the dylib line in AGENTS.md): a file that names `metal_buf_fill` needs the new fkwu, and a carrier built before the door still loads (the door
is an optional symbol) and runs the view road. WITNESSED ON THE REAL FILE (the token door, 2026-10-03 21:36 WITA, `docs/evidence/fkwu/dsv4-token-run-2-routed-2026-10-03-2136.txt`): the 14-id prefill and 24 greedy tokens completed through the arena with every handle released, cold first
token 9,334 ms, warm decode 287 ms a token (3.47 tokens a second), the device busy 6,298 ms over 38 tokens (about 166 ms a token, constant), the arena's experts hit 6,590 and missed 3,214 (67 percent), 9,642 fills all through the fill door.
WHERE THE DEVICE'S 166 MS GOES (the attribution, `dll-attribute-decode`; and the kernels band, claim 16384): the hyper-connection F16 matvec (hc_attn_fn, hc_ffn_fn, output_hc_fn: 24 rows of 16,384 columns, 87 dispatches a token) ran as the compensated
kernel with ONE THREAD A ROW, 1,450 us a dispatch on synthetic buffers of that shape = 127 ms a token; ds4's own wide dispatch (`form_dsv4_f16_matvec_wide`, in the lanes unit) reads 10 us and 2e-7 off it. The wide kernel is the default
(since the switch commit, on the real recording of 2026-10-03 23:03 to 23:20 WITA, `{"hc_matvec":"narrow"}` the control: see the next paragraph): the layer band's fixture gate (3e-5 against the compressor's twin) does not hold with it, so the oracle bands run the narrow control and hold the wide kernel to a stated looser bound. `{"go":1,"logits":1}` reads the head's logits after the prefill against the pinned ds4 recording
and every step's margin, the recorded stream's raw prompt turns it on by itself, and `{"attribute":n}` runs n decode tokens in the attribution mode (every dispatch its own command buffer,
the device's microseconds tallied by family, kernel and block) and keeps the tables in the receipt row. WITNESSED ON THE REAL FILE (2026-10-03 22:36, the lead): the attribution read 1,856 dispatches and 182.4 ms of device a token, `form_dsv4_f16_matvec`
87 dispatches 133.8 ms (73 percent, 1,537 us each) and the Q8_0 matvec 302 dispatches 19.1 ms; the logits read r 0.9974, top-10 overlap 8, argmax 2581 (the recorded one), the reference's top-two margin 10.37, our largest delta over its top 64 0.923.
THE PASS RULE IS EVIDENCE: the argmax equal and recorded, finite logits, the tie budget (the margin over twice our delta: on the narrow run 10.37 over 2 x 0.923 = 1.85, on the wide run 10.37 over 2 x 1.1448 = 2.2896) and a delta within the spread ds4 shows against itself (2.1636, corpus row 973, one point of the REAP25 file: an order of
magnitude, not a bound); r and the top-10 overlap are readings, not floors (a first guess of r 0.999 and overlap 9 had called this correct engine a failure). The compare is linear, a cursor a pass: 129,280 values in 0.2 s where the first real run took 20 minutes
(an index and a length asked at every element: 17.7 s a pass of the stream step alone).
WITH THE WIDE KERNEL ON THE REAL FILE (the lead's windows, 2026-10-03 23:03 to 23:20 WITA; `docs/evidence/fkwu/dsv4-token-run-4-wide-attribution-*`, `-6-wide-logits-attribution-*`, `-7-wide-rawstream-*`, `-3-narrow-logits-attribution-*`): the device 182.4 -> 48.4 ms a
token (the 87 dispatches 133.8 -> 1.2 ms), the logits r 0.99813 (narrow 0.99742), top-10 overlap 9 (8), argmax 2581, our largest delta 1.1448 (0.923) under ds4's 2.1636 and half the margin 5.18: PASS. THE WIDE KERNEL IS THE DEFAULT; the narrow kernel is the control
(`hc_matvec` "narrow"; the layer, end-to-end and kernels bands run it for their oracle comparisons and hold the wide kernel to a stated bound with a broken wide planted). The whole run: 38 tokens, cold first token 2,626 ms, warm decode 101 ms a token (9.85 tokens a second),
the device 43 ms a token, hit 67 percent, no swap, every handle released; the rest of the wall (about 58 ms a token) is the host. THE RAW-ID STREAM free-running read 2 of 24 matched at a margin of 0.0264 (a near tie flipped by ds4's int8-activation order): that is not a
test of the engine, so `{"teacher":1}` FEEDS the recorded ids and judges each of the 24 positions on its own (exact, tie-explained by the validate door's one rule `dvr-flippable?` at twice ds4's spread, or failed), and the free-running mode names the first divergence
with its gap and whether a tie explains it. THE READINGS DEPEND ON NO HISTORY (the scratch is poisoned, not trusted): a first all-wide band read block 2's heads 0.73 and a later pass 2.6e-4, which looked like a kernel reading stale memory. It was the band's own short-circuit (claim 16's chain
of `&&` stopped at its first leg outside the gate, so block 2's positions 2 and 3 never ran and position 4 read a state without them). `dso-poison` fills every scratch buffer of a context with a word (a quiet NaN, 1e30) before a token: the layer band's wide legs, run
again on fresh states, then NaN-poisoned, then 1e30-poisoned, leave every buffer each leg wrote (both halves of the block, 24 legs) bit-equal to the first pass; the wide matvec's output pre-filled with zeros, NaN, 1e30 or its own last answer comes back the same bits (kernels band
claim 32768, with an accumulating plant caught); a walk of the token door under `{"poison":1}` (and 2) reads the same ids, prefill logits and readings on both fixtures, and a planted kernel that reads stale data (the shared expert's SwiGLU a no-op) is refused or differs. The fixture's block 2 compressed legs under the wide kernel read
heads 2.6e-4 where the control reads 3.2e-5: a stated bound (1e-3), not a hazard. Poison does NOT cover the state buffers (the KV arenas, the compressed rows: zeroed at allocation) nor the arena's expert slots (the slot buffers hold experts, not scratch: their bytes are held to the file's own by the arena band's span claims). The real runs under poison 1 and 2 (`docs/evidence/fkwu/dsv4-token-run-9-poison1-*`, `-10-poison2-*`, 2026-10-04 00:07 WITA) read r 0.9981291300866604, overlap 9, argmax 2581, delta 1.1447735229003904: identical to the unpoisoned run to the last digit; the teacher-forced run (`-8-teacher-*`, 2026-10-03 23:42) reads 23 exact + 1 tie-explained + 0 failed of 24.

WHERE THE HOST'S 58 MS GOES (step 3, 2026-10-04; branch `claude/dsv4-host`). The 24 decode tokens of the 23:43 window, wall against the experts each missed (`observe/dsv4-arena-sim.bml` reads the fit from the receipt): 0.397 ms of wall for each expert missed on top of 94.6 ms with none; the
misses ran 14 to 129 a token, about 40 warm, so the fills cost a warm token about 16 of its 110 ms and the fixed 94.6 less the device's 43 is the host's own 51 ms. Each ledger row now carries the host's time in columns (`dsv4-ledger.fk`; the clocks are the plan record's `tm-` keys): `sync_ms` (the waits: the host
idle, the device running), `plan_ms`/`plan_us` (a layer's id read, cache and slot write), `fill_ms`/`fill_us` (the span fills: a pread's wait on the disk is wall and not CPU), `enc_us` (the CPU of the stages' encoding), `read_us`, `cpu_us` (the process's CPU) and `other_ms` (the wall no column names); the summary line reads the warm token's mean of each, and the receipt's rows and summary
carry them. Measurable on the fixture: every column, and the cost of a dispatch's encoding (12 us of CPU, 5.9 of it the binding and the enqueue); not measurable there: the real file's balance of them (the fixture's device is 5 to 20 ms and its misses are three experts). FOUR CHANGES, each bit-equal to what it replaces on both fixtures: (1) SEGMENTS (`dkr-seg`, `dsv4-kernels.fk`): the runs
of a block's dispatches that read the same buffers with the same constants every token (the hyper-connection pre, the norms and dense projections, the output projections, the router, the whole second half over the arena) are captured at a stage's first replay (the pipeline by name, the context's handle met at every replay) and replayed with the plan's barrier words without running the layer code:
the encoding CPU of the 7 decode rows falls 41 to 43 percent on a quieter hour (the band's own prints, fixtures 1 and 2: 7.3 against 12.8 ms and 8.4 against 14.2) and 27 to 34 percent under a load average of about 8 (15.4 against 21.2 and 15.6 against 23.8: the CPU clock of a loaded machine reads more of everything) with the same ids, logits, readings and counts; a stale segment (block 0's tuples in block 1's place) is planted and seen, and so is a position-dependent dispatch moved into a segment (the query's rope, the cache append: they read other ids or logits); the segment and plan keys name the context by its residual and argmax buffers (two contexts, or a context closed and another opened, never share a key: banded); `segments` 0 is the control; (2) THE ONE-PASS CACHE (`dsv4-expert-cache.bml`): the step asked `len` and `xs[i]` of a list at each index, quadratic in the slots (a take
cost 528 us at 64 slots, 1,208 at 128, 1,859 at 192, 10 ms of a real token at 43 layers); now 130, 197 and 212 us, the indexed walks kept in the band as a reference and held equal over random sequences (band bit 4096), and the cost banded (bit 8192: a take at most 400 us at 64 slots and 900 at 256, at least 4 times under the reference at 256, which fails the same ceiling: 177 and 202 us against 2,577); (3) THE HAND-OVER CADENCE (`submit_every`, the door's option, 1 or 2 only: another value is refused by name before anything is planned or opened; default 2 as before): 1 hands the open batch to the queue after every block so the device runs a block's second half while the host
encodes the next block's first half (the structure's command buffers become blocks + blocks + 1, 87 for 43, against 65); NOT MEASURED ON THE REAL FILE: one window at 1 against the 2 the receipt's rows carry; (4) THE ARENA'S SIZE IS DECIDED in the dry plan: 64 slots (18.1 GiB, 25 percent of the layer's 256 experts). Every row carries the experts its layers picked (`picks`, on by default), and `observe/dsv4-arena-sim.bml` replays them through the arena's own cache at other sizes (proven equal to real walks row by row on both fixtures
at 2, 3, 4 and 6 slots, `dsv4-arena-sim-band.bml`). Row 10 of the receipts (the host-tip window, 2026-10-04 02:12 WITA, `docs/evidence/fkwu/dsv4-token-run-11-host-*`) carries them: a warm decode token misses 54.1 experts at 64 slots, 50.3 at 80, 49.0 at 96 and 48.9 flat from 128 to 256: most misses are COMPULSORY (an expert met for the first time in the window), so 36 to 72 GiB more arena buys about 5 misses a token; THE SIZE STAYS 64.
TWO FITS of wall against experts missed, both kept: 0.397 ms an expert on 94.6 ms with none (the 2026-10-03 23:43 window, quiet) and 0.495 ms on 319 ms (row 10, a load average of about 8 and GPU contention from our glass organs: contention-affected, the warm token read 345 ms there against 101 to 111 quiet). The dry plan prints the LEASE each size asks (dense + arena + KV + 8 GiB: 32.9 GiB at 64 slots, 87.3 at 256) and the governor's verdict on it against the machine as it reads then
(wired + need within 60 percent of memory, available - need over the reserve: 256 slots is REFUSED for wired-cap); the lease follows `arena_slots` (it leased 64's bytes whatever was asked). NOT DONE, and why: fusing the small kernels (hc_split with the weighted sum, 87 dispatches, saves about 1 ms a token and
moves five pins), patching the position-dependent constants of the rope, the append and the attention into segments (about 9 percent of the encoding left), the shared expert's early hand-over to overlap the fills (at most about 4 ms: its 4 dispatches read about 27 MB a layer). The device's 43 ms is bandwidth: the Q8_0 matvec 302 dispatches 18.3 ms at 60 us (the attribution above), not host time.

Bands that read a binary fixture through `read_file_slice` or a host door run on fkwu like every band;
`printf '{"list":1}' | ./fkwu gate/band-sweep-run.bml` prints each band's staging and pins. The restored emitters, the
`*-real.fk` cells, `dsv4-token.fk` and the windowed residency emitter say in their own headers whether a band proves
them or they are restored for the spec and not run physically. The reference evidence
`docs/evidence/fkwu/dsv4-oracle.json` is the reading of 2026-09-11 and is left as observed; the sidecar
`docs/evidence/fkwu/dsv4-oracle-reread.json` names which of its pinned identities still match.

The recorded references are `form/form-stdlib/tests/oracles/ds4-logits-capital-of-france.json` (129,280 logits, argmax
2581 "We" at 36.7579117) and the 24-token stream for the raw prompt ids [671 6102 294 8760 344] (a period-7 cycle,
" Paris. The capital of France is"). They were earned on the IQ2XXS file by the reference engine, which could read it; the registry's `ds4flash`
row names the reap25 file, which that engine could not, so they judge a native lane on the IQ2XXS file and a reap25 lane is judged by
the Form reference. The radius is one prompt, a cycle, never past position 127. The logits' own prompt ids are the row
`form/form-stdlib/tests/oracles/ds4-logits-capital-of-france-prompt.json` (read 2026-10-03 WITA through `./fkwu form/form-stdlib/tests/ds4-recorded-references-band.bml </dev/null`
-> 8191 and `dsv4-validate-band.bml` (then 2047), the governor reading room before each run, every exit 0): the 14 ids [0 3476 477 260 11502 22896 128803 671 6102 294 8760 344 128804 128821],
which are BOS, ds4's default system "You are a helpful assistant" (5 ids), User, the prompt "The capital of France is" (the raw prompt's 5), Assistant and `<think>`. `form/form-stdlib/bml/dsv4-chat-template.bml` derives them
from the IQ2XXS header alone, finding each marker by its bytes in the file's own vocabulary (BOS equals the header's `tokenizer.ggml.bos_token_id`) and detokenizing back to the template's text; the template is ds4's own
(`ds4.c:36195`, the default system at `ds4_cli.c:1767`, thinking on at `:1774`) and the count it gives, 1 + 5 + 1 + 5 + 1 + 1, is the recording's `"prompt_tokens":14`. The same code gives the token counts
the official DeepSeek API reported for three prompts with no system text (21, 18, 27). What the records do not say: the recording names neither the prompt nor the flags, so the prompt is read from the
commit that took it (`09c69ba8e`, "for the same prompt") and the corpus rows 939 and 948, and thinking on is the default path (a `</think>` ending, thinking off, has the same count and differs in the one last id, 128822; the recording's argmax "We" is, in the commit's words, ds4 opening its
reasoning, which fits `<think>`, and no flag is recorded). The dry door prints the ids and the leg's ask in place of the old WAITS line. Plants (each read a different list or no ids): a wrong id, a wrong count, a missing BOS in the row
(the recorded-references band's bits 1024 to 4096 and the validate band's 1024). The kernels the order-match band runs
carry `#pragma clang fp reassociate(off)` and `contract(off)`: `metal_pipeline` compiles with the default options, and
without them the eight-thread Q8_0 twin read 8 ulps from the one-thread kernel.

## The senses

What the body hears, says and perceives, read by bands that open no microphone, speaker or model:

```text
ear-heard-tongues-band   -> 63      the tongues the ear listens for are the tongues the glass offers
whisper-shape-band       -> 127     whisper's dimensions read from the model file, npz or safetensors
perception-symbols-band  -> 8191    the room, the ear and the voice's manner as addressed symbols
perception-rows-band     -> 65535   a perceived day folded into trainable rows; words travel only
                                    from a frame naming its speaker and an organ of this body
own-word-band            -> 65535   a sentence about the room stands only when the record backs every
                                    claim and it makes at least one
voice-say-band           -> 131071  one mouth per tongue the ear renders, the map proven in silence; the native
                                    mouth is the only mouth (a tongue without tables says "no native mouth yet")
voice-g2p-band           -> 32767   letters to phonemes from the body's tables: taught line, taught words,
                                    model-bridged words, else nothing
voice-g2p-bridge-band    -> 32767   the model's IPA spoken in the voice's symbols, learned from the taught pairs and
                                    measured on held-out words (exact / edit: id 99.57% / 0.05%, en 83.9% / 2.76%,
                                    pt 80.8% / 2.68%, fa 9.4% / 25.7% — Persian's taught phonemes are a guess
                                    from unvowelled text)
host-doors-band          -> 131071  host_spawn_at, host_alive and fs_mkfifo: the ear's lanes stand
                                    with no shell
host-signal-band         -> 127     host_signal pid sig: any signal to one process, so a worker that will not
                                    hear SIGTERM is ended with SIGKILL and no foreign program (voice-track,
                                    the host walk's deadline). A fkwu built before the door reads "door absent"
host-fs-band             -> 32767   leaf-door modes 35-49, one syscall each, Form names in bml/host-fs.bml: chmod, symlink,
                                    link (the atomic create-if-absent publish), sync, getenv, mkdir with a mode, localtime,
                                    os, file holders (Darwin libproc), file mode (with the type nibble), file copy (a fixed
                                    1 MiB buffer), file identity, utimes, realpath, pwrite. A fkwu built before them reads "door absent"
cell-run-band            -> 524287  cell_run(path, arg, deadline_ms, stdin) (leaf-door mode 50): a unit of Form run in this process answers
                                    {value, out, diag, stopped, errors, resident, ms, cpu_us}, held to the child it replaces on five
                                    tree bands and the fixtures (print, stop, unresolved name, stdin, .bml), with name windows, residency,
                                    a deadline, nesting and growth. PENDING THE SEED: proven on a temporary seed, the patch waits for
                                    runtime/ (docs/in-process-cells.md); a fkwu without the door reads "door absent" (0)
voice-track-reap-band    -> 7       the supervisor ends a wedged worker: SIGTERM, then SIGKILL after the grace
```

`own-word-band` proves that a claim contradicting the record is set aside; a real room has not yet
offered one. The cells are `form/form-stdlib/own-word.bml` and `form/form-stdlib/perception-rows.bml`;
the doors are `observe/say.fk`, `observe/voice-mouth-lanes-run.fk` and `observe/voice-pass-run.fk`.

### The native voice track

A text is cut (`voice-chunk.bml`), its words become phonemes from the body's tables or its own T5 on Metal
(`voice-g2p.bml`, `native-t5.bml`), the VITS voice renders each piece (`voice-pass.bml`), the master tones, compresses,
roomes and sets it to -30 LUFS on the device (`voice-master.bml`), and the body's own ear scores it back
(`voice-score.bml`, `voice-measure.bml`). Doors: `observe/voice-track-prepare-run.fk`, `voice-g2p-grow-run.fk`,
`voice-track-run.fk`, `voice-master-run.fk`, `voice-score-run.fk`, `voice-measure-run.fk`. No foreign program runs
anywhere in it; the model weights are data (`~/models/g2p-byt5-small-100/model.safetensors`, converted from the
PyTorch file by `observe/torch-bin-convert-run.fk`). These bands open Metal and a model: each takes the machine-wide
GPU lease (`gpu-lease.bml`) itself.

```text
voice-chunk-band          -> 4095     a text cut for a mouth that breathes: 22 words most, breaks after punctuation
voice-pass-band           -> 4095     a whole VITS voice, phoneme ids to samples, held to onnxruntime's numbers
voice-pass-old-export-band-> 255      the older exporter (serial node names) opens by structure; Indonesian renders
voice-pass-leak-band      -> 127      a render gives back every buffer: live count flat across renders
native-t5-band            -> 65535    a T5 grapheme-to-phoneme model on Metal, equal to the torch numbers it was
                                      proved against; no end-of-sequence token by default
torch-bin-band            -> 15871    a PyTorch checkpoint read as data: zip, allowlisted pickle, tensors from file ranges
safetensors-write-band    -> 15871    the body's own weight file, written whole or not at all, every byte verified
voice-master-band         -> 1048575  filters, shelves, compressor profiles, room, fades, K-weighted loudness, WAV
voice-score-band          -> 4095     whisper round trip per piece: WER, CER, and two readings of Persian joiners
voice-measure-band        -> 511      loudness, range, peaks, pauses, centroid, F0 and level jumps on the device
gpu-lease-band            -> 16383    one Metal client at a time: owner, staleness, tombstone takeover
gpu-window-band           -> 511      a GPU window: peers paused and resumed only by their holder, swept when the holder is gone, proven by a gauge
role-singleton-band       -> 255      one working process of a role: the elder stays, the younger yields, a stopped or dead elder is no role
```

A timed run on a shared device reads the device's weather: the same kernel measured 2x to 12x apart from minute to minute while a resident model
door, an ear lane or a glass lane held the GPU, and a fixed FMA loop falls from 12.7 to 4-5 TFLOP/s in two seconds of sustained load. The GPU window
(`gpu-window.bml`, door `observe/gpu-window-run.bml`) is the body's organ for it, on the lease above and on `host_processes`, `host_process` and
`host_signal`: it takes the machine-wide lease, names the peers the lease reads from the process table (an fkwu whose command line names a Metal
door, never itself or its parents), records them as `pid:start` rows in `<lock>/window` before the first SIGSTOP, waits for the device to settle (two
gauge readings within 10 percent) and to come back to 80 percent of the best gauge this checkout has seen (`.hearth/gpu-gauge-ceiling`, at most 90 s), and
opens on a gauge: a fixed FMA loop on the device, best of three, in TFLOP/s x 100. Its close is the holder's alone (the window file names the lease
token), reads the gauge again with the peers still stopped, sends SIGCONT to exactly the recorded rows that are still the same process (a reused pid is
another process), and prints one line: `gpu window  PROVEN | NOT PROVEN | NOT HELD  gauge a -> b`. A window whose holder died, or that is older than
20 minutes, is swept by the next open or by `sweep`. `printf 'with <cell>\n' | ./fkwu observe/gpu-window-run.bml` runs a cell in-process inside a
window (`cell_run`, no child process). A number printed beside NOT PROVEN is a reading of the weather. A peer already stopped by someone else when a
window opens is not recorded and not resumed by its close (the process row's run state says it is stopped). It replaces the shell helper whose stop was
ownerless (one client's stop thawed another's window; a killed client left the household frozen for two days).

A process row (`host_process pid`) is `(pid ppid exe cwd argv start-ms withheld state)`: `state` is the host's own word for the process, 1 running or
sleeping, 2 stopped, 3 a zombie, 0 not said (macOS `p_stat` of the process table row, Linux the letter after the name in `/proc/<pid>/stat`). A stopped
process exists, answers `host_alive`, and does nothing; the question a side process asks about another is whether it WORKS, and that is this field.
`role-singleton.bml` is one live process of a role by the process table (band `role-singleton-band` 255): a door (the file an fkwu runs, exactly
`./fkwu <door>`) has one working process, the elder (earlier start, then the lower pid) and a younger yields at its next check; a stopped or dead elder is
no role. The glass's three frame producers (`form-glass-sensors-live.fk`, `form-glass-machine-live.fk`, `form-glass-organs-live.fk`) yield to an elder at the cadence they
already check their carrier (every 2 s): six glass stacks had run at once, eighteen producers (160 percent of a core, 5 GB) publishing the same shared-memory
frames, and six readers of the one accelerator gauge that is consumed by its reading made it flip between 0 and 44 with no GPU work.
`observe/process-census-run.bml` names the doors that run, how many of each work and how many stand stopped, and marks a door with more than one DUPLICATE.

Measured on the sleep text (the body's own whisper, taught text word / character error): en 2.2% / 1.2%,
pt 4.8% / 1.4%, id 2.1% / 0.3%, fa 13.8% characters (48% by words). Unseen sentences: en 6.1 / 2.9, pt 8.5 / 2.4,
id 3.7 / 0.5, fa 48.9 / 17.2. Receipt: `receipts/2026-10-03-the-voice-spoke-without-leaving-the-body.md`.

### The lowering contract

The native loop lane (`runtime/fkwu-uni.c`, `fk_f64_*`) inlined a callee by substituting its arguments, so an argument
the callee never read lost its effects once a recipe was hot; a guard stands in the seed with its shrink path in its
comment. The contract is written in Form: `form/form-stdlib/bml/anf-lower.bml` binds every argument, in order, before
its call. Judge a detector by a plain build of the committed source, not by a shared binary.

```text
anf-lower-band                 -> 2047    a defn becomes ordered rows; what cannot be named answers nothing()
anf-shadow-band                -> 31      thirteen scalar defns lowered through rows to a native page, equal to the walker
arg-order-lane-parity-band     -> 16383   effect order equal across the cold walker, the Form evaluator and the hot lane
                                          (16356 on a build without the guard)
form-lower-contract-band       -> 255     uncovered tags decline, division by zero declines, compares read as compares
hot-recipe-effects-band        -> 63      a call with an effect runs, read or not, cold or hot (52 without the guard)
bml-demand-jit-guard-band      -> 511     the demand JIT declines what it cannot name and answers negatives signed
ignored-argument-scan-band     -> 7       a lint for the bug class: an ignored argument bound from a call
```

## The Glass

One persistent process paints a retained terminal frame from the body's own observation:
`./fkwu observe/form-glass-run.fk`. Views `a t o m f j s k v n d`, `h` for help; `r` focuses the room;
`1`–`4` and `0` choose tongues; `l i e c q g y w` act (`d` opens the body's choice points, `g` steps,
`y` takes, `w` holds); `z` puts the ear to sleep or wakes it. The ear stands awake with the glass unless
`.hearth/ear.slept` stands, and every frame opens with the ear's own state.

Nothing on a glass path forks, scans a directory or parses a tool's output. Telemetry crosses as gift
frames — POSIX shared memory with a seqlock header — and a value crosses as itself (`node_gift_write` /
`node_gift_read`). Every `fkwu` writes its own live page (`/fg-k<pid>`: dispatches, nodes, strings,
heat, boxes, CPU), so any process reads any kernel's hottest defns with their source. When the host
offers shared memory every kernel opens one field store, so a cell interned in two processes is one
cell with one word; `observe/field-reset-run.fk` starts the field over when no other kernel is alive.
A frame standing is not a giver giving: a frame not given for three of its cadences reads silent, and
`observe/body-vitals-live.fk` watches every surface's own rhythm and effort.

```text
form-glass-live-band 2147483647 · form-glass-observer-band 67108863 · form-glass-dashboard-band 16777215
form-glass-event-loop-band 16777215 · form-glass-staged-startup-band 262143 · form-glass-launch-band 131071
form-glass-gift-frame-band 4095 · form-glass-sensor-rows-band 2047 · form-glass-machine-band 511
form-glass-telemetry-membrane-band 2097151 · form-glass-vitals-band 1023
form-glass-standing-band 63 · form-glass-crossings-band 16383
form-glass-events-channels-band 32767 · form-glass-wait-band 255 · form-glass-kernel-view-band 511
form-choice-flow-band 16383 · sense-discernment-band 1023 · node-gift-band 4095
cell-store-band 255 · field-band 255 · kernel-census-band 2047
```

The events-channels, wait and kernel-view bands hold live children and timed rests: they read full
one at a time on a quiet host (`/tmp/form-glass-organs-band` is a path every run shares) and low under
load. The reading that counts is one taken on a quiet machine.

## The string floor

`substring(s, start, end)` answers the bytes of `s` from `max(start, 0)` up to `min(end, str_len(s))`;
`""` when that range is empty or reversed, or when `s` is not a string. `str_find(h, n, from)` answers
the byte index of the first occurrence of `n` at or after `max(from, 0)`, or -1; an empty needle
answers `max(from, 0)`; a start past `str_len(h)` answers -1. Both always answer, neither moves an
offset to a character start, and both are natives with their recipes kept in `core.fk` as the body's statement of
what they mean ([`docs/str-find-one-meaning.md`](docs/str-find-one-meaning.md)).

```text
substring-one-meaning-band 4095 · str-find-one-meaning-band 8191     (both drift-gate rows)
kernel-laws-band 2147483647     laws 1-9 of docs/kernel-interface.md (a drift-gate row)
core-substring-equivalence-band 2047 · substring-native-band 511
line-grammar-search-equivalence-band 8191 · core-str-find-equivalence-band 2047
meaning-codes-table-band 255
```

## What a melt gives back

fkwu's melt compacts the cons heap and then frees what no root names. It frees two more things than the heap.

A dead string's bytes become a hole in the string arena (`fk_hole_add`), the melt orders and joins the holes, and
`fk_sintern` places a new string in a hole that fits and discards its scratch. Live strings never move and the
arena's top never retreats. Before this, a dead string's slot came back and its bytes did not: the glass organs and
sensors processes held 6.8 and 7.5 GB behind ten thousand live strings, growing 200-300 KB/s, because the supervisor
starts them once and keeps them for its life. `kernel_stat(64)` reads the top, `kernel_stat(65)` the bytes in holes;
`string-holes-band` reads 127, and 115 on a build that takes no hole (`-DFK_HOLE_LOOK=0`). A dead string's slot owns
no bytes, so a stale word naming one reads length 0 where it used to read the old bytes until the slot was reused.

A melt costs the live heap, not the shared field. With the field on, every word a field node or pair holds is a
shared string (`fk_field_share_value` shares each one; `fk_smark` takes no shared string into a local mark), so the
string melt walks nothing of the field. `kernel_stat(69)` reads the melts this kernel has run, `(70)` and `(71)` the
shared string bytes and slots claimed (the field readings once sat on 65 and 66 and hid the holes and the closure
rows behind them). `melt-cost-band` reads 15: at least eight melts, a mean under 400 ms, what the churn kept intact.

A closure row lives while a root reaches it: the value stack, memory cells, records, nodes, held lets, the method
table, and the captures of a row that is itself reached. A melt also keeps the rows made since the melt before it
(`FK_CLO_YOUNG`: a row survives exactly one melt unrooted), copies only what a kept row captured, slides those values
down and hands the other rows to `fk_clo_free`; a row's index is the fn-value's own word and never changes, so a row
handed back and taken again is another closure under the same word. The mark walks what a row captured from an
explicit worklist, never one C frame per link: a chain of four million closures each capturing the one before is a
band claim. Before this, every evaluation of a capturing lambda kept what it captured for the life of the process: a
hermetic glass loop held 4.25 of its 4.36 million surviving pairs in closures, the heap doubled to 2^29 pairs in the
live glass, and each melt moved gigabytes of the machine's memory; the same loop now holds 125 thousand pairs flat in
a 1 M-pair heap and 87 MB. `kernel_stat(66)` reads the rows standing, `(67)` the rows reclaimed, `(68)` the captured
values held, and `kernel_live` word 21 the melt count. `closure-reclaim-band` reads 1023 and `closure-callee-band`
7; their plants (compile flags) read: `-DFK_PLANT_NOMOVABLE` 895 (eq leaves a closure word on no root),
`-DFK_PLANT_NOCALLEE` 2 on the callee band, `-DFK_CLO_YOUNG=0` 767, `-DFK_CLO_YOUNG=1000000000` 753,
`-DFK_PLANT_RECURSIVE_MARK` dies at the first melt of the long chain (rc 138). With `FK_MELT_WITNESS 1` in
`fkwu.conf` each melt prints which root held the pairs it kept (stack, mem, records, nodes, closures, holds) and the
string and closure tables.

Because a melt can move a pair, reuse a string slot or hand a closure row back, an evaluator arm that holds a word in a
C local while it walks something else must have that word on the value stack. What each arm does today
(`fk_walk` and `fk_walk_body`):
- on the stack across the walk: `cons` head and tail, `nth` list, `value_eq` both sides, the string arms (`str_eq`,
  `str_concat`, `str_byte_at`, `str_find`, `substring`), `record_get`, `record_set` (record and key),
  `method_define` (blueprint and name), `method_has`, `scan`, the indirect call's callee (`tag 244`, both arms),
  `apply` (`tag 44`), `eq` and `lt` (through `fk_movable`: a pair, a local string slot or a closure word), the
  two-argument direct call's first argument (`tag 240`, both arms), `intern_composite`'s category (`tag 47`),
  `fb_record`'s file word (`tag 128`), the metal, cuda, socket and wav doors that take a string or a handle first;
- in a C local with no root, and why that holds: `add`, `sub`, `le`, `mul`, `div`, `mod`, the bit doors and
  `round_ndigits` read numbers (a pair, string or closure word stops by name at the next check and is never read
  through its row), `and` and `or` read the word's truth only, `mem_set` and the page, cell and gift doors hold
  ints, `method_invoke` holds a record (records never move) and takes its method from the method table, which is a
  root; the single-walk arms keep nothing across a walk.
The claims that fail when an arm drops its root: `closure-reclaim-band` 128 (`eq`), `closure-callee-band` 1 (the
indirect call's callee); the arms under `tag 47` and `tag 128` have no claim yet, and `tag 240` is a node the parser
never emits (a lowered image carries it), so no source program reaches it.

A kernel started before these doors keeps the old tables until it is restarted, and a binary is replaced by
building beside it and renaming (`cc -O2 -o fkwu.new runtime/fkwu-uni.c && mv fkwu.new fkwu`; copying over a
running binary kills it). The glass fleet is no launchd job: `observe/form-glass-run.fk` is a terminal session whose
supervisor starts the sensors, machine, organs and ear processes once and the live loop under them; ending the
supervisor (`q` in the glass, or its pid) ends all of them, and `./fkwu observe/form-glass-run.fk` from the same
checkout starts them again on whichever `./fkwu` that checkout holds. The ear process stands the microphone while
`.hearth/ear.wanted` stands, so a restart is Urs's to choose.

## The body's own lenses

The body reads itself: which doors bear the most walking, which of a native's mirrors still stand,
where a definition is written twice (one body under several names, one name in several files), and
which written limit carries a witness.

```text
bearing-census-band 32767   form/form-stdlib/bearing-census.bml    (steps, not milliseconds)
mirror-census-band 16383    form/form-stdlib/mirror-census.bml
copy-census-band 63         form/form-stdlib/bml/copy-census.bml
wall-census-band 63         form/form-stdlib/bml/wall-census.bml
observe/tests/voice-frequency-band 255 · number-band 255 · float-printer-band 31
```

## The JIT

A pure-float defn crystallizes into an arm64 f64 leaf when its box ledger crosses a boundary
(`fk_f64_pulse`); a self-tail-call loop crystallizes on heat (`fk_heat_pulse`). `form-lower.fk`
carries runtime strings through the two-slot `fk_inram_args` convention.

```text
jit-lens-band 16383 · jit-heat-gate-band 4095 · observe/tests/jit-evaluator-heat-band 4095
form-lower-string-band 63 · form-lower-string-runtime-band 255 · form-lower-string-both-runtime-band 511
float-natives-band 28 · persistence-band 7 · channel-breath-band 500 · eq-shape-band 524287
blueprint-authority-band 8191
```

## Not standing today

What answered red, died, or was not witnessed today, so no one leans on it:

- The 03:30 night of 2026-10-04 never ran, and why is not known. `launchctl print gui/501/earth.hati.rent-walk`
  read `runs = 2` and `last exit reason = OS_REASON_CODESIGNING`; the OS wrote two reports in the same second,
  `~/Library/Logs/DiagnosticReports/fkwu-2026-10-04-033004.ips` (coalition `earth.hati.coherence.rent-walk`, the second 03:30 job, now
  removed from the repo) and `fkwu-2026-10-04-033004.000.ips` (coalition `earth.hati.rent-walk`): EXC_CRASH, SIGKILL (Code Signature
  Invalid), termination namespace CODESIGNING, indicator Launch Constraint Violation, procLaunch 03:30:04.2316 and 03:30:04.2337 +0800,
  the process alive about 15 ms, `usedImages` `dyld_path_missing` and `main_executable_path_missing` (the OS could not read the
  executable's path), codeSigningID `fkwu.next`. The binaries stand: `codesign --verify` passes (ad hoc, linker-signed), they run from a
  shell, and two throwaway launchd jobs begun at 09:15 (the walk checkout's fkwu and a freshly built one) exited 0; `launchctl print
  gui/501/earth.hati.day-turn` reads `runs = 3` and `last exit code = 0`. So the kill was a transient the body cannot prevent and has not
  explained; the retry job (above) survives one and keeps the evidence of the next in `.hearth/walk-retry.jsonl`, which is how it will be
  learned.

- A Claude Code Bash or Monitor call is refused by nothing but the agent's own compliance. No hook stands
  (Urs's direction: no hook, no non-Form tool). The rule is enforced at the body's own spawn doors
  (`spawn-guard.bml`: every door of the body that begins a child asks it) and observed afterward in the audit
  lens (`forbidden-tools-audit-run.bml`, the land cadence's fourth reading): a slip by an agent's own Bash call
  is counted in its transcript, not stopped.

- The double-click and hot-key launcher of the live ear glass is gone. `Sema Ear Glass.command` was a
  shell file that opened a full-screen terminal on `observe/ear-glass-live.fk`, and a Shortcut could be bound
  to it; the structural gate refuses a script, so it left. What Urs can do now: the documented door line in a
  terminal (`printf '600\n\n' | ./fkwu observe/ear-glass-live.fk`, 600 s of the room in three tongues), or
  `open -n "Sema Ear.app"` for the microphone's principal (that bundle runs the speech-to-text door on a request
  file and writes its answer under `.hearth/`; it is not the glass). `Sema Ear.app`'s executable is itself
  three lines of `sh`, a declared carrier. No native (non-script) launcher bundle exists. The smallest next
  step: emit the launcher as a Mach-O from a Form door the way `form-cli` is emitted (a Unix executable that Finder
  opens in Terminal, that prints the alternate-screen escape and runs the glass door), and make the bundle's
  executable the same kind of file, which retires the last carrier script but `form-run`.

- `form-source-lift` over a mixed module whose first plain form carries a `; preludes:` header writes the
  lifted `import` inside the first section, and its own verification compile stops: `the cursor does not
  read this form.bml section: line 3: import (wanted: topstmt)` (2026-10-01). The five grammar packs it
  lifted carry their imports above the section by hand; `form-source-lift-band` holds no such case yet.
- `form-cli-allowance-band` and `native-tensor-lifecycle-band` reach the Metal
  door and were not re-run in the gates pass (it held no GPU). The integrated review's sweep
  (2026-10-01) read form-cli-allowance 2047 and native-tensor-lifecycle 1023 on fkwu.
- `form-glass-wait-band` read 246 of 255 on the fkwu-only lane (2026-10-01, several sweeps loading the
  host): it landed 12 of 20 rests inside half a millisecond and its watched frame did not wake it. A
  quiet machine is the reading that counts.
- BML `import Num;` binds nothing (`bml-import-ref-resolution-band` 2111; R78). The lowering's other
  open row stands in the ledger: R96 (lowering time grows with one form's argument count).
- Windows: the seed's `_WIN32` branch carries its own spawn and wait twins (`fk_win_spawn`,
  `fk_win_waitpid`), its fifo door answers -1 there, and it passes
  `clang --target=x86_64-w64-windows-gnu -fsyntax-only runtime/fkwu-uni.c` with 0 errors (rc 0,
  today). A link and a run wait on a Windows toolchain or host, and neither is on this Mac.
- `form-glass-frame-work-band` reads 20387 of 32767: its child
  (`form/form-stdlib/tests/form-glass-frame-work-child.fk`) stops rc 1 after its first frame with
  `len: nothing has no length … in fgsr-publication-stamps` (`form/form-stdlib/form-glass-sensor-rows.bml`),
  so a held frame carried a rows cell that read nothing. The child reads other processes' producer
  frames, and a live glass from another checkout was running; `fgsr-take` over all sixteen sensor names
  answered no nothing rows, so the frame that carries it is not yet found.
- The GPU and model lanes not named above were not re-run today; their verdicts are as old as their
  receipts.
