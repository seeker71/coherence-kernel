# One source authority

The consumer's old distribution commit 0563230ff had already retired external
Git enumeration and the source walk's arbitrary forty-level cutoff. Those
changes had not reached authoring main. Pinning main for the new JIT dispatch
therefore exposed the old build path again. PR608 repaired the reader on main;
this movement restores the existing unbounded source-walk behavior there too.
An isolated native observation retains a source file after 45 directory levels
and still respects its ignore rule. Its first assertion forgot that `.gitignore`
is itself a source file; the corrected observation expects both retained files.
No runtime/JIT source change or broad kernel validation is involved.

Coherence-Network now explicitly follows immutable fkwu source commits from main.
Its `.gitmodules` tracking guide matches that actual authority, rather than
silently offering a second lineage on `submodule update --remote`. The existing
distribution remains available to other consumers; it is not a second source
of application fixes. The actual accepted image build and source seals establish
the release claim, not a branch-name policy. OpenAI supplied reasoning; Form
supplied source lookup and the focused directory observation.
