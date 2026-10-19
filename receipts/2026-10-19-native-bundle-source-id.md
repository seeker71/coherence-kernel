# Native bundle source identity publication

The immutable application build at 9b43f07 passed native compilation and bundle
publication, then its runtime-stage COPY reported `/build/form-cli.source.id` not
found. The reusable copy publisher emitted recipe, symbols, attestation and
executable, but omitted the source-ID derivative its consumer already expects.

`fcb-copy-out` now stages the already-verified `want-id` with that bundle and
publishes its `.source.id` before the executable's final rename. Identity still
comes from the same checked source seal; this is not a second authority or a
Docker-specific workaround. A failed identity write prevents publication.

One focused native publisher observation retained exact bytes for the artifact,
recipe, symbols, attestation and newline-terminated source ID. The observation
used inert fixture bytes, never executed them, and is publication evidence only.
The actual immutable image build owns executable/identity integration evidence;
no private full rebuild or kernel/sibling suite was added. Runtime source is
unchanged. OpenAI supplied reasoning; Form performed the effect and observation.
