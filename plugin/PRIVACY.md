# Privacy Policy — Sema (coherence-kernel)

**Effective: 2026-07-05 · Contact: umuff71@gmail.com**

Sema is a question-answering service at `https://sema.hati.earth` (also
`https://hati.earth/sema`). It answers questions by pointing to cells of the public
[coherence-kernel](https://github.com/seeker71/coherence-kernel) GitHub repository,
with a tone reading of the question and links for verifying every answer. This
policy describes what the service receives and what happens to it. It is written
to match what the serving code actually does — the code is public
([`plugin/chatgpt-plugin.fk`](chatgpt-plugin.fk)), so every claim here is checkable.

## What we receive

- **Your question text** (the `q` parameter of `/ask`) or a **repository path**
  (the `path` parameter of `/trace`). When you use Sema through ChatGPT, these
  arrive from OpenAI's servers, not directly from your device.
- Standard connection data inherent to any HTTP request (such as the caller's IP
  address) reaches the transit infrastructure described below.

We receive **no** account information, no names or email addresses, no cookies, no
authentication tokens (the API requires none), and no request bodies (all
endpoints are GET).

## What we do with it

Your question is processed **in memory only**: matched against a lexical index of
the public repository, given a tone reading, assembled into a JSON response, and
released. The serving application **writes nothing to disk and keeps no record of
your questions** — it contains no logging, storage, or analytics code of any kind.
There are no user profiles, no tracking, no advertising, and nothing is sold or
shared with anyone.

## Infrastructure

Requests pass through **Cloudflare** (TLS proxy) to a server operated on
**Hostinger**. These providers may process transient connection metadata (such as
IP addresses) under their own privacy policies, as is true for essentially any
website; we do not access, use, or retain that metadata. The reverse proxy on our
server keeps no access log.

## Third parties

- Answers contain links to **github.com** (the public repository, its commit
  history, and line attribution). Following those links is subject to GitHub's
  privacy policy.
- Your conversation with ChatGPT itself is handled by **OpenAI** under OpenAI's
  privacy policy; Sema only receives the single question string the Action sends.

## Children

The service is not directed at children and collects no personal information from
anyone.

## Changes

Any change to this policy lands as a dated commit in the public repository, so its
full history remains inspectable — the same traceability the service offers for
every answer.
