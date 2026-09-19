# The daily walk, on the Mac

The cloud routine walked the rent ladder in a fresh session each morning and
landed nothing twice: a green run there means only that the session exited,
and its log is not readable from the body. The walk moves home. Paste the
prompt below into a Desktop routine with **Local** chosen (Code tab, Routines,
New routine), or run it from a launchd or cron job on the Mac with
`claude -p "$(cat docs/local-walk-prompt.md)"` and the tools Bash and
Artifact allowed. It carries the goal, the one call that walks and lands the
movement, and nothing else. The loopback oracle on 127.0.0.1:18082 speaks
when it stands and reads `none` when it does not; the walk never starts one.

---

You are walking docs/rent-to-zero-goal.form in your checkout of seeker71/coherence-kernel on this machine with the least rented tokens. The north star is the minimum rented token spent on the path to full sovereignty. Do exactly the steps below, nothing else: no exploring, no reading documentation, no extra checks, no provider calls from any door.

1. In the checkout: `git fetch origin claude/form-cli-direct-guided-01ya6p && git checkout claude/form-cli-direct-guided-01ya6p && git pull --ff-only origin claude/form-cli-direct-guided-01ya6p`. If that branch no longer exists on origin, do the same with main.
2. Build the kernel: `cc -O2 -o fkwu runtime/fkwu-uni.c`
3. Find the newest .jsonl file under ~/.claude/projects/ (any subdirectory); call its path T. Then make one call, replacing `<date>` with today (YYYY-MM-DD) and T with that path:
   `printf '%s\n' '{"movement":"local <date>","transcript":"T","subject":"Local movement <date>: rows and the redrawn ladder","body":"Walked by the body in one call (observe/movement-run.bml) on the Mac: the native voice through the loopback oracle if one stands, the native single call, the flow meter on this session, the page redrawn from the ledgers."}' | ./fkwu observe/movement-run.bml`
   This walks the whole movement and lands it: commit and push to the branch you checked out. It prints one JSON line. Its `landing` field reads `landed` only when the push reached origin; `push` carries the push's own exit and last line.
4. Republish the page with the Artifact tool: read https://claude.ai/artifact/DzXVoJfo8T39G6V7GotT1b first, then publish docs/rent-ladder.html to that same url. If the Artifact tool is not available here, skip this step and say so in one line.
5. Reply with only the JSON line from step 3, and the one line from step 4 if it was skipped.

If any step fails, stop, commit nothing further, and reply with the failing command and its last output line only. Never modify any other file, never comment on pull requests, never run a step twice, never start an oracle.
