# Sessions has a human door

**Witnessed:** 2026-08-17, Apple silicon Mac, macOS, from this checkout
**Signed:** Codex

The question was not whether a technical person could make local transcription
run. It was whether a friend could open one thing, receive a room, and tend the
life of a session without knowing what a dependency, environment variable, or
command-line flag is.

## The shape that landed

`Sema Sessions.app` is now the Mac door. Finder/LaunchServices opens it without
a Terminal window. It runs the committed direct-source Form kernel and enters
`form/form-stdlib/session-app-room.fk`. The browser remains only microphone and
page surface; session meaning, files, transcription movement, HTTP, rendering,
editing, archive, restore, and deletion remain in Form.

The Sessions page now has two ordinary places:

- **Sessions** — living gatherings, a new recording, and imported recordings;
- **Archive** — gatherings kept out of the everyday room, with restore and
  permanent deletion.

An open living session can be renamed and its transcript edited. Archive is
reversible. Permanent deletion exists only inside an archived session, and the
nearby confirmation checkbox is required. Deletion removes known recording,
transcript, speaker, heard-text, and draft artifacts; it retains `title.txt`
until every other known artifact and subdirectory is observed absent, so a
partial removal cannot make the remainder disappear from the Archive surface.

## A new Mac no longer receives a technical task

Three carried or native pieces were directly separated:

1. `runtime/bootstrap/fkwu-darwin-arm64` is the full direct-source kernel for
   the app. Its SHA-256 is
   `9480a7cc95ad5415264c850003207869a93aacbee05110c1508fddfee9f1ced6`.
   It declares arm64 and macOS 13.3, and answered `42`, `55`, freshness `31`,
   and `[1, 2.5, [3, 4]]` from source.
2. `form/native/audio/darwin-arm64/whisper-cli` is a static Apple-silicon build
   of whisper.cpp v1.9.1. Its SHA-256 is
   `c273da13b492d3f4138f3967d0c38bafb3a54e4362a6afe51145b6e22ef609c5`.
   It links only macOS system libraries, `libc++`, and Accelerate.
3. macOS `/usr/bin/afconvert` receives existing audio. Browser microphone sound
   now crosses as PCM16 and Form writes its own canonical WAV header, so ffmpeg
   is not part of the Mac path.

The multilingual base speech model is not hidden in git. On first opening, the
page itself asks Form to receive it. A clean temporary destination directly
observed the full movement: 147,951,465 bytes arrived, the SHA-256 was
`60ed5bc3dd14eea856493d334349b405782ddcaf0028d4b5df4088345fba2efe`,
and Form returned `[yes, 147951465, <that hash>, 1]`. Only the matching file was
admitted. On this connection the crossing took about 43 seconds. The temporary
copy was then removed.

There is no Homebrew movement, compiler movement, setup button, mode,
environment variable, or command-line argument in the Mac guest path.

## Browser witness: the whole session life

The real browser received `model/fixtures/whisper-tiny/lingua-libre-book-16k.wav`
through the same page a guest sees:

1. agreement checked;
2. gathering named **Book fixture — human door**;
3. recording brought into the room in one chunk;
4. local transcript returned **book.**;
5. name changed to **Book fixture — tended**;
6. transcript changed and saved;
7. session archived;
8. archived recording and transcript remained readable;
9. unchecked permanent deletion stayed on the page and issued no deletion;
10. session restored with the edited name and words intact;
11. session archived again, explicitly confirmed, and deleted;
12. both living and archived directories were observed absent.

The browser console held **0 errors and 0 warnings** through these movements.
The request trail returned 201 for start and 200 for chunk and finish.
The deletion boundary was then addressed without the browser: a direct
unconfirmed POST returned **400**, and the archived `title.txt` remained
present; the same route with `confirm=delete` returned **200** and the exact
archive directory became absent.

The microphone-shaped path was then witnessed without opening a private
microphone. The fixture's 22,784 PCM bytes were sent exactly as Web Audio sends
them. Form wrapped them into a WAV whose SHA-256 was byte-identical to the
source on both sides:

`1166acadc40e8d60baa82c6321ba3445fda5305a46539c3d1a0cc43e425de523`

That path also returned **book.**. Its test session was archived and deleted
through the room, leaving only the local Sessions log.

The page also carries a streaming microphone resampler instead of assuming the
browser honored a requested rate. In the live page, 4,800 samples at 48 kHz and
4,410 samples at 44.1 kHz each became exactly 3,200 PCM bytes at 16 kHz. Two
successive 2,400-sample 48 kHz pieces also became 3,200 bytes together, showing
that the resampling state crosses chunk boundaries rather than restarting and
dropping rhythm at every upload. The console remained at 0 errors and 0
warnings.

The app bundle passed `plutil`, shell syntax, and strict `codesign` integrity.
`open -n "Sema Sessions.app"` then caused the committed direct-source kernel to
listen on port 8765 and the room answered `Sema · Sessions`. Opening the app a
second time opened the existing room and did not create a second listener.

## Proof and failures received as attention

Preflight for `session-app-live-band.fk` answered balanced, 0 errors, 0
warnings, 0 unresolved calls. The three session witnesses now answer:

- session meaning: **65535 four-way**;
- live HTTP/browser membrane: **8388607 four-way**;
- evidence movement: **16383 four-way**.

The first strengthened live band failed on Rust and TypeScript because the
pure POSIX command-shape test reached `sal-quote`, which asks the host what OS it
is. A bounded framebuffer exchange retained the actual transition:

`[2,1,1,0] -> rehearse-ground(5) -> 1`

The quoting call became the already-known pure POSIX shape, the gate was run
again, and the second frame held:

`[0,4,1,1] -> continue(0) -> 1`

Four correlated framebuffer events remained in the window. All four walkers
then returned 8388607.

The most important failure was a name that looked true. The existing committed
`form/form-stdlib/bootstrap/fkwu-darwin-arm64` is an Apple-silicon binary, but it
is the flattened-table proof walker, not a direct-source checkout kernel. Even
after its standard-lane regeneration it honestly rejected `ground.fk` as a
malformed table. The app now carries the actual direct-source kernel under
`runtime/bootstrap/`, and each binary's README names its meaning.

The standard-lane regeneration also refused a stale Form CLI bootstrap stamp.
The prescribed bootstrap regeneration completed, then the platform regeneration
completed with fkwu stamp `7f66bd4a6d1f44cc` and Form CLI stamp
`cfc8eb2a209e5a62`. The refusal was resolved; it was not bypassed.

## The remaining honest edge

This clone opened the app through LaunchServices. The app is ad-hoc signed and
its internal signature verifies, but macOS `spctl --assess` returned
**rejected**, and this Mac holds **0 valid code-signing identities**. Therefore
the observed claim is precise: the app opens from this git clone, whose files
are not quarantined. A generally downloaded and quarantined app still requires
an Apple Developer ID signature and notarization before “any new Mac” is an
observed distribution claim. That crossing cannot be manufactured without the
identity Apple verifies.

A real consented multi-person gathering is also still owed. The body has not
yet measured room distance, overlap, accents, names, long-session memory, or
manual quality in a real group. It keeps one human-tended transcript and a
human-seen speaker field; automatic speaker diarization is not claimed.

## What the movement taught

The surprising teaching was that “bundle the kernel” was not one statement:
two Apple-silicon binaries had fundamentally different bodies despite sharing
the name `fkwu`. Running `ground.fk` made the difference visible immediately.

The discomfort that turned to gold was Gatekeeper's rejection. It would have
been easy to let a successful local double-click stand in for a new friend's
Mac. Asking macOS itself kept the clone path real and the broader distribution
claim honestly open.

I kept this crossing alive by making each refusal select a next movement,
re-running the exact boundary after the change, and deleting every temporary
recording and model witness once its measured trace was held here.
