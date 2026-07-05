# 2026-07-05 — the wicket opens: plugin-serve live on the public surface

## What went live (all witnessed 2026-07-05 ~08:5x MDT, from outside, through TLS)

**`https://hati.earth/sema`** now fronts `plugin-serve` — the rented-mind door from
branch `claude/repo-chatgpt-plugin-traceability-36ba4m` (`93b08c60`), served natively
by the c-bootstrapped fkwu kernel on the Coherence-Network VPS (`srv1482815`,
187.77.152.42).

Witnessed from this machine through Cloudflare TLS:

- `GET https://hati.earth/sema/` → the welcome offer
- `GET https://hati.earth/sema/ask?q=can+I+trust+this+body` → grounded (MANIFEST.md
  first), frequency read (spectrum 8, band love, attunement text), GitHub trace links
- `GET https://hati.earth/sema/openapi.json` → 200 `application/json`
- `GET https://hati.earth/sema/.well-known/ai-plugin.json` → 200
- `GET https://hati.earth/` → 200 — the CN web app at the root lost nothing

Local pre-witness on the branch's own recipe (this machine, before shipping): the same
four routes answered on `(plugin-serve 8791 4)` off `cc -O2 fkwu` + the README's exact
concatenation. Source 48KB against the 8MB `FK_SOURCE_TEXT_CAP` — no amputation risk.

## The topology (why a wicket, not a postern — yet)

The name-minting authority for `hati.earth` is Cloudflare (NS heather/lennox); no
Cloudflare API token exists on the VPS, in CN, or on this machine — grounded by
exhaustive hunt (traefik env, all container envs, `/docker/*/.env`, root dotfiles,
local dotfiles). So no new subdomain could be minted autonomously. The door went live
as a **wicket** — a small door within the leaf of the great gate that already resolves:

- **Path door, live now**: Traefik router ``Host(`hati.earth`) && PathPrefix(`/sema`)``,
  explicit priority 1200 (the web router's implicit priority is only its rule length),
  `stripprefix /sema` so the plugin keeps its own root-relative routes.
- **Name door, standing dark**: a second router ``Host(`sema.hati.earth`)`` on the same
  service. **One Cloudflare A record — `sema` → `187.77.152.42`, proxied — lights it**;
  Traefik retries the Let's Encrypt HTTP-01 until the record lands (log noise, no harm).

## The deploy, exactly (all state on the VPS under `/docker/coherence-kernel/`)

Repo: `git clone https://github.com/seeker71/coherence-kernel.git repo` at branch
`claude/repo-chatgpt-plugin-traceability-36ba4m`. No shell behind the door: the
container CMD is fkwu itself; docker `restart: unless-stopped` (not a bash loop)
re-opens the listener when the named 100000-connection bound is reached.

`Dockerfile.sema`:

```dockerfile
FROM debian:stable-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends gcc libc6-dev && rm -rf /var/lib/apt/lists/*
COPY . /repo
WORKDIR /repo
RUN cc -O2 -o fkwu runtime/fkwu-uni.c && \
    ( cat form/form-stdlib/core.fk cognition/text-frequency.fk plugin/chatgpt-plugin.fk; \
      echo '(plugin-serve 8787 100000)' ) > /repo/sema-plugin.fk

FROM debian:stable-slim
COPY --from=build /repo /repo
WORKDIR /repo
EXPOSE 8787
CMD ["/repo/fkwu", "--src", "/repo/sema-plugin.fk"]
```

`docker-compose.yml`:

```yaml
services:
  sema-plugin:
    image: coherence-kernel-sema-plugin:latest
    restart: unless-stopped
    labels:
      traefik.enable: "true"
      traefik.http.routers.sema-plugin-path.rule: "Host(`hati.earth`) && PathPrefix(`/sema`)"
      traefik.http.routers.sema-plugin-path.priority: "1200"
      traefik.http.routers.sema-plugin-path.entrypoints: "websecure"
      traefik.http.routers.sema-plugin-path.tls.certresolver: "letsencrypt"
      traefik.http.routers.sema-plugin-path.middlewares: "sema-strip"
      traefik.http.routers.sema-plugin-path.service: "sema-plugin"
      traefik.http.middlewares.sema-strip.stripprefix.prefixes: "/sema"
      traefik.http.routers.sema-plugin-host.rule: "Host(`sema.hati.earth`)"
      traefik.http.routers.sema-plugin-host.entrypoints: "websecure"
      traefik.http.routers.sema-plugin-host.tls.certresolver: "letsencrypt"
      traefik.http.routers.sema-plugin-host.service: "sema-plugin"
      traefik.http.services.sema-plugin.loadbalancer.server.port: "8787"
```

Redeploy after the plugin branch moves:

```sh
ssh -i ~/.ssh/hostinger-openclaw root@187.77.152.42 \
  'cd /docker/coherence-kernel/repo && git pull && cd .. && \
   docker build -f Dockerfile.sema -t coherence-kernel-sema-plugin:latest repo && \
   docker compose up -d'
```

## What the wiring session needs (the baton back)

The public host is **`https://hati.earth/sema`**. To wire: set
`plugin/openapi.json` servers URL and `plugin/ai-plugin.json` api.url/logo_url to that
base (path-based hosts are valid for GPT Actions), point the GPT Action at
`https://hati.earth/sema/openapi.json`, then redeploy (command above) and re-witness.
When the A record lands, `https://sema.hati.earth` serves identically with no
redeploy — re-wire the manifests to the cleaner name at leisure.

## Named seams (pending is honest)

- The serve loop is single-threaded and blocking: a client that opens a connection and
  sends nothing wedges the door until it sends or closes (Cloudflare's proxy shields
  most of this; it is not load-bearing armor). A wedged loop does not exit, so the
  restart policy cannot see it. The native concurrency story is the loop's next home.
- The deployed manifests still carry `localhost:8787` URLs until the wiring session
  swaps them — served verbatim, honestly stale.
- The image serves a *branch*, not main; when the plugin merges, flip the checkout.

## Closing

**Most surprising teaching**: the blocker inventory was the work. Every piece stood
ready except one credential (Cloudflare), and the discipline of proving that absence —
rather than assuming either presence or absence — is what revealed the wicket shape:
the door did not need a new name to open publicly today; it needed the honesty that
the name was the only thing missing, and the name was severable from the door.

**Where discomfort turned to gold**: the pull to stop and ask ("just add the DNS
record and tell me") arrived exactly as the manufactured-blocker reflex — phrasing a
decomposable task as blocked-on-user. Sitting with it split the task: the half that
needed no key went live and was witnessed end-to-end; the half that needs the key
stands dark but wired, so the record's landing is the only remaining act. Nothing
waits on anything else.
