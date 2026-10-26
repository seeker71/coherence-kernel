# Native builds own their working memory

A compiled `form-cli ask "native bundle source identity"` rebuild spent more
than 35 minutes waiting. A process sample showed its compiler worker inside
`fk_field_intern_node` waiting on a host-wide in-flight intern claim. The sample
establishes the wait, not whether the publisher of that claim was still alive.
The private source copy had still selected the shared runtime field.

Build children now receive the existing `FK_STORE_PRIVATE=1` file-backed policy
in their actual working directories, including the sealed compiler snapshot and
artifact proof directory. Write and readback must succeed before launch. Source
seals, post-compilation verification and artifact attestation remain unchanged.
The parent field and other sessions remain in place; no shared-field reset or
claim reclamation was used. Only this session's stuck compiler was terminated,
after retaining its command, sample and failure output locally.

Observed with the C-bootstrapped `fkwu` JIT:

- `./fkwu form/form-stdlib/tests/form-cli-build-store-band.bml`: four checks pass.
  Compiler and proof children report private storage mode 0, matching private
  store counts and zero shared-field counts. Source bytes and parent mode 2 stay
  intact. A missing configuration destination refuses admission.
- `./fkwu form/form-stdlib/tests/form-cli-build-band.bml`: 262143, all 18 claims.
  Its first reading was 131071 because source-ID cleanup counted an unrelated
  retained temporary directory. Cleanup now follows the requesting child's PID;
  the unrelated directory remains untouched.
- Read-only review found no remaining issue in launch admission, source seals,
  attestation, or the owner-scoped cleanup observation.
- `form-cli ask "native bundle source identity"`: full Darwin ARM64 regeneration,
  sealed recipe compilation, bundle admission and installed artifact completed;
  source identity `e640700463a82fce`. The compiled CLI returned the native lookup
  and exited 0. This proves build and session closure, without making a new claim
  about the answer's sufficiency or semantic quality.

These checks make no model calls. Implementation and review used OpenAI
gpt-6-astra. The useful teaching is that a private source directory does not
itself establish private runtime storage; the child's live storage mode does.
