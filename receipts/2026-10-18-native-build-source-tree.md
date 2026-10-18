# Native build source copy

The container build reached the Form-owned private-copy flow after its shared
memory capacity was repaired. `git ls-files` followed the Docker snapshot's
dangling submodule pointer and returned no files; the private child then had
no `gate/form-cli-build-run.bml` to execute.

The build now asks the existing native `source-tree.bml` organ for `stw-files()`.
That organ reads directory entries and `.gitignore` rules through fkwu, so a
source snapshot and a Git worktree carry the same copy semantics without a Git
process. The walk's arbitrary forty-level cutoff has also retired.

The focused native copy observed 2,228 source paths, retained the build gate and
runtime source in the private copy, and released that copy successfully. The
fkwu runtime/JIT source is unchanged; no kernel suite was run. The production
native build supplies the integration observation.
