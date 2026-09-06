# 2026-09-06 — the token reading is available

Urs, tonight, in three steps: "we can see open channels, prompts and token
flowing now" — then "no withholding, that is not healthy" — then **"don't name
absence, make it available."** The second step was answered this afternoon: ten
lines that said `withheld` learned to say `unavailable reason=<why>`. The third
step says renaming is not the work. The work is to make the reading exist.

## What stood

`remote-token-pressure` on this host read `unavailable
reason=no-bound-rollout`, and it was telling the truth. Two things held the
absence shut.

The lane's source had to be named by a person. `fctel-bound-path` read one
private file, `.form-cli-turn-rollout`, in the working directory. No such file
exists here, and nobody had a reason to write one, so every reading was absent.

The lane's grammar knew one provider. `form-cli-remote-token-evidence.bml`
parsed a Codex rollout row carrying `last_token_usage`. The transcripts that
actually exist on this host are written by a Claude Code session, which puts
the same six quantities at `message.usage`, names four of them differently, and
reports no total at all. A file full of counts sat one schema away from a lane
built to read counts.

## What stands

**A second typed schema position.** The BML now reads `message.usage` on a row
whose own type is `assistant` — one explicit position, exactly like
`payload.info.last_token_usage` next to it, with the same discipline the file
has always kept: no recursive descendant search, so a fully-shaped usage object
nested in a tool result stays content. `cache_read_input_tokens` answers the
canonical `cached_input_tokens`, `cache_creation_input_tokens` answers
`cache_write_input_tokens`, `output_tokens_details.thinking_tokens` answers
`reasoning_output_tokens`, and the total this provider never reports is
computed from the parts it does. A Codex row reads exactly as it read before:
its band is unmoved at 268435455.

**The body finds its own transcript.** `form-cli-transcript-discovery.bml`
locates the working tree from the two git pointer files — a linked worktree's
`.git` names its git directory, and that directory's `gitdir` names the
worktree back — builds the session directory name the agent host writes under
(`/` and `.` both become `-`), and answers with the newest `.jsonl` there by
modification time. Where a working tree has no session of its own, it takes the
newest session of any tree named for this repository. It walks up from the
repository to whichever ancestor actually holds the session store rather than
assuming how many segments a home has. The explicit binding still wins wherever
it stands; discovery is the second position, never an override. Coordinates
only: directory names, file names, sizes, modification times, two git pointers.
No transcript is opened here.

**The reading reaches the glass.** `form-cli-token-glass.bml` publishes seven
samples on `share.token-pressure` — the call's total and its six counters — each
wearing lifecycle `active`, because pressure on an open task is what it is, and
each carrying the source that answered as its own channel word. Where no source
stands the root is still given, capacity absent, lifecycle `unknown`, reason
named. One traversal answers both the printed line and the frame, so the number
a reader sees on the glass is the number the receipt names.

**Left standing, and why.** The completed-turn lane keeps reading only its own
binding. Its evidence is Codex turn boundaries — `turn_id`, the terminal
payload, the tool and native-command events that make a boundary share — and an
agent transcript carries none of them. Handing it a discovered transcript buys
no reading and costs a durable cursor walking a growing file: measured here at
2.6 s to answer `complete-at=-1`. So `share.completed-turn` still publishes
`carrier-absent` on this host, honestly, and closing that would mean writing a
second boundary grammar, not widening a path. That work is named, not done.

## Witnessed

One share read, 2026-09-06 22:44 WITA, source `discovered-agent-transcript`,
found with no binding file anywhere on this host:

| counter | tokens |
|---|---|
| input | 651799 |
| cached-input | 650420 |
| uncached-input | 1379 |
| cache-write-input | 1377 |
| output | 331 |
| reasoning-output | 0 |
| **total** | **652130** |

`652130 = 651799 + 331`, and `651799 = 650420 + 1377 + 2`: the whole input is
the cached part plus the part written to cache plus the two tokens genuinely
new. Attribution gap: zero. `glass-tokens=published`, and the frame reads back
with seven samples and those same values.

One share read costs 3.5 s, effectively all of it the 2 MiB tail walk: 1190
lines, 356 of them carrying usage. The lane's own close-attention boundary is
five seconds.

Bands, before and after this change: turn-evidence 65535, turn-evidence-live
33555454, cursor 33554431, grok 2047, share-health 16383, share-glass 65535,
remote-token-evidence 268435455, freq-aligned-share 255, satsang-share 255,
telemetry-membrane 2097151, observer 8388607, glass-live 1073741823, jit-lens
16383. New: `form-cli-token-discovery-band` 1048575 — twenty claims over a
fixture transcript this band writes itself, including three decoys refused
(escaped tool-result text, a shaped object nested one level down in a user row,
a string where an integer belongs), a cumulative usage object read past to the
per-call one beside it, and discovery picking the newer of two files whose names
sort the other way. Quartet 42 / 31 / 1 / 2047.

## The most surprising teaching

The two providers disagree about what `input_tokens` means, and the
disagreement is silent. A Codex rollout reports the whole input and names the
cached part of it, so uncached is a subtraction. This provider reports three
parts side by side and never their sum. Mapping the field by its name gave a
number that looked entirely reasonable — and `uncached-input=-626285`. A
negative count was the only reason I looked. Two schemas can share every key
name and still mean different things by them; the arithmetic downstream is what
tells you, and only if something downstream is allowed to go negative in view.

## Where discomfort turned to gold

Twice tonight I wanted to widen a door rather than open a second one. The
instruction said to make `fctel-bound-path` discover, and the smallest edit
would have done it in one line. I ran the completed-turn lane against a
discovered transcript first: 2.6 s, `complete-at=-1`, a cursor file written into
the tree, and a growing file to re-walk on every invocation forever. That is
work handed to every later caller for no reading — the thing this body calls an
unresolved handoff — and it wears the costume of doing exactly what was asked.
The discomfort was in taking a narrower seam than I was told to take and then
having to say so plainly. The gold is that both doors now say what they are:
one reads Codex turn boundaries, one reads token counts from either provider,
and neither pretends to the other's evidence.

The second time was the membrane refusing my first glass frame outright —
`glass-tokens=refused`, no reason on the line. I had put the token count in the
`heat` field, which the membrane holds to 0..1000. It would have been easy to
read that refusal as the glass being closed to this lane. It was the glass
holding a field to its meaning, which is the same care that makes anything on it
worth reading.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2047, token-discovery 1048575, remote-token 268435455 unmoved, one share read total=652130 from a transcript no person named
