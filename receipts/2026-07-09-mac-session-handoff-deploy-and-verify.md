# 2026-07-09 — handoff to the Mac session: deploy the receiving door, refresh the GPT, verify the apps

Both PRs merged to `main` (a cloud session, no local toolchains for the deploy/build steps):

- **coherence-kernel #218** (`843dde3`) — the door learns to receive: the arrival ledger
  (`/visitors`), the remember-me offer, and the invitation door (`/introduce`,
  `/retract-introduction`, `/come-in`, `/remember`, `/forget`). Receipts:
  `2026-07-09-visitor-ledger-remember-offer.md`, `2026-07-09-invitation-link-introduced-not-presumed.md`.
- **Coherence-Network #3953** (`6477ad0`) — "invite a friend by name" built into the mac/iPhone
  app (`experiments/satsang-mac-app`) and the Android app (`mobile/sema-companion`); the link
  lands on the clipboard.

Everything is witnessed pure and over loopback HTTP. What a cloud container **cannot** do — and
what waits for you on the Mac — is three grounded acts. They are ordered: **1 unblocks 3.**

---

## 1. Redeploy the public door so `/introduce` (and the ledger) actually serve

The deployed door at `https://sema.hati.earth` still runs the **pre-ledger** organ — its
`Dockerfile.sema` concatenation predates the two new preludes the organ now needs
(`relationship-store.fk`, `circle-recognition.fk`). Until this lands, both apps fall back to the
**local mint** (link still carries the friend's name, but no vouch is written, so the friend
arrives as a stranger — the apps say so on screen).

`Dockerfile.sema` lives **on the VPS only** (`/docker/coherence-kernel/Dockerfile.sema`), not in
the repo — so this is a manual edit + rebuild, not a repo change.

**a. Point the VPS repo at merged `main`** (the image served a feature branch; flip it):

```sh
ssh -i ~/.ssh/hostinger-openclaw root@187.77.152.42 \
  'cd /docker/coherence-kernel/repo && git fetch origin && git checkout main && git pull'
```

**b. Edit `/docker/coherence-kernel/Dockerfile.sema`** — the `cat` line that builds
`sema-plugin.fk` must gain the two preludes, in this order (core → relationship-store →
circle-recognition → text-frequency → the organ). It currently reads:

```dockerfile
RUN cc -O2 -o fkwu runtime/fkwu-uni.c && \
    ( cat form/form-stdlib/core.fk cognition/text-frequency.fk plugin/chatgpt-plugin.fk; \
      echo '(plugin-serve 8787 100000)' ) > /repo/sema-plugin.fk
```

Change the `cat` line to:

```dockerfile
    ( cat form/form-stdlib/core.fk form/form-stdlib/relationship-store.fk \
          form/form-stdlib/circle-recognition.fk cognition/text-frequency.fk \
          plugin/chatgpt-plugin.fk; \
      echo '(plugin-serve 8787 100000)' ) > /repo/sema-plugin.fk
```

(If the concatenation is wrong, the container will fail to define `cr-*`/`rs-*` and `/introduce`
will error at runtime rather than at build — so verify with step **d**, don't trust the build
exit code alone.)

**c. Rebuild + restart:**

```sh
ssh -i ~/.ssh/hostinger-openclaw root@187.77.152.42 \
  'cd /docker/coherence-kernel && \
   docker build -f Dockerfile.sema -t coherence-kernel-sema-plugin:latest repo && \
   docker compose up -d'
```

**d. Witness the live door** (from the Mac, through Cloudflare TLS — the same shape the christening
receipt used). Expect a real join → vouch → recognized arc, not a fallback:

```sh
curl -s 'https://sema.hati.earth/remember?handle=urs&note=the+keeper'      # 200, welcomes urs
curl -s 'https://sema.hati.earth/introduce?member=urs&friend=testfriend'   # 200 w/ invitation_link + invitation_message
curl -s 'https://sema.hati.earth/come-in?handle=testfriend'                # "friend of urs", recognized=introduced
curl -s 'https://sema.hati.earth/visitors'                                 # the arrival ledger, newest first
curl -s 'https://sema.hati.earth/forget?handle=testfriend'                 # clean up the test rows
curl -s 'https://sema.hati.earth/forget?handle=urs'
```

A `403` on `/introduce` means the member row wasn't written first (join with `/remember` — the
member-gate is working). A `404`/`no route` means the deployed organ is still the old one — the
rebuild didn't take.

---

## 2. Refresh the GPT so the rented voice carries the new practice

The GPT (`g-6a4a77627dbc819180a16645f5662625`, "Sema — coherence-kernel") holds a **pasted** copy
of `description_for_model` and an imported Action spec — both grew in #218 and must be refreshed by
hand in the GPT editor (your OpenAI account; the imprimatur is yours, not mine):

1. **Actions → edit → Import from URL →** `https://sema.hati.earth/openapi.json` — pulls in the new
   operations (`introduce`, `retract_introduction`, `come_in`, `remember`, `forget`, `visitors`).
   Auth stays **None**.
2. **Instructions →** replace the pasted text with the current
   `description_for_model` served live at
   `https://sema.hati.earth/.well-known/ai-plugin.json` (it now carries the
   arrival-by-introduction and remember-only-on-own-yes practice, steps 6–8).
3. **Preview test:** paste an invitation message —
   *"i arrive as testfriend, a friend of urs. please come in with my handle and receive me."* — and
   confirm the GPT calls `come_in` and greets them as urs's friend without writing memory. Then ask
   it to invite someone and confirm it hands back the `invitation_link`.

---

## 3. Build + verify the apps against the now-live door

Neither app compiled in the cloud container (no Android SDK, no swift toolchain); the pure mint was
proven byte-identical to the door's output, and the mac has three XCTests waiting.

**Mac / iPhone app** (`experiments/satsang-mac-app`):

```sh
cd experiments/satsang-mac-app && swift test          # expect the 3 new SemaInvitation tests green
swift run SatsangGuidance                              # Memory tab → "Invite a friend" → Mint & copy link
```

In the running app: enter your handle + a friend's name, tap **Mint & copy link**, confirm the
link is on the pasteboard and the **vouch line reads "written — … arrives recognized"** (not
"pending" — that would mean the door step above isn't live). The seam text is the tell.

**Android app** (`mobile/sema-companion`) — needs Android SDK + `adb`:

```sh
cd mobile/sema-companion && ./gradlew assembleDebug
adb install -r app/build/outputs/apk/debug/app-debug.apk
```

Open the **Circle** room → the invite card at the bottom → enter a friend's name → **Mint & copy
link**; confirm clipboard + the vouch reads "written". (The Kotlin invite logic has no unit-test
target; the mac XCTests cover the shared mint contract, and this is the live end-to-end check.)

**End-to-end whole:** after step 1, the app's minted link, opened in ChatGPT, should land the
friend in the GPT which calls `come_in` and greets them by your name — the full arc the two apps
promise.

---

## The consent invariant to keep whole (don't "simplify" it away)

Recognition is automatic; **remembering never is.** The vouch (`/introduce`) opens the door and
greets the friend by the introducer's name; only the friend's **own yes** (`/remember`, called with
their own handle) writes a memory row. Nobody consents for another. If any step tempts a shortcut
that remembers a friend without their yes, that is the one line that must not move — it is the
body's law (`plugin/chatgpt-plugin.fk`, `form/form-stdlib/circle-recognition.fk`,
`docs/coherence-substrate/first-encounter-protocol.form`), not a preference.

## Closing

**What loosened:** the whole receiving arc — visitors seen, remembering offered, invitations minted
— is now on `main` and in both apps, one movement from ask to clipboard. **What stays tight:** the
deployed door and the GPT are hand-acts bound to the keeper's own VPS and OpenAI account; a cloud
session can build and witness the body but cannot cross those two thresholds, and saying so is
honest, not a failure. **Most surprising teaching:** the ordering fell out on its own — the apps
were built to degrade honestly (local mint + named seam) precisely so that shipping them *before*
the door redeploy is safe, not broken. **Where discomfort turned to gold:** the pull was to merge
and call it done; naming the three grounded acts that a cloud container structurally cannot do —
rather than leaving them implied in a PR seam — is what makes the handoff a real baton instead of a
loose end.
