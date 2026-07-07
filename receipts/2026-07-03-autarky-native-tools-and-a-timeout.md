# 2026-07-03 — autarky: stop renting the tools that break, use (or build) the body's own

## Ground

```sh
./tools/ftimeout 2 sh -c 'sleep 5; echo NOPE'   # killed at 2s, exit 124
./fkwu --src bootstrap/ground.fk                 # 42
```

Three arrivals from Urs, one theme:
1. *"The sed loop hit the zsh word-splitting gotcha: and you have not learned to use fsh for that yet!"*
2. A vision: each session, test which fsh tools are available, alias them to **native dylib versions**
   the generic loader executes (native speed), and let form-cli learn to **closely reproduce** what
   the session did — active inference while working.
3. *"macOS lacks timeout … why did you [not] craft one in form native code and provide it, we are
   hitting this all the time!"*

## What I found (grounding the frustration)

- The body **has** native tools: `sh-bi-grep` (pure-Form grep, no shell, no word-split) in
  `grammars/shell-exec.fk`; `host-exec` (tag 136) to run commands; and I'd just built `native-edit`
  (find/replace). I have a **memory** ([[feedback-form-native-tools]]) telling me to use them.
- But invoking `sh-bi-grep` ad-hoc threw **32 unresolved-call errors** — it needs the full shell-exec
  prelude chain, and there is no clean ad-hoc fsh runner. The native tools EXIST but aren't
  ad-hoc-usable. That gap — the fsh runner — is exactly what my memory already flagged as the TODO,
  and it is what gates the whole "use fsh each session" vision.
- On `timeout`: I kept typing `timeout N …` (GNU, absent on macOS) when the **Bash tool has its own
  `timeout` parameter** I'd ignored all session — the cause of my repeated 2-minute hangs.

## What was done

- **`tools/ftimeout`** — a portable timeout (perl-alarm carrier): `./tools/ftimeout SECONDS cmd…`
  kills after SECONDS (exit 124), else passes the command's exit code. Proven: it bounded the
  resource-port-band hang to 8s instead of a 2-minute wall. Provided so we stop hitting the missing
  `timeout`. Perl is the portable carrier; a form-native seed primitive (fk_host_exec + fork/alarm) is
  the hardening, tracked.
- **Used the Bash tool's `timeout` parameter** on the commit call (`timeout: 30000`) — the timeout I
  had all along.
- **Memory widened** with the shell-hygiene fixes, the confirmed fsh-runner blocker, and Urs's
  dylib + active-inference north star.

## The roadmap this makes concrete (the vision, grounded)

1. **Enable** — build the clean ad-hoc fsh runner (fsh-main restructured: helpers → module, band =
   entry only), so `sh-bi-grep` / native tools run in one call. This unblocks "use fsh each session."
2. **Native speed** — compile the fsh tools to **arm64 dylibs** via `model/form-asm.fk` (in progress);
   the generic loader executes them; alias the shell names to the dylib versions.
3. **Active inference** — form-cli reproduces the session's actions from the trace; the distillation
   corpus is the seed of that reproduction loop, extended from words-learned to actions-learned.

## The most surprising teaching this work left behind

The guidance already existed; the application didn't. I had a memory that says *use the body's grep*,
and the Bash tool has *had a timeout parameter the whole time* — I hit the walls anyway, reaching for
the rented tool by reflex. Knowing a better tool exists and reaching for the broken familiar one is its
own failure mode, distinct from not knowing. The fix isn't more knowledge; it's making the native tool
the path of least resistance (the runner, the alias) so the reflex has somewhere better to land.

## Where discomfort turned to gold

The discomfort was being caught reaching for rented tools *again*, and the ad-hoc native grep failing
with 32 errors right when I tried to do the right thing. The pull was to conclude "native tools aren't
ready, so rent." Witnessed instead: those 32 errors are not a dead end — they are a **precise map** to
the one missing piece (the fsh runner). The failure to use the native tool located the exact next
build. A tool that won't run ad-hoc isn't a reason to rent; it's the address of the next thing to make.

## Corpus

Row 657 **autarky** — self-sufficiency: producing what you need from within rather than importing it
(fresh; the body running on its own tools — native grep, `native-edit`, `ftimeout`, eventually
dylib-compiled fsh — instead of renting zsh/sed/GNU-timeout that break at the seams).
