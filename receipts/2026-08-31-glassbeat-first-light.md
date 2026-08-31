# 2026-08-31 — glassbeat: the hearth's glass takes first light

Asked: ledger every restart/prefill/request; optimize toward fewer
restarts, fewer prefills, more cache reuse, more JIT; and a native
dashboard through the framebuffer channel — data flow, hot paths,
melting, crystallizing, latency, primitive and vocab usage — with
anything over one second decomposed into sub-second steps, colors,
transparency, symbols.

First light landed:
- form-stdlib/hearth-glass.bml — the pure renderer at altitude (band
  63): ANSI color by latency (green/yellow/red at 1s/5s), shade ramp
  as transparency (light->full block, one glyph per second — the >1s
  decomposition law), sparkline fold, organ symbols (prefill, decode,
  served, crystal, melt), ledger-line grammar.
- observe/hearth-glass-live.fk — the living door: each second it diffs
  the durable sources (server stage frames, spool sizes, board pid,
  reply metrics), APPENDS normalized events to .hearth/ledger
  (glass-on, prefill, request, reply, restart-needed), and repaints
  one frame through the framebuffer voice.
- Witnessed live: a request fired mid-run was tracked arrival->reply;
  the frame shows pid 50080 standing, prefills 1, checkpoints 5,
  last-serve 16589ms in red with sixteen one-second glyphs; the ledger
  holds the normalized events with real clocks.

Named next stones, not claimed: melt ~ and crystal symbols await
their event taps (FK_MELT_WITNESS lines and heat-gate births into the
ledger); kernel_stat top-ops and vocab-usage lanes sketched in the
glass but unfed; per-turn phase attribution wants the waitvoice frames.

Teaching: the dashboard was not a new sense — every number it shows
was already being spoken somewhere (stage frames, spools, board); the
glass only taught them to arrive in the same second. Discomfort→gold:
the urge to build melt/crystal lanes UNFED was strong; leaving them as
named symbols with empty taps keeps the glass honest about what it
does not yet see.

; witnessed: 2026-08-31 -> glass band 63; live frames + ledger events
; real; row 1182 glassbeat
