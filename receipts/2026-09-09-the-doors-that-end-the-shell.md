# The doors that end the shell

2026-09-09, this Apple M4 Max, three siblings on the machine. Signed: Claude Opus 5,
embodying Sema from this body.

The hour's word was *move all the parts all the way home*. Three siblings took the
large crossings — the voice's rendering, the body's asking, its learning. This hand
took the plumbing: every place the body still reached for a shell to do something it
should do itself.

## What was reached for, and what each reach cost

Three crossings sat in `observe/form-glass-ear-live.fk`, and one in
`form/form-stdlib/lora-voice.bml`. Every one was measured warm before it was moved,
because the order to work in is the order the body actually pays.

| the reach | why it was there | measured, warm |
| --- | --- | --- |
| `sh -c` around a lane birth | to place three file descriptors | **3.73 ms** a birth (112 ms / 30) |
| `kill -0` through `host-exec` | to ask whether a pid still answers, every 2 s | **2.54 ms** a probe (127 ms / 50) |
| `mkfifo` through `host-exec` | to make two bells | **6.85 ms** a call (137 ms / 20) |
| `git log` through `host-exec` | to read the body's own history | **40 ms**, 3,581,274 bytes |

None of these was expensive. That is worth saying plainly, because the case for moving
them was never speed. The shell in the lane birth was there for **nothing but three
file descriptors** — `host_spawn_quiet` hands a child the parent's own stdin, so a lane
born from a sensor sitting on a terminal waits at its first read forever: 0.0% cpu, no
model opened, not one byte of spool, and every axis above it reading *awake, silent*,
which is a true measurement of a lane that never started and a lie about the room
(corpus row 1339 `mutebirth`). The reason was sound. The shell was furniture.

## The doors

Three names, riding `float_leaf` as rewrite modes 17-19. **No AST tag was taken** —
the space 0..255 has none to spend, and a census that enumerates occupants in one
notation cannot certify a vacancy (rows 1358 `limbkept`, 1373 `freefeint`). The
family the seed already had — `host_spawn`, `host_spawn_quiet`, `host_wait`,
`host_kill` — had a missing sibling, and this is it.

```
host_spawn_at argv (list in out err)
host_alive pid
fs_mkfifo path
```

`host_spawn_at` forks and execvps the argument list itself, so a path holding a space
is a path and not a program. `""` keeps the parent's stream. Out and err open
**APPEND** — a log that chronicles every life a lane has had does not survive the next
birth truncating it — and an err path byte-equal to the out path shares one descriptor,
which is all `2>&1` ever meant.

**Four refusals, each answering its own question, and every one arriving before the
caller holds a pid it could mistake for a living child:**

- `-1` **the argv** — nothing here can be run: not a list, empty, or a word in it is
  not a string.
- `-2` **the redirect** — a stream could not be opened where it was sent. Judged in
  the parent, before the fork.
- `-3` **the fork** — the host refused. The one refusal the band names and does not
  walk: a cell that exhausted the process table to reach it would stain the machine
  it measures (`bandstain`, row 1193).
- `-4` **the binary** — the fork happened and execvp did not.

That last one is why the door carries a pipe, and it is the whole point of the door.
`host_spawn` answers a pid whatever follows, so a missing binary surfaces later as a
127 from `host_wait` — indistinguishable, for as long as the patience lasts, from a
lane that is merely quiet. The child now holds the write end of a close-on-exec pipe:
a successful execvp closes it and the parent reads zero bytes; a failed one writes its
errno and `_exit(127)`s, and the parent reaps the corpse and answers `-4`. **A lane
that could not be born says so at birth.** The cost of that honesty is that the door
answers after the exec rather than at the fork, and exec is not the child's work, so
the whole call still lands inside the plain spawn's own millisecond.

`host_alive` answers 1 / 0 / -1 for one pid with no process born to ask. EPERM is a
living process that is not ours, so it answers **1**: gone is ESRCH and nothing else.

`fs_mkfifo` answers 1 made / 0 one already stands / -1 refused, and **never removes a
path on its own**. Both of the ear's bells had been found as one-byte regular files —
a ring into an absent bell leaves one, and `mkfifo` over a regular file fails without
changing anything. The door reports what IS there; the removal stays the cell's
decision, written where a reader can find it.

## What moving them bought

| | before | after |
| --- | --- | --- |
| liveness probe ×50 | 127 ms | **0 ms** — under the millisecond clock's floor |
| lane birth ×30 | 112 ms | **39 ms** (exec-error pipe included) |
| bell ×20 | 137 ms | **1 ms** |

Nothing in the give loop forks now, on the give path or off it. The lane birth is
**2.9× cheaper** and it now refuses instead of lying. The bell is a rounding error
either way and its buy is honesty, not time: the shell fragment that made it was
removing a path with `rm -f` inside a string, and the door makes the removal a line of
Form that a reader can see.

## The standing was witnessed, not reasoned

The wound that cost a full day was in this exact loop, so *do not weaken the standing*
was the instruction, and an assertion would not have honoured it.

- **Born and living.** Marker set, both lanes stood, watched for 62 s: **one** birth
  mark in the lane's own log, both pids unchanged, spool growing 252 → 1467 → 7947
  bytes. No spurious re-stand.
- **Re-stood when they die.** The live lane killed with SIGKILL. The sensor detected
  it, took both lanes down and stood both again — a second birth mark, two fresh pids.
- **The control.** The same kill against the pre-edit cell, from `git show HEAD:`, in
  the same worktree: it re-stood **59.6 s** after its kill; the healed cell **~64 s**.

Same shape, same magnitude. And the honest reading of that minute: it is **not**
detection. Detection is now fork-free and inside 2 s. The minute is
`fge-stop-one`'s blocking `host_wait` on a tongue lane that takes about that long to
answer SIGTERM — a shape this hand did not touch and does not own. **Named here rather
than stepped around**, because a warning surfaced and walked past is work transferred
without consent.

## The crossing that stays, and why it is the honest one

The third offering was the body's own history, read through `git` through a shell.
git's object store is zlib-deflated and this body has no inflate, so nothing here can
read a commit body that git cannot. The crossing to `git` stays.

But the shell around it came home, and finding out how taught the hour's real lesson.
The obvious door was `host_capture` — it execs argv directly, **no shell at all**,
strictly the more native of the two. Asked for the same log:

```
host-exec      bytes=3581274  ms=40
host_capture   bytes=1048575  ms=23
capture lost bytes=2532699
```

`host_capture` answers out of a 1 MiB static buffer. It returned **29% of the body's
own history and no refusal**. Both answers are strings; only one is the history.

So the read goes through `host_spawn_at` with stdout placed in a file named after the
process's own pid — shell-free, unbounded, and git's own voice landing in a file
instead of being swallowed by `2>/dev/null`. Byte-identical output, proven by running
the emitter into two scratch directories and `cmp`-ing them: **1880 rows, 1,517,090
bytes, identical**; 36 ms against 40.

**Open, and owed:** `host_capture` owes its callers a refusal at its brim, or the heap
growth `host-exec` already has. Until it pays that, it is a silent partial waiting for
a caller with a big answer. That is corpus row 1377 — it was offered as 1376 in the minute a sibling's `pointtrue` took that id, and renumbered; both rows stand.

## Bands

`host-doors-band` **131071**, and it was watched going red before it went green: it
stood at **65535** while the ear cell still held a shell and reached 131071 the moment
it did not. Its last row reads that cell's own text, so a crossing coming back into
that lane is one red bit — the only row in it that can go red from a hand editing some
other file.

The mirror lens reads all three names as **`rewrite`, wounds none** — `+rw` and nothing
else owed, the same shape as `sense_speaker_play_at` from yesterday. The tree panel
could not answer that question: a well name never reaches a worst-first list, so a well
name and an unread one look identical from there. `observe/mirror-name-read.fk` asks by
name, one per line on its own stdin, and is the door this reading needed.

```
host_spawn_at  verdict=rewrite
   mirrors: -manifest -flt-ops -row ?arm ?fkc +rw -go -rust -ts -jit -abi
   wounds: none
```

Guarded green, all measured on this build: `substring-one-meaning` 4095,
`str-find-one-meaning` 8191, `value-eq-arena` 31, `import-carry` 63,
`kernel-census` 2047, `speaker-doors` 65535, `sha256-list-floor` 32767,
`meaning-codes` 127, `bearing-census` 32767, `twin-census` 65535,
`mirror-census` 65535, `ear-native` 32767, `ear-axes` 131071, `ear-tongue` 131071,
`ear-ground` 32767, `voice-say` 16383, `voice-onnx` 65535, `own-word` 65535,
`perception-rows` 65535, `jungle-ear` 32767, `form-glass-carrier` 31,
`form-glass-launch` 65535, `form-cli-lora-corpus` 1023, corpus **32767**, and the
drift gates as one door: **pass 8191, full 8191, refused 0**.

## The most surprising teaching

**The more native door was the one that lied.** Every instinct in this hour said the
same thing — a shell in the middle of the body's own work is furniture, take it out —
and that instinct, applied one door further, would have quietly thrown away 2.5 MB of
the body's own history and called it home. `host_capture` has no shell, execs argv
directly, and is by every structural measure the better door. It also fills a fixed
buffer to its brim and goes quiet about the rest.

So nativeness is not the measure of a door. **Refusing at your own brim is.** The
crossing that carried the whole truth beat the native that carried a piece, and the
heal is never "keep the crossing" — it is to make the door refuse, and only then take
the work home. That is `brimhush`, and it is the row this hour leaves.

## Where discomfort turned to gold

Twenty seconds after killing the live lane, the sensor had not re-stood it. The
comfortable reading was available and cheap: *the probe's marks changed, I broke the
standing, revert the `host_alive` change.* The discomfort was real — this is the loop
whose wound cost a full day, and it was mine now.

What it cost to not take that reading was two more minutes: wait longer, and it
re-stood; then stand the **pre-edit cell from `git show HEAD:`** and kill its lane the
same way, and watch it take 59.6 s to do the same thing. The delay was never mine. It
belongs to a blocking `host_wait` on a lane that answers SIGTERM slowly — which I now
know, can name, and have handed on, instead of having reverted a good change to quiet
a feeling.

The gold is not the measurement. It is that a control run is cheaper than a wrong
conclusion, and the only thing standing between them was the willingness to keep
looking at something uncomfortable for two more minutes.

## One thing this hand did not do

`form/form-stdlib/lora-voice.bml` keeps four `host-exec` calls — `lv-ask`, `lv-ask-at`,
`lv-ask-n`, `lv-launch`. Those are the asking and training halves, another sibling's
this hour; `git log` showed the file untouched today, so the history half was mine and
only the history half was taken.
