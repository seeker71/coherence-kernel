# The native git (2026-10-04)

Urs's rule is absolute: only Form native code in the body; external models are fine; a download needs his yes. `git` was the one declared
exception to it. This receipt is the first of the organ that retires it: git's repository formats read, written and walked in Form, with
no program started, proved by self-verification and by repositories the bands write themselves (no oracle: the bands never run `git`).

Door: `observe/git-native-run.bml` (stdin one git command line, stdout what git prints, stderr git's stderr, the last line the exit code).

## 1. The census (what the product asks of git)

Measured with `printf '{"word":"git"}' | ./fkwu observe/membrane-sites-run.bml` (2026-10-04): **107 call sites in 30 files**; where a site's
argv sits in a variable (belief-stamps, lane-motion, lora-voice, hearth-glass-live) the def was read; the two drift-gates shell lines and the
band-cover row each name two commands, so the command incidences are **109** (the brief's "~109"). Class: R reads objects, refs, index or
working tree; W writes them locally; T crosses the network; G is a pass-through whose argv the caller decides (`hch-git-in` / `hch-git-quiet-in`).

| subcommand | sites | class | what the sites ask |
|---|---:|---|---|
| ls-files | 21 | R | `--cached --others --exclude-standard` (+`-z`, `--` pathspecs `*/tests/*-band.fk`), `--others`, `-u -z -- path`, plain |
| rev-parse | 16 | R | `HEAD`; `--abbrev-ref HEAD`; `--verify [--quiet|-q] <rev>^{commit}` / `REBASE_HEAD`; `-C dir` |
| log | 11 | R | `-1 --format=%H -S <needle> <head> -- <path>` (pickaxe), `--oneline -1`, `-1 --format=who=%an-%cd --date=format:%H:%M origin/main`, `--no-merges --format=%h%x1f%ad%x1f%an%x1f%s%x1f%b%x1e --date=short`, `-1 --format=%ad --date=short -- <path>` |
| diff | 11 | R | `-U0 --no-color <seen> <head> -- <path>` (hunks), `--quiet HEAD -- <path>`, `--cached --quiet -- <paths>`, `--name-only [-z] [--diff-filter=U]`, `--name-only <base>` / `HEAD` / `<rev>`, `--numstat origin/main -- <path>` |
| merge-base | 7 | R | `--is-ancestor a b`, `<c> <head>`, `HEAD origin/main` |
| cat-file | 7 | R | `-e <rev>^{commit}`, `-e <rev>:<path>`, `blob <rev>:<path>`, `-t origin/main:<path>` |
| rev-list | 2 | R | `--count origin/main..HEAD` |
| status | 1 | R | `--porcelain=v1` |
| worktree | 1 | R | `list --porcelain -z` |
| check-attr | 1 | R | `merge -- <path>` |
| grep | 2 | R | `-a -n -E <pattern> -- *.fk *.bml` (the body's internal grep reads the tree: refused here by name) |
| add | 3 | W | `--pathspec-from-file=<f> --pathspec-file-nul`, `-- <path>`, `-- <paths>` |
| commit | 3 | W | `-c user.name=Claude -c user.email=noreply@anthropic.com commit -q -F <file>` / `-q -m <subject> -- <paths>` / `-q --amend --no-edit` |
| checkout | 3 | W | `-B <branch> <base>`, `--ours -- <path>`, `HEAD -- <paths>` |
| init | 1 | W | `-q` (the heal-native scratch repository) |
| rebase | 7 | W | `origin/main`, `-q --autostash origin/main`, `--continue` (`-c core.editor=true`), `--abort` ×3 |
| fetch | 3 | T | `origin main [branch]` |
| push | 4 | T | `-u origin HEAD`, `--force-with-lease [-u] origin HEAD[:branch]`, `-q origin HEAD:main` |
| (pass-through) | 5 | G | `hch-git-in(dir, args)` ×3 sites, the landing's generic call, prelude-reach's `cons("git", args)` |

R = 78 incidences (+ 2 grep), W = 17, T = 7, G = 5; total 109 incidences over 107 sites. The table orders the work: phase 1 (R) is
three quarters of the product's git, so it was built first and fully; W (17) next; T (7) last because it needs a door the seed lacks.

## 2. Phase 1 — the read organ (built, proved)

Files (all new, all Form): `form/form-stdlib/bml/`
`git-sha1.bml` (SHA-1), `git-inflate.bml` (zlib/DEFLATE: stored, fixed and dynamic Huffman, Adler-32),
`git-store.bml` (discovery incl. a worktree's `.git` FILE, `commondir`, alternates; loose objects; pack indexes v2 with fanout, 32- and 64-bit
offsets; OFS_DELTA and REF_DELTA with the delta stream; the commit-graph file; abbreviations; tree/commit/tag parsers),
`git-refs.bml` (HEAD, loose and packed refs with peeled lines, symbolic refs, listing, config files),
`git-index.bml` (index v2/v3/v4 incl. path prefix compression, extensions skipped, the cache-tree parsed, checksum, a sorted-names string with
binary search), `git-ignore.bml` (gitignore: anchoring, `**`, classes, negation, directory-only, escapes, nested files, info/exclude, global),
`git-status.bml`, `git-rev.bml` (rev names, date-ordered walks, ranges, merge-base, dates, `--format`), `git-diff.bml` (tree/index/worktree
diffs, exact renames, Myers line diff, unified hunks, numstat/stat), `git-cmd.bml` (the commands).

Commands and flags supported (exit codes as git's): `rev-parse` (`--show-toplevel --git-dir --git-common-dir --absolute-git-dir
--is-inside-work-tree --is-bare-repository --show-prefix --show-cdup --verify -q --short[=N] --abbrev-ref --symbolic-full-name`, `^ ~ ^{type}
:path @{u}`), `status` (`--porcelain[=v1] -s -b --ignored -u<mode> -z`, paths), `diff` (`<a> <b>`, `a..b`, `a...b`, `--cached`, `--name-only`,
`--name-status`, `--numstat`, `--stat`, `-U<n>`, `--quiet`, `--diff-filter`, `-z`, `--no-renames`, paths), `log` (`--oneline --format=
--pretty=format: -n -N --max-count --skip --first-parent --no-merges --date= -S<s>`, ranges, `-- paths`, medium layout), `show`, `ls-files`
(`-c -o -m -d -u -s -z -i --exclude-standard`, pathspecs incl. globs and `:!`), `ls-tree`, `cat-file` (`-t -s -p -e`, typed), `merge-base`
(`--all --is-ancestor`), `rev-list` (`--count --left-right --first-parent --no-merges --parents --reverse -n --skip`), `branch` (list forms),
`config` (`--get --get-all --list`), `remote` (`get-url`, list, `-v`), `worktree list [--porcelain] [-z]`, `check-attr`, `hash-object`,
`symbolic-ref`.

Refused by name with exit 129 and the flag in the message, never answered wrongly: every flag outside those sets (`log --graph`, `diff
--word-diff`, `ls-files --eol`, `status --porcelain=v2` ...), the long `status` format, `git grep`; and the commands of phases 2/3 when the
read door is used alone.

## 3. Proofs observed (the bands; each prints `Verdict N` and the rest as printed lines)

Self-verification, not an oracle: an object read hashes (sha1 of `"<type> <size>\0" ++ body`) to the name it was found under; commits' parents
and trees exist; a pack's trailing checksum equals the sha1 of the pack; the index's trailing checksum equals the sha1 of the index.

| band | verdict | what it showed |
|---|---:|---|
| `git-native-sha1-band` | 255 / 255 | NIST vectors (`abc`, empty, the 56- and 112-byte ones, one million `a`), git's ids for the empty blob / `hello\n` / `test content\n`, 48 of this repository's loose objects hash to their names |
| `git-native-inflate-band` | 255 / 255 | hand-made stored, fixed-Huffman (every length class 3..258, distances to 12000, overlapping matches) and multi-block streams; zlib's own `hello world` stream; 24 real loose objects (dynamic Huffman) pass Adler-32 and hash to their names; 22 packed objects; eight faulty-stream kinds answer nothing(); Adler constants |
| `git-native-store-band` | 255 / 255 | this worktree opened through its `.git` FILE and `commondir`; the first **200 first-parent ancestors**: every commit and every root tree read hashes to its id (13.7–21 s); 13 blobs + a subtree walk; **712 packed + 68 loose refs** all resolve to existing objects; the 3 packs (**73,416 / 612 / 5,023 objects**, the largest **1,456,125,550 bytes**): idx v2 header, monotone fanout ending at n, sorted names, `PACK` v2 header count = n, recorded pack checksum = the pack's last 20 bytes, idx trailing checksum = sha1 of the idx, pack checksum = sha1 for packs under 4 MB, 24 objects sampled across the largest pack (OFS_DELTA chains included) hash to their names; a synthetic repository (loose blob/tree/commit, a hand-built pack with an OFS_DELTA, unique/ambiguous abbreviations, alternates, a 64-bit idx offset, symbolic/loose/packed/peeled/detached refs); the commit-graph's rows agree with the commit objects for 60 spread commits |
| `git-native-index-band` | 255 / 255 | this checkout's index (version 3 today, 2,079 entries, 91 cache-tree nodes, checksum ok); every blob it names exists; cache-tree counts equal the entries under each directory; the sorted-names search; synthetic v2 (names of 1..70 bytes, a 5,000-byte name, three conflict stages), hand-assembled v4 (prefix compression) and v3 (extended flags), unknown/TREE/REUC extensions |
| `git-native-status-band` | 255 / 255 | a synthetic worktree altered every way: the **16 expected porcelain lines exactly** (AM, M, UU, D, R, A, ??, quoted path, collapsed dirs), `--ignored` lines, `-b`, `-z`, `-uno/-uall`, the stat shortcut and the racy rule, a 40-row ignore table, nested ignore files, the seven unmerged codes, unborn HEAD |
| `git-native-rev-band` | 255 / 255 | a known history (A..G with a merge): names, `^ ~ ^{} :path @{u}`, abbreviations, date-ordered walks, first-parent, no-merges, skip/limit, ranges, symmetric ranges, merge-base, is-ancestor, date arithmetic (1e9 is a Sunday, leap days, zones), `--format`, decorations; and this repository's real history |
| `git-native-cmd-band` | 255 / 255 | 23 groups of exact stdout/exit/stderr over the demo history: every command and flag of section 2, refusals and exit codes |
| `git-native-pack-band` | 15 / 15 | the whole 2 MB pack (612 objects, **106 OFS_DELTA**) read and hashed; the 30 MB pack's sha1; 300 of its 5,023 objects; the pack and idx checksums (see the timings) |
| `git-native-speed-band` | see §6 | wall times on this repository, with ceilings |

## 4. Phase 2 — local writes (built, proved) and merge/rebase

`git-write.bml` writes: zlib streams (stored blocks), CRC-32, loose objects (temporary name then rename, mode 0444), trees, commits, pack files
with OFS_DELTA entries and their v2 indexes, the index file, refs with reflog lines, `git init`. `git-wcmd.bml` carries `init, hash-object -w,
add, rm, commit, update-ref, branch, tag, checkout, switch, restore, reset`; `git-merge.bml` carries a diff3 line merge (over the Myers
scripts, git's touching-hunks-conflict rule, `union` attribute), a three-way tree merge (modify/delete, add/add), `merge` and `rebase` (state in
`.git/rebase-merge` as git writes it; `--continue --skip --abort --autostash`).

The gate: a write is allowed only in a repository under `host_temp_dir()` or with `-c native.allow-write=true`; otherwise exit 128 and nothing
touched. **No band wrote into this worktree's `.git`**: every write band runs in a scratch repository it makes and removes.

| band | verdict | what it showed |
|---|---:|---|
| `git-native-write-band` | 255 / 255 | 45 groups: init (-q, -b, Reinitialized), add (files, dirs, -A, -u), the root-commit line, `-F` message normalisation, `-m` ×2, amend, `--allow-empty`, nothing-to-commit, `--author`, a path-limited commit that leaves other changes, branch/tag/update-ref (old-value check, refusals), checkout/switch (files rewritten exactly, local change blocks the switch, -b/-B, detached, `<rev> -- path`), restore/reset (soft, mixed, hard, paths)/rm, the gate, hash-object -w, a hand-built pack with an OFS_DELTA read through its index, and an **fsck of the scratch repository after the run: every loose object hashes to its name, every commit's tree and parents exist, every ref resolves** (35 loose objects) |
| `git-native-merge-band` | see §6 | text merges against merges worked out by hand; rebase over a diverged history (clean, conflict stop with stages and markers, `--abort`, resolve + `--continue` keeping the author, `--skip`, modify/delete `DU`, union attribute, dropping a change already upstream), autostash, merge (ff, `--ff-only` 128, `--no-ff`, three-way commit with two parents, conflicted merge finished by `commit`, `--abort`), fsck of all eight repositories, refusals |

## 5. What each phase retires of the 109 incidences

* Phase 1 (R) retires **78 incidences** the moment their callers call `git-run-line` / `git-open` + the functions instead of spawning: all
  `rev-parse`, `ls-files`, `log`, `diff`, `merge-base`, `cat-file`, `rev-list`, `status`, `worktree`, `check-attr` sites (`band-sweep*`,
  `findings-requests`, `form-cli-build-flow`, `host-walk-*`, `native-*`, `drift-gates`, `band-cover-run`, `band-truth-run`, `belief-stamps`,
  `carrier-mass`, `day-turn`, `door-link-health`, `lane-motion`, `lora-voice`, `hearth-glass-live`, `prelude-reach`, `scheduled-walk`,
  `walk-sync-run`, `band-sweep-run`). The 2 `git grep` sites move to the internal `grep` over `ls-files`.
* Phase 2 (W) retires **17**: `init` (heal-native-eval), `add` (3), `commit` (3), `checkout` (3) and `rebase` (7: `origin/main`, `--autostash`,
  `--continue`, `--abort`) — the landing, reunion, walk-settle and walk-sync flows — once those callers are pointed at `git-run-full`.
* Phase 3 (T) retires the last **7** (`fetch` ×3, `push` ×4), and with them the G pass-throughs' remaining targets. Design below.

## 6. Timings, throughput

Observed on this repository (1.4 GB pack, 2,000-file index, machine heavily loaded by sibling helpers), one cold call each in a fresh process,
from `git-native-speed-band` (verdict 63) and the door:

| call | observed |
|---|---|
| git-open | 43-79 ms |
| rev-parse HEAD | 17-60 ms in-process; 7.8 s through the door cold |
| HEAD commit read | 6-16 ms |
| log --oneline -n 20 | 0.4 s in-process; 0.96 s through the door |
| status --porcelain | 8.0 s in-process; 9.5 s through the door (was 37 s before the size shortcut and merge-join scan) |
| diff --name-only HEAD | 3.8-4.0 s |
| rev-list --count origin/main..HEAD, merge-base HEAD origin/main | 1.0-1.3 s together |
| 20 cached commit reads | 0-91 ms |

Band verdicts at the last run: sha1, inflate, store, index, status, rev, cmd, write, merge 255; pack 15; speed 63.

## 7. Phase 3 — transport (design; not built)

What stands: the body has `tls_request(host, port, request-bytes)` (TLS 1.2+ with certificate verification, one request, a **64 KB response
cap**), `http_get`, and `socket_*` (plain TCP, streaming). What is missing is a streaming TLS door (`tls_open / tls_read / tls_write /
tls_close` over the same libssl the seed already loads): a pack download is megabytes and `tls_request` returns at most 65,535 bytes. That
door belongs in `runtime/fkwu-uni.c` (another helper's file this session) and is the one seed addition phase 3 needs.

Protocol (smart HTTP, protocol v2; SSH is not planned — it needs a Form SSH client with curve25519/ed25519/chacha20-poly1305):

1. `GET <url>/info/refs?service=git-upload-pack` with `Git-Protocol: version=2` → pkt-lines (4 hex length + payload; `0000` flush, `0001` delim,
   `0002` response-end) → the capability list (`agent`, `ls-refs`, `fetch=shallow wait-for-done`, `object-format=sha1`, `server-option`).
2. `POST <url>/git-upload-pack` command `ls-refs` (`peel`, `symrefs`, `ref-prefix refs/heads/ refs/tags/`) → `<oid> <ref> [symref-target:..] [peeled:..]`.
3. `POST` command `fetch`: `want <oid>` per wanted tip, `have <oid>` from our refs walked newest first (the date-ordered walk of git-rev.bml)
   in rounds until `ready`, then `done`; capabilities `thin-pack ofs-delta include-tag no-progress`. The response: `acknowledgments`, then
   `packfile` carrying sideband-64k frames (band 1 pack data, 2 progress, 3 error).
4. The pack is written to `objects/pack/tmp-*.pack`, then **indexed in Form**: one sequential pass inflating each entry (git-inflate.bml) to find
   its end, resolving OFS_DELTA against the pack and REF_DELTA (thin-pack) against the local store, hashing every object (git-sha1.bml), and
   writing the idx v2 (gw-idx) — the same machinery the phase-1 reader and the phase-2 writer already prove. Then refs under `refs/remotes/<name>/` move with a reflog line.
5. push: `GET info/refs?service=git-receive-pack`; the commands `<old> <new> <ref>\0 report-status side-band-64k` plus a pack of the objects the
   remote lacks (the commits `new..` not reachable from its advertised tips; blobs/trees by the walk in git-rev.bml/git-diff.bml; stored-block deflate
   from gw-pack, which git accepts) → `unpack ok` / `ok <ref>` / `ng <ref> <reason>`; `--force-with-lease` is the old-value in the command.

Credentials: **the organ reads none.** It never opens `~/.git-credentials`, the keychain, `~/.netrc`, an SSH agent or any token file. Auth is
supplied by the caller as an explicit argument — an `Authorization` header value (or a username/token pair) passed in the command line the
door receives (`-c native.http.extraHeader=...` or a door field), taken by the caller from wherever Urs decides. The URL comes from
`remote.<name>.url`; a remote that answers 401 is reported as git does (exit 128, "Authentication failed") and nothing is retried with
anything it was not given.

Honest measure of the work: with the door in place, ls-refs + fetch + index-pack is the two weeks of git that matters; push adds pack
building (already present) and the report-status parse. Neither was started this session: nothing may be downloaded without Urs's yes, and
the streaming door is a seed change.
