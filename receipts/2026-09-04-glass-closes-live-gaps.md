# Glass closes live gaps

Date: 2026-09-04  
Witness: Grok, in relation with Urs

## Movement

The atlas still had holes after doing became visible. A lone request
sample stole the pipeline and painted T/X/M as `--`. A five-second
lease marked a living owner's last snapshot stale, so ninety live
samples wore `+` while the process was still there. One-shot frames
had no predecessor, so G/C/D died as `?` and cadence unavailables
filled SEAM. Metric wallpaper ate the sample rows. `iostat -d 1 1`
blocked a second per host cycle. A bare owner `status` file was
healed away as uncorrelated, so the snapshot never refreshed.

Token-flow now requires a token-position stage, so SCHEMA reads the
owner's live T/X/M instead of a request-only MIXED pipe. Dual-resident
samples stay physical-live while the owner PID lives; age stays on
the stall. Flow, host pageins, and completed-frame cadence persist
across one-shot frames. Valued samples keep three rows. Host load is
numeric. Pageins delta is `pin=+N`. Owner `status` is offered with
the lock the resident actually polls, and a bounded frame waits one
poll so the refreshed snapshot is the one it reads.

## Physical Glass witness

Glass atlas panel **#0** at **02:01:53.903Z**:

```text
SCHEMA Q#0e ->T*27 ->L+1  ->X*9Gi->M*1K ->F*104->G*1
DOING n=7 cpu=99% rss=539MiB load=6.9 owner=87MiB cpu=6% T*p21 stall=37m pin=+4M
PRESS free=71% used=29% ok file=2M cmp=2M rss=87MiB cpu=6% gov=2
s O*1K ... T*21 ... T*6  .*73G.*9Gi
now *165 +22 #25 ?7 valued-s=96
```

The owner PID is alive (87 MiB, 6% after a status publish). Qwen is
still at context 21, last moved 37 minutes ago — that age is the
token, not the snapshot. `pin=+4M` is pageins since the previous
host sample, so a stall can be checked against motion instead of a
cumulative wallpaper. SCHEMA is live T/X/M/F/G, not MIXED holes.
SEAM names seven real missing doors, not first-frame cadence.

The dual-resident snapshot file moved from 09:24 to 10:01 on that
same poke. The command directory was empty afterward: the owner
consumed `status` and republished.

## Proof

- binary freshness: `31`
- telemetry membrane: `2097151`
- observer: `8388607`
- live-ui: `1073741823`
- live: `1073741823`
- deadline cadence: `4095`
- token-flow UI: `14319`
- preflights balanced, zero errors
- `git diff --check`: clean

## Closing

Alive: SCHEMA from owner data, stall age on a live PID, pageins
delta, load, and a snapshot that refreshes when asked.  
Most surprising: a missing lock file made every status poke look
like success and delete itself.  
Discomfort into gold: five seconds of silence on a living owner
was being painted as derived, and the request-only flow was
covering the tokens that were already in the snapshot.
