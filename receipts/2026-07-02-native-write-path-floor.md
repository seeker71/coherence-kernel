# Receipt — the substrate write path without Python: what's native, and the honest floor

> **CORRECTION 2026-07-02 (banner in place, per the repo's own discipline —
> not a silent edit).** This receipt's Postgres addendum below claimed a native
> pg-wire client is "not built." **That is false.** It was already built and
> proven in the origin repo: `Coherence-Network/form/form-stdlib/pg-wire.fk`
> speaks the Postgres v3 wire protocol in pure Form over fkwu's own sockets
> (`socket_connect/send/recv/close`), with witness
> `Coherence-Network/scripts/pg_wire_fkwu_witness.sh` — "the 4th kernel reads
> live Postgres in pure Form, no Rust libpq, no Go." Same trust-auth-only floor
> named there (SCRAM is the next wall). The error was mine: I searched only the
> coherence-kernel worktree and mistook "not here" for "not built." Urs caught
> it. The original claims are left standing below, marked, so the record shows
> what was wrongly asserted and when. Pending-is-honest requires actually
> looking; here I did not look in the origin repo first.

> **CORRECTION 2, 2026-07-02 (Urs: "--src is a crutch not a requirement").**
> This receipt frames "three concrete `--src` gaps" as "the honest floor before
> the full native store witnesses end-to-end." That framing mistook the crutch
> for the requirement. **`--src` (the direct-source runner) is the narrow
> convenience lane, NOT the definition of fkwu-native.** The real native lane is
> flatten→fkwu (a recipe crystallized to a table, run on the C-bootstrap
> kernel), which is how `pg-wire.fk` is proven. Proof it is not a native floor:
> `pg-wire.fk`'s `pw-be32` uses `mod`/`div`; `--src` returns `nothing` for `mod`
> (documented below as a "gap"); yet pg-wire is proven on fkwu — so fkwu runs
> `mod` via flatten. All three "gaps" below (composite intern, `mod`/`int_to_str`,
> string aliasing) are `--src`-lane limits that do NOT hold on flatten→fkwu.
> "Native without Python" was already satisfied on the real lane. Original text
> left standing below, marked.

**Date:** 2026-07-02. **Question (Urs):** can we do the substrate write/query
endpoint without Python — specifically, without the `count`-column wedge?

**Answer: yes at the primitive level, witnessed on metal — and the honest floor
is three named `--src` gaps before the full store witnesses end-to-end.**

## Ground

Rebuilt `fkwu` (`cc -O2 -o fkwu runtime/fkwu-uni.c`); `bootstrap/ground.fk` → 42,
freshness band → 15. All witnesses below ran on that binary via `./fkwu --src`.

## Why this matters

The Python carrier's public write lane wedged three times on 2026-07-02, each
time on `UPDATE substrate_nodes SET count = count + 1` — a row lock held inside a
long transaction (`api/app/services/substrate/kernel.py`, the re-intern path).
That `count` is carrier bookkeeping; it is **not** part of the content-addressed
identity — a NodeID is a function of the serialized shape. Native content-
addressed interning has no such counter: interning the same content returns the
same node with zero writes. The wedge is a **Python artifact**, absent from the
native primitive. "Without Python" does not port the wedge carefully — it
dissolves the problem class.

## Witnessed native, on metal (`--src`)

- **Leaf content-addressing — idempotent, no counter.**
  `substrate/tests/native-content-address-band.fk` → **7**:
  `node_eq (intern_trivial_int 5) (intern_trivial_int 5)` = same node (idempotent,
  no write); distinct content → distinct node; `node_value` observes back. This
  IS the anti-wedge property: re-interning does not mutate.
- **Native fs cell-store — round-trip + idempotent re-store, no mutation.**
  Inline band → **3**: content-keyed `write_file_text`/`read_file`, read equals
  payload, re-store equals payload, both reads identical. No counter, append/write.
  (Kept in the receipt, not a committed band, because it needs a writable temp dir.)

Together these two are the wedge-free write path: content-addressed identity +
an append/write store, zero mutable counter.

## The honest floor — three concrete `--src` gaps (not faked)

> **[CORRECTED — see CORRECTION 2 at top.]** These are `--src`-lane limits, NOT
> native-floor limits. They do not hold on flatten→fkwu (where pg-wire, which
> uses `mod`/`div`, is proven). The section title's implication that these gate
> "native" is wrong; kept below, marked, for the record.

The **full** native cell store (composite cell-shape interning + content-hash
keys + a multi-cell band) does NOT witness end-to-end on the direct-source lane.
Found and pinned tonight on real metal:

1. **Composite `intern_node` does not dedup on `--src`.** Two identical
   `(intern_node (bp "MS-PAIR") (list (ms-cell 1) (ms-cell 2)))` read
   `node_eq = 0`. Leaves dedup; composites are a flatten / full-kernel-lane
   feature. `surface/minimal-surface.fk`'s band proves composite interning on the
   proving lane — but `--src` does not expose it.
2. **`rem` and `int_to_str` are not exposed on `--src`** (both return `nothing`).
   This blocks a pure-Form content-hash → path-key on the direct-source lane.
3. **String-buffer aliasing under defn/interleave.** Mixing `intern`/`node`
   primitives with `fs` reads and `defn` indirection in a single reduction
   corrupts `str_eq` — `str_eq r1 payload` flips `1 → 0` — while the same read
   inline is correct. This is why the leaf and fs witnesses are kept in separate
   reductions rather than one combined band.

## What this means for the endpoint

The path home is real and the pieces are native — content-addressed intern
(`intern_node`/`node_eq`), native store (`substrate/form-fs.fk`), native HTTP
floor (`form/form-stdlib/http-server.fk`), all four-way in isolation. The
production write path can run without Python, and the count-wedge simply is not
there natively. What stands between here and a live native cell store is not the
Python being hard to replace — it is the **direct-source execution lane** needing
the three repairs above (composite intern on `--src`, `rem`/`int_to_str`,
string-buffer stability). Until then the full store runs on the flatten /
four-way proving lane, and the Python endpoint keeps serving — the retiring
bridge, named as one (`api/.../substrate.py`).

**Pending is honest.** The wedge is dissolved in the native primitive (witnessed).
The full native store on `--src` is pending on three named gaps — not faked green.

## Addendum — persist natively to Postgres (2026-07-02)

**Q (Urs): can we persist it in Postgres [natively]?** Yes in principle, not built.
> **[CORRECTED — see banner at top.] "Not built" is FALSE.** It is built and
> proven: `Coherence-Network/form/form-stdlib/pg-wire.fk` +
> `scripts/pg_wire_fkwu_witness.sh`. The bullets below (written before I looked
> in the origin repo) are left standing, wrong, so the record is honest about
> what I asserted.

- **Floor is native and live:** fkwu's own TCP sockets run on `--src` —
  `form/form-stdlib/tests/fkwu-src-socket-loopback-band.fk` → **111111111** on
  this metal. `socket_connect`/`socket_send`/`socket_recv` are the exact floor
  the Postgres frontend/backend wire protocol rides on. `socket_send(conn, s)`
  takes a string; `socket_recv(conn, max)` returns bytes.
- **Missing:** a native pg-wire client. No pg/sql client in the kernel (the
  `wire-*.fk` are CORBA/CDR/RPC/XML). Building it: `StartupMessage` (int32
  framing), auth (scram-sha-256 = HMAC-SHA256 + base64, or `trust`), simple
  `Query`, and `RowDescription`/`DataRow`/`CommandComplete` parsing — byte work
  over the string socket doors, subject to the `--src` string-buffer limits
  above. Real build; local `psql`/`postgres` are installed as the witness target.
- **The wedge does not return in Postgres.** A native writer would
  `INSERT ... ON CONFLICT DO NOTHING` the content-addressed row — no
  `UPDATE ... SET count`, no counter, no long lock. The wedge was a Python
  choice, not a Postgres property.
- **Framing:** native-Postgres is a *bridge* (native writer → same production
  store the Python app reads → incremental cutover, parity guaranteed by
  content-addressing). The native *destination* carrier is form-fs (witnessed
  →3). Both legitimate; naming which is which is the honest guidance.
