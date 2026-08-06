# Receipt — real persistence for the come-in flow: stable relationship, or anonymity (2026-07-01)

**Follow-up to `receipts/2026-07-01-come-in-flow-form-native-port.md`.** That receipt named
public-API/session-account persistence as carrier-pending and stopped there. Asked to compare
against the sibling repo and improve toward the actual goal — *a stable relationship between
Sema and the user if they want to be remembered, anonymity otherwise* — this receipt closes
that gap for real, inside this kernel's own boundary, no Python/API involved.

## Comparison to Coherence-Network's `arrival.fk`

| Coherence-Network | coherence-kernel (this receipt) | Verdict |
|---|---|---|
| `register-persistent-identity` (substrate NamedCell) | `rs-remember` ([relationship-store.fk](../form/form-stdlib/relationship-store.fk)) — one real file per handle | Real persistence, simpler backing |
| `read-relationship` | `rs-recall` | Same idea, one string not an event list |
| `resolve-or-create-relationship-cell` (pairwise CONTACT-THREAD between two named identities) | not built | Still absent — this is single-sided (body remembers the arriving cell), not a general any-cell↔any-cell relationship graph |
| `resume-aware-meet` (continue if history exists, else welcome-with-orientation) | `come-in`'s `rs-known?` branch | Same shape |
| `start-or-resume-agent-session` / `bootstrap-persistent-agent-session` | `come-in` ([arrival.fk](../form/form-stdlib/arrival.fk)) | Direct equivalent, one call |
| `contribute-to-relationship` (channel-backed, APPENDS every arrival+resonance as history) | `rs-remember` (OVERWRITES one description) | Real simplification, not parity — no accumulating history log here |
| natural-language answer → consent value | still manual | Unchanged: whatever carrier exists still has to map "yes, remember me" to `(rc-open)` itself |
| identity derived from session/account | still absent, on both sides | `handle` is any string the arriving cell offers; Coherence-Network's own carrier doesn't derive it from a platform account either — both leave that to the human choosing a stable name |

## What's new here

- **`form/form-stdlib/relationship-store.fk`** — a real, file-per-handle store: `rs-remember`
  (write), `rs-known?` (has this handle ever been remembered), `rs-recall` (read it back),
  `rs-forget` (delete — honors "revocable always" from `reception-consent-policy.form`).
- **`form/form-stdlib/arrival.fk`** — `resonance-note`/`resonance-qualities` accessors (the gap
  named in the prior conversation), `welcome-back`, and the composed `come-in(dir, handle,
  offering, consent-value, description)`:
  1. no handle (`(nothing)`) → anonymous, **zero storage I/O**, identical to the bare empty-room welcome.
  2. handle offered, not yet known, consent closed → same welcome as full anonymity, **nothing written**.
  3. handle offered, not yet known, consent open → **real file write**.
  4. handle already known → **resumes** from what was actually remembered; a fresh, unconsented
     description offered on a later visit is ignored and the store stays untouched — no
     re-interrogation on return, matching `lc-arrival-as-recognition`.

## Proof — verified directly, across separate process invocations (not just within one run)

- `tests/relationship-store-band.fk` → **31** on `fkwu --src`.
- `tests/come-in-band.fk` → **31** on `fkwu --src`, covering exactly the four scenarios above.
- Beyond the band (which runs everything in one process), I re-verified the persistence claim
  the way it actually matters — across **separate `fkwu --src` invocations**, simulating
  separate breaths: wrote in one process, and in a fresh process confirmed `rs-known?` = true,
  the recalled content matched exactly, and an unremembered handle read back false. Then ran
  `come-in` itself the same way: consent-closed writes nothing (dir never created), consent-open
  writes for real, and a later invocation with a *different* unconsented description resumes
  from the *original* remembered one rather than overwriting it.
- **Not four-way.** All three minimal walkers lack the host-effect ops this needs
  (`fs_mkdir`/`fs_exists`/`file_append_bytes`/`read_file`/`str_byte_at`'s effects) — expected and
  correct per `walkers/README.md` (they prove the pure-recipe surface only). Witnessed on fkwu
  directly, which is where this effect actually has to run.

## Two real bugs found building this — both worth naming plainly

1. **`ord`/`char_at` are unbound on `fkwu --src`.** `form-stdlib/cell-log-store.fk` (this
   kernel's existing, scale-appropriate segmented log store) uses `(ord (char_at s i))` to turn a
   string into bytes. On the direct-source runner, `(ord (char_at "hello" 0))` returns `nothing`
   instead of `104`, and neither `ord` nor `char_at` appear in `runtime/fkwu-optable.h` at all —
   they're not implemented in this lane. Writing through `cell-log-store.fk` on `fkwu --src`
   silently produces a segment file of **null bytes** instead of the record text — a real
   data-corrupting gap, not a cosmetic one. `relationship-store.fk` sidesteps it with
   `str_byte_at` (confirmed correct: `(str_byte_at "hello" 0)` → `104`, and it *is* in the
   optable at tag 28) instead of reimplementing the segmented-log design. `cell-log-store.fk`
   itself is unmodified — this is named as a gap in it, not fixed in it, since fixing it wasn't
   this receipt's ask.
2. **`intern_trivial_string` values are not raw strings.** `str_concat`/`str_len` silently
   misbehave when given an `intern_trivial_string` result (`(str_concat (intern_trivial_string
   "a") "b")` → `-1`; `(str_len (intern_trivial_string "hello"))` → `0`) instead of erroring.
   `intern_trivial_string` produces a content-addressed node value (the right thing for `eq`
   comparison and for everything else `arrival.fk` builds), while `relationship-store.fk`'s
   `str_*`/`fs_*`/`file_*` primitives need genuine raw strings. `come-in`'s `handle` and
   `description` parameters are raw strings for exactly this reason — documented at the call
   site in `arrival.fk` so a future caller doesn't reach for `intern_trivial_string` there by
   habit and get silent garbage.

## Still honestly open

- No pairwise relationship graph (any-cell↔any-cell) — only the body remembering one arriving
  handle. Coherence-Network's `contact-thread`/`resolve-or-create-relationship-cell` generalize
  further than this does.
- No accumulating history — `rs-remember` overwrites; nothing here logs every past meeting.
- No mapping from a human's natural-language answer to a consent value — a carrier (today, the
  embodying agent, by hand) still decides that.
- No identity derived from a session or account, on purpose — `handle` is whatever the arriving
  cell offers, matching this kernel's boundary and, for what it's worth, Coherence-Network's own
  carrier too.
