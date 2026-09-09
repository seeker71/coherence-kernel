# Depth becomes attention and retained work

Signed: Codex, 2026-09-09.

The compiler-cache repair exposed a legitimate API artifact deeper than the
Go FORMBIN2 reader's fixed 256-level refusal. The native traversal now lives in
`form/form-stdlib/bml/formbin-depth.bml` and executes on fkwu through a retained
pipe process. Go receives postorder rows and performs physical NodeID interning.
No Python, substitute helper-language algorithm, or authored C was introduced.
The old Go FORMBIN2 recursive decoder was removed.

Depth crossings advance an attention watermark. Measured delay shrinks the next
slice; spare time grows it. Descent and completion both retain continuations.
There is no maximum-depth verdict or aggregate duration timeout in this path.
Ordinary collection activity remains visible as attention without repeatedly
halving fast slices. Native Form records correlated observation/control pairs;
the carrier keeps every round's actual observations, offered choices, selected
action and applied state in JSONL.

The earlier source-cache repair is included in this landing. The serving compiler
now loads and hashes the whole declared dependency closure. Once decoding passed,
the API revealed one missing parenthesis in `api-presence-supported?`, which had
swallowed later handler definitions. Repairing that source restored those handlers.

## Actual measurements

The [complete evidence](2026-09-09-adaptive-depth-evidence) includes the final
trace, scalar results and the full compressed traces of the slower attempts.
These are individual runs on this host, not a throughput benchmark.

| Depth-4096 attempt | Full child execution | Recorded rounds | Result |
| --- | ---: | ---: | --- |
| Initial Form algorithm on the Go proof walker | 2210 ms | 53 | 42 |
| Added instrumentation exposed warmed JIT copying | 26636 ms | 9778 | 42 |
| GC activity separated from measured delay | 25968 ms | 858 | 42 |
| Diagnostic run with automatic proof JIT held back | 985 ms | trace retained | 42 |
| Native traversal before rebase | 662 ms | 7 | 42 |
| Native traversal after rebase and freshness rebuild | **47 ms** | **7** | **42** |

The final decoder lifecycle is **40 ms**, including **11 ms** setup. It consumes
all **114720 bytes**, visits **8193 nodes**, reaches depth **4096** and finishes
with continuation depth **0**. The remaining full-process time includes Go
evaluation after decoding. The final seven native slice timings are
**1, 0, 0, 2, 4, 5, 0 ms**, at millisecond resolution. The trace retains carrier
intervals separately; they overlap native work and cannot be summed as separate
costs. Go carrier memory/GC fields do not claim native heap or system headroom.

The warmed proof JIT converted nested list arguments at its boxed-value ABI
boundary. An isolated held-back run confirmed that detour; the final solution
executes traversal in native Form and keeps its continuation there. Automatic
JIT was not disabled globally or in the final witness.

## Verification

- Native depth band **2047**, preparation band **255**, authoring guide **4095**;
  clean preflight diagnostics and process exit 0.
- Real native depth witness **1**, scalar roundtrip **1**. The scalar reader
  returns `[hello, 1.25, 4294967296]` after emission and reload, both exit 0.
- Original regression: `form-run go test -run '^TestSubstrateFormCompilerRouteRunsBML$' -count=1 .`
  from `form/form-kernel-go`, **PASS, 32.822 s** before rebase and **32.697 s**
  after rebase/rebuild, both exit 0. This is the targeted
  server regression, not a claim that the entire server suite ran.
- Native compiler-cache witness **1023**, exit 0, re-run against the final Go
  carrier. Cold preparation **1230 ms**, warm **36 ms**, source edit **35 ms**,
  compiler dependency edit **378 ms**, poisoned old cache ignored **72 ms**.
  All ten scenarios pass; their current per-stage JSON is retained.
- Shared conformance: **13** expressions, three-arm binary interchange and
  **12** malformed inputs pass. The formerly valid depth-257 negative vector
  is replaced by a genuinely truncated child-count field; deep validity has
  dedicated positive witnesses.
- Drift gates **8191/8191**, refused **0**, exit 0; whitespace check clean.
- Actual heal dispatcher returns the native source, executable witness and
  `docs/adaptive-artifact-depth.md` for the observed maximum-depth error.

The clean rebase includes origin/main through `8b03d055`. Its incoming native
changes made binary freshness return 15; rebuilding the local binary restored
31. Native cache artifacts then identified the different compiler build and
rebuilt themselves. The depth witness, native band, server regression and all
drift gates were re-witnessed on that combined tree. The full-process timing
change across these single runs is retained as observed; it is not attributed
to one upstream change. The earlier 662 ms trace remains available separately.

The self-panel reads **orphans 0** and **11/12** unobserved lanes because no hearth
stands. Native authoring reads **26** existing Python implementations, **325**
execution candidates, **50** grammar inputs, **0** unread paths. This movement
adds no Python and does not claim those existing paths were removed. Share
measurement remains declared with its percentage withheld while carrier
coordinates reconcile. No LoRA update, model throughput or hardware-floor
claim is made by an artifact-decoder witness.

This path covers Go FORMBIN2 admission through native Form. FORMBIN1/raw readers,
other proof carriers and the independent historical caches named in the earlier
receipt remain outside this migration. The complete protocol and timing scopes
are described in [adaptive artifact depth](../docs/adaptive-artifact-depth.md).

The surprising teaching was that observing a collector can create a shrinking
feedback loop, while a warmed JIT can make shared continuations expensive to
copy. The uncomfortable 26-second regression became useful evidence: following
it brought traversal onto fkwu and reduced the final decoder to 40 ms. We kept
the work moving by preserving every failed interpretation, changing the actual
execution path, and re-observing its result.
