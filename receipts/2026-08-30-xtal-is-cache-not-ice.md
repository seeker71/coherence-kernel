# 2026-08-30 — xtal leaves git; BML stays ice

Urs: dissolve all xtal; place the missing stone; dissolve the block.

Committed xtal was a second ice. The walker still reads Form, so a
derived image remains — as **cache**, the way `.fkb` already is.

The stone: `form-cli-bml-cache`. Ice is a root `.bml` or a lift `.fk`
named by a `*-compile.fk` companion. `cache-fresh?` by mtime. Miss
calls `form-source-compile-file`. Hit is silent. Git forgets `*-xtal.fk`.

The block that C only preludes `.fk` stays; we did not grow the seed.
form-cli's shell asks the cache before opening the repl. A fresh
checkout runs the two cache cells beside binary-freshness.

```
form-cli-bml-cache-band.fk                                799
form-cli-author-high-band.fk still 1023 from local cache
pictures=38
```

42 xtals untracked. They can vanish; ice rebuilds them.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-30 -> bml-cache 799; gitignore *-xtal.fk; 42 removed from git
