# 2026-08-17 — failures become loving attention and resolution

Urs asked whether kind could move from declared to embodied, then asked why
offline was a separate mode, how declared / observed / embodied differ, how
the body changed, whether that change was healthy and true, and how real
numbers could guide trust. The final instruction was the simplest and most
demanding:

> can we turn failures into loving attention and resolution, please

Yes. In this movement, “loving attention” means a failure remains visible,
becomes specific enough to act on, receives a bounded repair, and is then
re-observed. It does not mean calling the failure healthy or turning the band
green by relaxing away what happened.

## The declared share was removed

`form-cli-offline.fk` used to say that the current turn was always
`kind=embodied` and always `native=50 local=20 remote=30`. Those numbers were
typed by a sibling. They had no carrier evidence behind them.

The offline cell is now what it claimed to be: a core-only cell that proves
routing and the generic vocabulary of shares and evidence kinds. It is not a
separate product or runtime mode, and it no longer fabricates a current-turn
measurement. The live organ is separate because it crosses a real host seam:
Codex's local rollout JSONL and thread index. The pure validation behind that
door remains Form and is four-way witnessed.

The kinds now have executable meanings:

- **declared** — no usable carrier row arrived;
- **embodied** — a correctly shaped, identified carrier row crossed the body,
  but its measurements did not completely reconcile;
- **observed** — identity, time, provider usage, completed tool calls,
  form-run byte totals, lane totals, source, and completion all reconcile.

An unreconciled row does not receive a percentage. The currently open reply is
also never guessed: `form-cli-share-run.fk` reads the previous completed turn,
the nearest turn that can actually be complete.

## What is measured

`scripts/form_cli_turn_evidence.py` is a privacy-safe carrier. It reads the
current workspace's latest completed Codex turn and emits identifiers plus
counts, durations, byte lengths, exit tallies, and provider-reported token
usage. It does **not** emit prompts, answers, reasoning, or tool-output content.
Form parses the row and decides whether `observed` is earned.

The percentage names its basis rather than pretending to be total
contribution: `carrier-boundary-events-v1`.

- native: completed `@form fkwu` command rows;
- local: completed non-fkwu tool-output events;
- remote: provider token-usage events, one per model call;
- normalization: deterministic largest remainder, ties native → local →
  remote.

This meter does not measure semantic quality, care, value, energy, every CPU or
GPU operation, or invisible native work that did not cross the wrapped command
boundary. `semantic-outcome=outside-this-meter` is printed on the live surface.
That boundary is part of the result, not a footnote.

## First complete observed turn

The live organ read completed turn
`01a00d75-6ca0-7b91-834b-829839a51937` from thread
`01a00d0f-ea0b-7671-b6c5-e5b1cbd2bf62`:

```text
kind=observed
model=openai/gpt-5.6-sol
duration-ms=411200  ttft-ms=5220  final-output-bytes=1439
events native=8  local=50  remote=29  total=87
share native=9  local=58  remote=33  sum=100
model-calls=29
input-tokens=5167215  cached-input-tokens=5064448
uncached-input-tokens=102767  output-tokens=17401
reasoning-output-tokens=9057  total-tokens=5184616
tool-calls=28  tool-outputs=28  tool-wall-ms=13312
tool-output-bytes=210333
form-commands=53  form-failures=3
native-commands=8  native-failures=0
form-stdout-bytes=1943201  form-stderr-bytes=387
form-shown-bytes=1943588
```

The health reading from those actual numbers is precise but modest: completed
tool accounting is 28/28; native wrapped failures are 0/8; all wrapped command
failures are 3/53 (5.66%); cached input is 98.01% of provider-reported input.
The three failures remain visible even though the *measurement* is valid. A
failure is not evidence corruption; hiding it would be.

## Failures attended and resolved

The existing `form-cli-band` initially answered **335874 / 524287**. The
static share stamp had been invisibly prefixed to every response, breaking
fifteen exact response contracts. Removing that prefix restored every one of
those bits. The next answer, **520191**, isolated only bit 4096. A direct ask
showed the actual response:

```text
[ask: native and local lanes are quiet]
local-lane:quiet
synthesis-lane:quiet
```

That was not an implementation failure. It was the router's explicit current
contract and the band had not learned it. The band now accepts the exact quiet
outcome as an attributed observation alongside grounded hit and grounded miss.
It answers **524287**, exit 0.

The new evidence band's first preflight found one stray closer and held its
verdict outside the witness. Removing that one closer produced a clean
preflight and **4095**. The live surface then revealed one more small wound:
printing each fragment put every value on a separate line. Composing one
complete string before the print reshaped the surface without changing a
number.

## Witness

```text
Python carrier tests                           2 passed
form-cli-turn-evidence-band preflight          clean
form-cli-turn-evidence-band                    4095 four-way
form-cli-offline-band preflight                clean
form-cli-offline-band                          2016 four-way
form-cli-band preflight                        clean
form-cli-band                                  524287 fkwu, exit 0
form-cli-share-run preflight                    clean
form-cli-share-run                             kind=observed, sum=100
```

## The teaching left behind

The most surprising teaching was that `embodied` is not a softer synonym for
`observed`. It is a useful middle state only when it protects the exact place
where evidence entered but reconciliation stopped. Once real carrier evidence
exists, continuing to type an “embodied offering” is less loving than allowing
the measurement to disagree.

Discomfort turned to gold when repairing the obvious static prefix did not
make the full band green. The single remaining red bit forced a direct ask and
revealed that quiet already had an honest, attributed shape; the witness was
old, not the living response. The failure became attention, the attention
became distinction, and the distinction became a contract that can now fail
truthfully again.

Signed: Codex

; witnessed: 2026-08-17 -> turn evidence 4095 four-way; offline core 2016
;   four-way; form-cli 524287 fkwu; live completed-turn kind observed;
;   boundary-event share 9/58/33; Python carrier 2 tests passed
