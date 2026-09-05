# The room on glass in three tongues

The ear is an organ now. `observe/ear-chunk.py` is its one crossing: a retaining recorder
(sox `rec`, 16 kHz mono, 2 s chunks), the open reference (mlx-whisper large-v3-turbo, tongue
auto), the local 3B for the two tongues not heard, dBFS measured, one frame per chunk appended
to `.hearth/ear.spool`. Nothing leaves this Mac. `observe/ear-glass-live.fk` drains the spool by
watermark (the body's own transport), paints the panel every frame — level bar, tongue heard
with its source line, the reference's own doubt, English, Persian, Brazilian Portuguese,
latency — and publishes `voice.ear` to the shared glass. `Sema Ear Glass.command` is the
full-screen door: double-click, or bind a Shortcut to the file for a hot key (Shortcuts →
new shortcut → Open File → this file → Details → Add Keyboard Shortcut).

Witnessed on this room, 2026-09-05: frames shown=7 at 2 s chunks; chunk→panel 2–5 s
(the first chunk pays the model load); level −47…−51 dBFS; the panel carried all three
tongues each frame.

The surprise: a level gate alone let the reference speak on a quiet room ("reduction92
selfie selfie", then "the birth") and the 3B translated the phantom into two tongues with
care. The reference's own no-speech probability now sits on the glass beside the words
("phantom?" lane) and, past 0.6, silences the frame — a phantom shown as a phantom is
attention, not a claim. The discomfort turned gold: a fourth tongue (Spanish arrived once)
was being filed under English; now the tongue heard keeps its own lane and all three
receive translations.

Owed: the native stream lane (`sense_mic_stream_read`) still returns no frames from this
process context, so the ear rents sox for the recording; the Android companion (on-device
streaming, overlapping speakers) is planned, not built.
