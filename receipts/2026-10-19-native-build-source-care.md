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

OpenAI supplied reasoning (exact runtime model/task tokens unavailable); Form
selected and copied the source set. The local form-cli lookup executable was
absent; native source lookup, not a fabricated router hit, established the path.
The useful teaching is that the source reader already existed: the obsolete
external enumeration, not missing filesystem capability, kept the image out.
