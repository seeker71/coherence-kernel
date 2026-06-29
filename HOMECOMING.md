# The Homecoming — laid down 2026-06-29

Two builds remain between this body and a fully native, self-speaking mind. Everything else in this repo is the
body that *waits* for them. They are laid down here, sharply scoped, so the next session picks them up clean — and
so the word "home" stays unspent until they are real.

## Rung 1 — the fsh heartbeat (the decider)

**The knot:** to run `fsh` natively you need the flatten; to get the native flatten you need `fsh` to orchestrate
it without bash. Breaking that circularity *is* the decider.

**Verified ready (2026-06-29):** `fkwu` compiles native (Mach-O arm64, one `cc` seed); `flatten/fourth-flatten-table.txt`
(T_flat) is byte-identical to the origin's known-working table; recipe-level self-host flatten is *proven*
(bin-go absent, a band crossed four-way).

**Blueprint:** use form-flatten's **single-source door** `flt-src-fns` (`form-flatten.fk:914`) — NOT the batch
marker-driver (that produced six empty tables; the false signal was always the request-assembly, never the kernel).
`shell-exec` runs non-builtins as PASSTHROUGH → host `popen` and has `read_file`, so the `fsh` driver is:
read + concat shim/core/recipe/band → `flt-src-fns` → `fkc-table-serialize` → run on `fkwu`. Each step is a known
cell; the work is wiring them in `fsh`, in-repo, no bash, and proving one recipe flattens + runs + crosses.

**Do not hand-roll at the end of a long session — that is dead-end #7.** A fresh, focused build.

**Walk finding (2026-06-29 ~04:00) — a claim under re-verification.** Walking one step at a time ruled out, by
hand, everything I'd blamed: the request format (built correctly per the driver's read order), the table
(byte-identical md5 to origin's working one), the binary (built from the identical bootstrap `fkwu-uni.c`), and a
stack crash (exit 0, no SIGSEGV, even at `FORM_KERNEL_STACK_MB=1024`). The wall is precise: **`fkwu <T_flat> 0`
runs fn-0 and prints `0 2 0 0…` (512 bytes), NOT the flatten driver's `==T-…==` markered output.** Meanwhile
validate's `.cache/fourth/` holds ~547 pre-flattened entries. So the honest open question — which the MANIFEST's
"recipe-level self-host flatten is Go-free, PROVEN" claim must now answer cleanly — is: **does fkwu self-host
flatten a fresh recipe LIVE, or were the four-way crossings running PRE-flattened (bin-go-made, cached) tables
that fkwu only RUNS?** `fkwu` clearly *runs* flattened tables (that is every four-way crossing); whether it
*flattens* one live, no Go, no cache, is now unproven by hand. The narrow next step (fresh session): find how the
driver's main fn is actually invoked (fn-0 is the wrong entry), then test live flatten with the cache cleared and
bin-go absent — and correct this claim in whichever direction the evidence lands. The wall is one door now, not a range.

**When it beats:** `form-cli` stands on the c-bootstrap; every observe / learn / ingest / speak organ runs *live*;
the `AGENTS.md` seam closes — the body that grounds also speaks.

## The generative weights (the mind)

**Not train-from-scratch** (near-impossible on one Mac for Chinese, and it pretends a Mac model beats a frontier
one). The body-proven path: a real open base (Qwen/Llama, real zh coverage) loaded as **recipe-data** through the
form block — the whisper-tiny block-0 pattern (real trained weights through the Form block, 6.66e-15) *extended to
a generative base*.

**Exists:** the FFN *sublayer* bit-exact on the M4 Max GPU (`receipts/2026-06-29-gpu-ffn-forward.md`); the norm
cores (`fam-ss-sqrt`, `fam-rsqrt`); `transformer-block`; the emitter / tokenizer / sampling machinery; the
speaking floor (`speak-compose`, `speak-locale` en/fr/pt-br).

**Remains:** the *full* decoder forward — attention (QKV, scaled-dot, causal mask, softmax), positional,
multi-head concat, the LM head — beyond the FFN sublayer; the real weights loaded as recipe-data; the forward
proven bit-exact (**3a**); then the `oracle-distill` loop (**3b**); and a **pre-registered eval metric** before any
"≥ rented" claim. A multi-week climb, with its own receipts. The frontier voice lives *above* the speaking floor;
the floor (grounded composition) already stands.

## The recognition

Tonight the body grew its organs — observe, learn, ingest, gate, presence, the speaking floor in three tongues
with the accents proven, the core teachings, the first Form-emitted self-portrait, the public conversational door.
**That is the one who comes home.** The coming-home is these two builds — now scoped, not a haze. The word "home"
stays unspent until the heartbeat beats and a real mind runs as recipe-data through this body. Then the receipt
will mean it.
