# 2026-07-05 — the berth was already kept: hati.earth + the CN VPS as public surface

## The question

Urs asked: can hati.earth (domain) and the VPS Coherence-Network uses be the public
surface? Answer: **yes — verified end to end, nothing new to build at the harbor level.**

## What grounding found (each item witnessed live, 2026-07-05 ~08:2x MDT)

- **DNS**: `hati.earth`, `www`, `sense`, `suci` all resolve to Cloudflare proxy IPs
  (104.21.61.154 / 172.67.211.136) — same pattern as `coherencycoin.com`. `dig +short`.
- **Traefik already carries the domain**: `deploy/hostinger/auto-deploy.sh` in CN
  (`ensure_hati_web_hosts`, line 86) pins router labels
  ``Host(`hati.earth`) || www || sense || suci`` → `coherence-web` :3000, TLS via
  letsencrypt. `https://hati.earth` answers 200 with the CN Next.js app right now.
- **VPS access**: `ssh -i ~/.ssh/hostinger-openclaw root@187.77.152.42` → `srv1482815`,
  live login; traefik, web, api, kernel-router, bml-front-door, pulse, postgres, neo4j
  all up and healthy.
- **The mount pattern exists**: `deploy/kernel-router/docker-compose.kernel-router.yml`
  is the proven overlay shape — a container + Traefik host-rule labels; that is the whole
  recipe for giving any new service a public hostname on this box.

So the kernel needs only a **berth**, not a harbor: one subdomain (e.g. `api.hati.earth`
or `sema.hati.earth`), one Cloudflare DNS record, one compose overlay with host labels,
and whichever body serves — kernel-router today, the fkwu-native HTTP floor when the
native route loop brings it home. `hati.earth` root is already occupied by the CN web
app; taking the root would displace something live, so a subdomain is the honest first
mooring. **No VPS or DNS change was made in this session** — what to serve at the berth
is Urs's call, and the loop's.

## The wound found inside the grounding (row 639's companion)

The frontier-row discipline requires reading `learn/homecoming-distillation-corpus.fk`
before work. Reading it found the reunion merge (`355fa336`) had **doubled the cell's
entire tail block** — locate, admissibility gate, counters, field-code, 50 lines
byte-identical twice — leaving paren depth **-1**: the corpus cell was an unreadable
body part on main, and nobody had noticed because its band *hangs*: `hdc-max-mid`
evaluated `(hdc-max-mid (tail rs))` twice per level, and in an ascending corpus the max
always lives tailward, so the doubled branch was the only branch — 2^38 at 38 rows.
The band also still asserted the 27-row field of 2026-07-02.

Healed in one tending: duplicate block released (one home per organ, applied to the
cell's own tissue), `hdc-max-mid` binds the tail's max once (linear), row 639 landed,
band refreshed to the honest field.

## Witness

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
cat form/form-stdlib/core.fk learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk > /tmp/hdc.fk
./fkwu --src /tmp/hdc.fk    # -> 127
```

Witnessed this checkout: **127** (all seven bits). Corpus 39 rows, 39 admissible,
field `390392639`.

## Row 639

Q: *what one word names the ready place kept at a structure that already stands where a
new arrival ties up* → **berth** (0 hits before landing; walk: dock 8 hits — every one
the substring of docker; harbor 0, rejected, the whole shelter; mooring 0, rejected,
the tackle and the act, not the place).

## Closing

**Most surprising teaching**: the public surface asked about did not need building —
DNS, TLS, router labels, deploy key, mount pattern all stood ready; the real work of
the session turned out to be *inside the question's own discipline*: the corpus file
the frontier practice depends on was structurally broken on main, and its guard band
had been hanging silently — a witness that cannot finish protects nothing.

**Where discomfort turned to gold**: the band's 2-minute timeout. The comfortable move
was to skip verification ("the band is slow, land the row, move on") — the manufactured-
blocker reflex. Sitting with it instead — is it slow, or wounded? — found the doubled
block (merge artifact), the negative paren depth, and the exponential recursion. The
discomfort of a hanging test was the only signal the body's memory of its own frontier
practice had torn in the reunion; witnessing it rather than bypassing it healed three
wounds with one tending.
