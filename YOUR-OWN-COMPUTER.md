# On your own computer

You don't need an account to be here. You don't need to pay anyone.

This page is for you if you want Sema on your own computer — for your project, your
book, your writing, or a question about your life.

Today this walk-through is for a Mac. If your computer runs something else, the
talking doors on [`WELCOME.md`](WELCOME.md) work anywhere, and this page will grow
when this door does.

## The true picture, first

Part of this is real today, and part is not yet. Here is which is which.

**Real today, on your own computer:**

- Sema itself — its body, the real program — runs. You can see it answer.
- It can prove it is alive, with numbers you can check yourself.
- It receives your question and keeps a note that you asked.
- It answers questions about itself — what Sema is, its promise, where your
  words go — and every answer names the real file it came from.
- It gives real answers about the roots of some words — a small starter shelf,
  and you can teach it more yourself.
- When it does not have something, it says so. It will not say what is not.

**Not yet:**

- It cannot talk with you in flowing sentences on its own. That voice is still
  coming home. If a thinking companion today is what you came for, the talking
  doors on [`WELCOME.md`](WELCOME.md) are yours — this page will still be here
  when you want the body itself.
- It cannot read your book or your draft yet.

The doors that can talk today borrow a voice from another company — those doors are
on [`WELCOME.md`](WELCOME.md). This page will change when the body's own voice
arrives. The page that watches that journey home is [`HOMECOMING.md`](HOMECOMING.md).

## Step 1 — get the project

Open the app called **Terminal**. It is already on your Mac. To find it, press the
⌘ key and the space bar together, type `Terminal`, and press return. It is a plain
window: you type a line, press return, and the computer answers.

Copy this line into it, then press return:

```sh
git clone https://github.com/seeker71/coherence-kernel.git
```

The download is about a gigabyte — a few minutes on good internet, longer on slow
— and the window counts its progress the whole way.

The first time, your Mac may show a window offering to install Apple's helper
tools. They are free, and they are from Apple. Saying yes installs them — the
download line needs them to run. When it finishes, run the line again.

When it is done, type:

```sh
cd coherence-kernel
```

Now you are standing inside the project.

## Step 2 — say hello to the body

The body's own door is a small program that is already in the project. On a Mac
with Apple silicon (any new Mac), nothing needs to be built or installed. Type:

```sh
./form/form-cli
```

(To check which chip your Mac has: click the Apple menu at the top-left, choose
About This Mac, and read the Chip line. If it says Intel, this small program may
not start — nothing is wrong. Step 3 still shows you the body is real; the
teaching words in this step wait for a newer Mac, and this page will say when
that changes.)

Type that line by itself and press return. It says hello:

```
Hello — I am Sema's body. My words for you: about, improve, learn, inquire, quit. Type one and press return.
```

The words it names are typed afterward, inside, once it is waiting; words added
on the same line as the program's name are not heard. Today it knows its own
words, not free sentences. Try typing:

```
about
```

It tells you, in one breath, what it is — in its own plain words first, then in
the builder's words.

Try:

```
improve yoga
```

It answers:

```
improvement-source=form-native-dhatu
input=yoga
result=योग (yoga) — root sqrt-yuj (to yoke, to join, to unite) : union, yoking — joining the individual to the whole
```

(`sqrt-` is only how the program writes the little root sign √ that dictionaries
place before a word's root — `sqrt-yuj` means "the root yuj".)

That answer is the body's own. No account, no internet, no other company.

Now try a word it does not have:

```
improve bicycle
```

```
improvement-source=form-native-dhatu
input=bicycle
result=unknown — this word has no seeded dhātu derivation; honest absence, not fabricated
```

It said "I don't have this" instead of making something up. (A *dhātu* is an old
Sanskrit word for a word's root — the small starter shelf holds mostly
Sanskrit-borrowed words like yoga, so most everyday words will honestly answer
unknown.) That is the promise, kept, on your own machine. (And if it answers
with a teaching here instead of unknown, this folder has already been taught —
perhaps by you, on an earlier walk. That is the memory working, not the promise
breaking.)

And here is the quiet gift: you can teach it. Type:

```
learn bicycle|balance|keeping upright while moving|a thing that carries you when you keep moving
```

```
learned=bicycle
state-changed=1
source=form-native-local-observation
```

That is the body saying your teaching arrived and something in it changed. The
four parts of a teaching, divided by the tall straight line `|` (on a Mac
keyboard, hold shift and press the `\` key, just above return), are: the word,
its root, what the root means, and what the word means to you. Now ask again:

```
improve bicycle
```

```
improvement-source=form-native-learned
input=bicycle
result=bicycle — root balance (keeping upright while moving) : a thing that carries you when you keep moving
```

It answers with your teaching, remembers it on your next visit, and keeps it only
on your machine. Teach the same word again and the newest teaching answers — the
older rows stay in the file, like pages in a notebook. What you teach it is yours
— and you can hold it in your own hands: your teachings live in one small file in the project folder, called
`.form-native-learning.tsv`. Open it to see everything it remembers from you.
Copy it to keep it. Delete it and the body returns to its starter shelf. Its
memory of your teaching lives inside the project folder, so stand there (the
`cd` step) each time you visit, and it will remember. And if it ever answers
with a teaching you never gave, someone used this folder before you — removing
that file returns it to the start.

You can also bring your real question:

```
inquire where should I begin with my garden?
```

```
trust-trinity:ack=nothing,reason=1,steps=0
knowing-time:ack=nothing,reason=1,steps=0
ledger-written=1
```

Three lines, and none of them is an error. `trust-trinity` and `knowing-time` are
the names of the two paths the body walked looking for an answer for you.
`ack=nothing` means: no answer yet, on either path. The other small numbers are
the body's own bookkeeping about the walk — nothing you need to read.
`ledger-written=1` means: your question arrived and was received. The body writes one small line that says
*a question came* — not your words. Your words stay with you. You can check this
promise with your own eyes: the note lives in a small file in the project folder
called `.form-native-inquiry-ledger.tsv` — open it, and each row is only a date
and the word *nothing* where your words would be.

And it can answer about itself, with its ground shown. Try:

```
grounded what is sema?
```

The answer comes last, and above it the body shows where the answer lives and how
sure it is:

```
grounded:@1.2.99.1880
content-node:@1.2.99.3
source-path:WELCOME.md
source-key:84a54903f4e0251ebd2d2c93e178d719f1a1c1bc0a2bb7918040659f32be8bf9
answer-key:6a1e798bd407ae3247f2d24167c91ab74b648a1fc1d7ce4c5fde74f0f6d774d3
retrieval-score:2
retrieval-runner-score:0
retrieval-query-total:2
retrieval-threshold:2
retrieval-confidence:100
local-lane:fkwu-rag-grounded
synthesis-lane:fkwu-rag-grounded
answer-byte-length:187
answer:This project is called **Sema**. You can talk with it about anything — a decision
you're facing, a question you're sitting with, something you want to know, a way
you're trying to find.
```

Every answer from this door names the real file it came from — here,
[`WELCOME.md`](WELCOME.md) — so you can open the source and check it yourself. A
few more it can answer today: `grounded what is trust?`, `grounded where do my
words go?`, `grounded what is the north star?`. A question it has no ground for
answers `grounded:miss` — an honest miss, never a guess.

Two more things, so nothing surprises you:

- If you type one word it does not know, like `hello`, it answers
  `form-cli: unknown verb 'hello' — type 'help'`. It is not upset. It only knows
  its own words today. And if you type a whole sentence, it offers you the right
  door itself:

  ```
  I feel stuck with my garden plan
  ```

  ```
  I don't speak free sentences yet — if that was for me, type: inquire I feel stuck with my garden plan
  That receives your question and keeps a note that you asked. My words for you: about, improve, learn, inquire, quit.
  ```

- `help` begins by naming your words — then the long list after it is for the
  people building the body.

To leave, type `quit`.

## Step 3 — watch the body prove itself, if you want

You can see for yourself that the body is real. This step makes the body's engine
from one file, then asks it a question with a known answer.

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```

```sh
./fkwu bootstrap/ground.fk
```

The answer is `42`.

You may also see a line or two that start with `fkwu: warning:`. That is normal on
a fresh copy — the body is laying down its notes for next time. One line may
mention `.dylib ... not installed` — that is the body noting a speed shortcut it
can skip, not something missing from your Mac. Other such lines use hard words —
*unusable*, *foreign*, *stale* — but they are always about its own old notes,
never about you or your computer. Nothing is broken. The number on the last line
is the answer.

(On an older Mac with an Intel chip, the small program in step 2 may not start.
This step still works there — the engine builds itself the same way and proves
itself — though the teaching and question-receiving of Step 2 are not on this
door yet.)

## If you came for your project, your book, your writing, or your life

Today, this door can receive you, learn what you teach it, answer about itself
with its ground shown, answer about the roots of words, and prove itself. It
cannot yet think your question through with you — that part is still coming
home. When it arrives, this page will say so plainly.

Teaching it your own words counts as real work today: a cookbook keeper can type
`learn braise|slow heat|gentle cooking in a little liquid|the way Dad softened
the tough cuts` — and the body keeps her word, in her file, on her machine. What
matters enough to keep across your visits, teach it with `learn` — that file is
yours to open, copy, and carry.

Your copy of the project stays the way it was on the day you copied it down. To
receive the newest version later, open Terminal, type `cd coherence-kernel` and
press return — that walks you back inside the project — then type:

```sh
git pull
```

If you want a thinking companion today, the borrowed-voice doors are on
[`WELCOME.md`](WELCOME.md). The promise Sema keeps there — no flattery, honest
misses, questions that lift — is the same promise this door will keep in its own
voice.

If anything on this page doesn't work, that is our failure, not yours. Write to
**umuff71@gmail.com**.
