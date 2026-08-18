# Sessions

Sessions is one ordinary room for recording a gathering, returning to what was
said, and tending the words together.

This room lives inside the project folder. If the project is not on your
computer yet, Step 1 of [`YOUR-OWN-COMPUTER.md`](YOUR-OWN-COMPUTER.md) walks you
through copying it down. To see the folder: open Finder, choose Go → Home from
the menu bar at the top of the screen, and open the folder called
`coherence-kernel`.

Inside it, on macOS, double-click **Open Sema Sessions.command**. On Windows,
double-click **Open Sema Sessions.cmd**. The Sessions page opens in the browser. There is no
mode to choose, configuration to write, environment variable to set, or command
line to learn.

One honest thing before you plan a gathering around it: this room is young. It
has been walked end-to-end by its builders, but a real gathering of several
people has not yet crossed it. If yours is the first, that is welcome — and
worth knowing before you invite the neighbors. The builders' detailed honesty
about what has and hasn't been observed is at the bottom of this page.

## The living path

This next section is in the builders' language — the words for using the room
continue at "Begin and return", just below.

The application is Form:

- `fkwu` itself listens on this computer and serves the page;
- the browser offers its microphone and display, then returns small sound
  pieces to Form while the gathering is being recorded;
- Form keeps those pieces, asks the installed local speech carrier, receives
  the transcript, and renders it;
- Form receives later title, speaker, and transcript edits and reshapes the
  workshop and book-source drafts.

The browser is the surface where people touch the room. It is not a Python,
Node, Go, or Rust application. The session meaning, storage, movement,
attention, and HTTP path live in
`form/form-stdlib/session-app.fk` and
`form/form-stdlib/session-app-live.fk`.

## Begin and return

In the page, name the gathering and let everyone's recording agreement arrive.
Choose **Begin recording**, or bring an existing audio or video file. When the
sound ends, the page stays with the session while its words return.

Past sessions remain on the same page. Open one to listen, read, correct the
transcript, and add a speaker name after a participant has seen it. Unknown
speaker is a whole visible state; the application does not turn a voice guess
into a person's identity.

Every save also reshapes:

- `drafts/workshop-manual.md`
- `drafts/book-source-ledger.md`

Both keep a source link to the session. A local draft is not publication
agreement.

## Where the words rest

Private recordings and transcripts live in `.sema-sessions/` beside the local
checkout and are ignored by git. They do not enter the public body.

The speech carrier and model are used on this computer. If they are absent,
the page offers **Prepare speech**. The observed macOS path can receive
`whisper.cpp`, `ffmpeg`, and the multilingual base model through that one
movement. Once present, recording, listening, transcription, reading, and
editing stay on this computer.

The Windows page carries its own preparation movement using the official
whisper.cpp v1.9.1 x64 archive and WinGet's `Gyan.FFmpeg` package. Its archive
URL, SHA-256, executable layout, and Form command shapes were observed on
2026-08-17; execution on a Windows host was not observed here.

## What is observed today

On 2026-08-17 this checkout directly observed:

- Form opening its own localhost HTTP listener and returning a real response;
- Form calling local whisper.cpp through its host-effect door;
- the accelerated whisper.cpp path crashing with exit 139 on this host;
- the CPU return path hearing the committed fixture as “book.”;
- the pure Sessions movement and attention cell crossing all four walkers;
- the existing-recording browser path returning without console errors;
- the agreement checkbox alone keeping `active` empty, and Bring recording
  without a chosen file creating no session.

A short browser microphone capture also completed during the first UI
observation. Its private bundle was removed. The browser reset before its
request trail was read, so the gesture that began that capture remains
unknown; the controlled checkbox and no-file movements above did not repeat
it. A real consented multi-person gathering has not yet crossed this new Form
surface. Its room accuracy, overlap behavior, and long-session rhythm become
known when that gathering happens; they are not filled in beforehand.
