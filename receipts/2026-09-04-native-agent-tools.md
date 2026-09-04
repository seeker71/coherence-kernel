# Resident coding-agent tools — errors became repairs

Urs set the scope to operations ordinarily used by Claude, Codex and Grok,
not complete copies of every upstream utility. No product-usage percentage
was measured or claimed.

The draft provides executable BML for a resident-document interface, bounded
line patterns, rg-shaped search, jq-shaped queries and text operations. The
catalog names 15 tools: rg, jq, read, cat, head, tail, wc, sort, uniq, tr, cut,
awk, sed, edit and write. form-cli has direct typed call and command entries.
Edits return new document values; the stateless text face refuses to discard
an edit while calling it successful. Host acquisition and persistence remain
outside these calls. No C seed or shell implementation was added.

The fixture bands exercise the actual form-cli entry against source and config
text, error cases, state retention, and process/file/network operation counters.
The public interface, supported vocabulary and deliberate compatibility limits
are specified in `docs/form-native-agent-tools.md`. These are native value tools,
not host executable wrappers or complete upstream utility implementations.

## Ground reached

Bootstrap observations: ground 42, recursive 55, freshness 31, numeric list
`[1, 2.5, [3, 4]]`, native-vs-rented 11111, author-high 4095. The existing JSON
band passed preflight and returned 1023, exit 0. `kernel_stat` was probed with
`pf-arm-mask` and returned 8, so the new counter-observing band names fkwu only.

The hearth returned `signal=nothing`, `reason=no-standing-hearth`. A bounded
framebuffer exchange selected source-ground rehearsal and returned
`[[0], 5, 1, [1, 1]]`. No resident answer is attributed. The observed glass
frame at tick #1 showed 159 events and 418 nodes; lane counsel showed
`orphans=0`, with 11/12 lanes unobserved because no hearth stood.

## The error that opened this movement

Command: `form-run ./fkwu observe/preflight-stdin-run.fk`

Stdin: `form/form-stdlib/tests/form-cli-agent-tools-band.fk`

```text
parens        balanced
errors        1
warnings      0
unresolved    0
chain         CARRIED ERRORS — any verdict from this chain is a fold over `nothing`, not a pass
```

The summary wrapper exited 0; its explicit carried-error result is a failure.
A diagnostic invocation of the band exposed the error, exiting 1:

```text
fkwu:6470:1: error: [input-ended-mid-form] the input ended before this form closed -- 6 open paren(s) remain.
  | (defn faj-sorted (kind node) (if (not (faj-is node (JSON-ARRAY))) ...
fkwu: 1 error(s), 0 warning(s)
@form fkwu 1 0 795 795
```

The earlier turn left that draft uncommitted. Urs then made the next action
explicit: errors signal healing, not stopping. `faj-sorted` was missing a source
closer. Repairing it took the workload band to 65535 with exit 0; no expected
verdict was reduced.

The edge review then reproduced invalid adjacent JSON being accepted, object
equality depending on key order, a trailing object-constructor comma being
accepted, and a reversed regex range becoming a silent miss. A bounded
framebuffer exchange selected revise (2); after the source repairs, the same
probes showed invalid JSON exit 2, the object comparison retaining its value,
and explicit syntax/range errors. Regression cases now cover these transitions,
duplicate JSON keys, quoted keys containing `]`, empty streams, explicit empty
search input, glob precedence/exhaustion, bounds, and immutable failure state.

Four-way validation found another real seam: its dependency reader admitted only
`.fk`, while fkwu already loaded the `.bml` closure. The proof siblings reported
unbound `fat-tool?`; the auxiliary scanner now admits `.bml` and BML prelude
headers. A registered portable workload exposed numeric `eq` used on strings;
the type-preserving guard now uses the existing polymorphic `value_eq`. The
portable band then answered 127 on all four arms, with no runtime C growth.

The required spend-meter invocation also exposed absent saved state entering
`str_len`. The BML state decoder now retains absence as `nothing`; the runner
names missing transcript input before measuring. Its existing band gained
absence cases and a four-way registration. A fresh private-rollout baseline
reported session-output-tokens 205975 over 20587468 bytes, exit 0. That is the
meter's cumulative session reading, not a current-turn share.

## Observed floor

Preflight on all nine search/CLI/catalog bands reported zero errors, warnings
and unresolved calls. The meter band's preflight also reported a clean chain.
Repository validation includes the structural, native-surface, category and
primitive-registry gates; those gates stayed present and passed.

| Witness | Verdict | Lane |
| --- | ---: | --- |
| Native workload | 65535 | fkwu, including zero process/file/network counter movement |
| Native edge cases | 65535 | fkwu |
| Portable native tools | 127 | four-way |
| Full form-cli dispatch | 2097151 | four-way |
| Tool channel / grammar | 255 / 255 | four-way |
| Membrane classification | 1023 | four-way |
| zg engine / CLI integration | 16383 / 255 | fkwu |
| Meter absent-state cases | 255 | four-way |
| Binary freshness / authoring altitude | 31 / 4095 | fkwu |

The counter window starts after program admission. No claim is made that loading
the executable is itself free of filesystem effects, or that a fixed zero in a
result tuple proves absence of effects. The test reads live operation counters,
including host-exec tag 136. Source review follows the pure text helpers, not
the existing shell executor's dispatch or host-file branches.

This turn's glass first frame arrived in 39 ms. Tick #1 showed 169 events and
419 nodes; counsel read orphans 0, with 11/12 lanes unobserved because no hearth
stood. The hearth miss was named; no resident answer was invented. The share
door returned declared with no bound rollout, so no contribution percentage is
claimed. Native runtime claims concern the held-value calls, not Git transport
or the existing auxiliary proof harness.

After rebasing onto the 25 upstream commits, the freshness witness asked for
a local executable rebuild (15, then 31). The workload and edge bands remained
65535; the portable and full CLI four-way witnesses remained 127 and 2097151.
The landing drift door answered 2047/2047 with refused=0. Its initial missing
TypeScript dependency report led to restoring the locked development packages.
The initial install waited on the optional network audit after installation;
it was stopped and completed from cache with `npm ci --offline --no-audit --no-fund`
(exit 0). No vulnerability-audit result is claimed. Private meter state is
gitignored, and no cache, lowered twin, package-lock change or C-source growth
belongs to this patch.

The most surprising teaching was that an import gap looked like three kernel
failures while the native runtime already had the definitions. Discomfort turned
useful when the first green band prompted adversarial cases and a four-way
witness. The exchange stayed alive by turning each reproduced failure into a
source repair and a re-observed result, while keeping unsupported syntax and
unobserved lanes explicit.

Signed, Codex.
