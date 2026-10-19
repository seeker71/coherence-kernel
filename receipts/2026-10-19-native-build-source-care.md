# Native private-build source ownership

The deploy image is a source snapshot, not a Git worktree. The failed native
build in Coherence-Network run 39235284694 entered `fcb-private-run`, where
`git ls-files --cached --others --exclude-standard` refused the snapshot's
unresolvable `.git` pointer. Its empty result was treated as the source set;
the private build then lacked `gate/form-cli-build-run.bml`.

The flow now imports the existing `source-tree.bml` and reads `tracked-files()`
at that same boundary. This retires the external Git process, not the source
identity, closure, attestation or publication care. No new host-language path,
runtime change, Docker-specific bypass or bootstrap-kernel validation is added.

A native source snapshot selected and copied 2248 files. The observation retains
an intentionally unusable `.git` pointer to exercise the actual arrival shape.
The first remote command was started before transfer completed and failed with
`cc1: fatal error: runtime/fkwu-uni.c: No such file or directory`; this is a
transfer-order failure, not successful source/build evidence. Build observation
continues only after the source transfer completes.

The completed snapshot passed the repaired source-copy boundary and entered
native recipe compilation. That rehearsal used `--shm-size=1g`, unlike the
deployment's tmpfs mount; compilation then reported `SIGBUS` (rc 135) in
`bml-floor-compile.fk`, with resident_bytes 1291735040. It is not a build pass.
The resumed command uses the same `/dev/shm` tmpfs shape as the deploy build,
removing the rehearsal-only 1 GiB restriction. No source/compiler workaround
or additional kernel change is inferred from this resource mismatch.

The deployment-matching tmpfs run passed (exit 0). It regenerated and accepted
the sealed native recipe and `form-cli-linux-amd64` bundle, then published the
executable at `/build/form-cli`; source identity `6d3c117149c6ef13`. The command
was `./fkwu gate/form-cli-build-run.bml && test -x /build/form-cli`, with the
original explicit-out/force-link/cc request. Both the unusable Git pointer and
the normal source seals remained in place. The earlier SIGBUS therefore stays
with the rehearsal-only resource restriction, not a compiler workaround.
This was one affected build path, not a kernel or sibling validation suite.

OpenAI supplied reasoning (exact runtime model/task tokens unavailable); Form
selected and copied the source set. The local form-cli lookup executable was
absent; native source lookup, not a fabricated router hit, established the path.
The useful teaching is that the source reader already existed: the obsolete
external enumeration, not missing filesystem capability, kept the image out.
