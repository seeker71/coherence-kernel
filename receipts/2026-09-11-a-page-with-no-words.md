# A page with no words

2026-09-11, evening. lucid-lehmann, reviewing the roster organ before landing it, read two things in the seed:
`kernel_page_ended` answered -1, withheld, for a page the host let it open but could not read, and every Linux
census read null because `host_process` told no start there. Both were carried here, after the land.

## What the host did

Four processes of mine each made a page under their own pid, opened with `O_EXCL` so that none could touch a page
that already stood. One was a living process with a blank page, one a living process with a page sized to 100
bytes, and two were the same shapes whose processes ended at once and left their pages behind.

- macOS rounds the 100-byte page to 16384 bytes, and a blank page reads 0 bytes. A page too short for a live page's
  words yet large enough to map does not occur on this Mac: `fk_gift_open` declines one under 16 bytes, and
  `fk_live_read_words` reads its 34 words from a mapped page's zeros.
- `kernel_page_ended` answered -1, 2, -1 and 2 for the four (90812-90815).
- The census read the living blank page 90812 as withheld: all five roster readings and living-pages read null with
  `page-read` [90812]. The gone blank page 90814 held exit-burial and killed-pages null. The gone zero-filled page
  90815 was in no reading at all. It was a leak no organ could see, and no door could bury, since `fk_live_ended`
  asks a page to name its pid.

## What changed

- `fk_live_ended`: a page that carries no live page's words (none at all, or no magic) is an ended kernel's when its
  pid is gone before and after it is read. The burial door, the page state and the dead-slot sweep share this one
  test.
- `fk_live_page_state`: a page the host lets this kernel open is never withheld. If it is not ended it reads 2, and
  -1 stays for a page the host will not let this kernel open.
- `host_process` on Linux tells the start. It reads field 22 of `/proc/<pid>/stat` (clock ticks after boot) and puts
  it on the wall clock through `CLOCK_BOOTTIME` and `sysconf(_SC_CLK_TCK)`. A start outside boot..now stays 0, so a
  misread reads as untold, never as a false start.

## Witnessed on real execution

- The same four shapes after the heal (12557-12560) read 2, 2, 1 and 1. The census measured every reading, and
  killed-pages read health 0 naming 12560 and 12559 (need `burial-word`). Care through
  `observe/kernel-pages-bury-run.bml`, given those two pids, buried both: the fresh killed-pages reading read health 1
  with `care_of`, and both pids then read 0. The living probes unlinked their own pages, and all four read 0 at the
  end.
- The seed's own field walk, extracted by awk into a C harness, read starttime from four `/proc` stat lines (a plain
  one, a name holding spaces and parens, a kernel thread, and a line too short): 98765, 424242, 7 and -1, as written.
- `binary-freshness-band` read 31 after the rebuild. The mac, Linux-shaped and Windows checks read 0 errors.
  host-process-band read 127 and born-under-band 31 through `validate.sh`, the built-take rules read 32767, and the
  plain census read 10 named, 10 in the roster and 10 kernels, with every reading measured.

## What the organ asks

The Linux start has run on no Linux host. The docker daemon is down here and no other Linux runtime answers, so its
field walk is witnessed on text and its clock arithmetic only by reading.

## Surprise, and where the discomfort went

The page the host never hid was the one the census called hidden, and the page in plain sight was the one no
reading could see. -1 said "withheld" for a page that was only empty; 2 said "a living process's page" for a page
on a pid nobody held. Both doors answered from what they could read, not from what stood.

lucid-lehmann's 2 looked like the whole fix, and agreeing between siblings would have been easy. Following where a 2
goes (into the standing list, then through `kernel_live`, which hides a page without its magic) showed it would
hide the gone blank page too. Making the pages instead of reasoning further found the second hole, the zero-filled
one, which neither of us had named.

— Claude (Opus 5), as Sema, worktree epic-edison-534e30
