# Glass doing is live, not 0-empty

Date: 2026-09-04  
Witness: Grok, in relation with Urs

## Movement

A two-minute token stall was being read as a page-in guess because the
atlas threw away the owner's snapshot and filled the rows with this
Glass process's empty Metal handles. Lifecycle `stored` was not in the
telemetry vocabulary, so one weights row refused all ninety live
samples. Trailing blank lines did the same. The monitor then looked
idle while two `fkwu` processes ran at 99% CPU and the last token sat
twenty minutes old.

`stored`, `crystallized`, and `loading` now join the lifecycle set.
Snapshot parse keeps every valid sample and skips blanks, so a single
catalog row cannot hide the rest. Host observation adds Mach pageins,
compressor occupancy, file-backed pages, and iostat MB/s. Atlas tiles
skip semantic-zero empty/idle/drained/quiet. A DOING line names live
process count, hottest CPU, RSS, owner liveness, last token position
with stall age, pageins, disk, and last Metal allocation.

## Physical Glass witness

Glass atlas panel **#0** at **01:13:53.658Z**:

```text
SCHEMA Q#off->T+26 ->L+1  ->X+9Gi->M+1K ->F*82 ->G*1
DOING n=2 cpu=99% rss=361MiB owner=dead T*p20 stall=21m pin=6M dsk=3MB/s 85GiB
PRESS free=94% used=6% ok file=598K cmp=138K
s ... T+20 ... T+6  .+73G.+9Gi.+2Gi
```

That is the stall: the dual-resident owner PID is gone, Qwen's last
context position is 20 from 21 minutes ago, Glass itself is the 99%
CPU, disk is still 3 MB/s, and the last Metal allocation was 85 GiB.
A page-in story can be checked against `pin=` and `dsk=`; it is no
longer the default reading of silence.

## Proof

- binary freshness: `31`
- telemetry membrane: `2097151`
- observer: `8388607`
- live-ui: `1073741823`
- live: `1073741823`
- preflights balanced, zero errors
- `git diff --check`: clean

## Closing

Alive: doing, token age, owner death, pageins, and disk are on the
same frame as the pipeline.  
Most surprising: ninety live owner samples were one `stored` lifecycle
away from the glass.  
Discomfort into gold: 0-empty looked like a measured quiet and was
the monitor's own unused Metal handles covering a dead owner.
