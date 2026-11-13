# BML to nodes directly, JIT instead of lowering

Where it stands (measured 2026-11-13, one-def edit to `observe/local-plan-run.bml`, `FKWU_CACHE_REPORT=1`, `observe/page-watch-run.bml`):

- A 59,464-byte door becomes 201,998 bytes of lowered Form text (`.lowfk`) and a 38,391,347-byte image (`.fkb`), 645 times its source, because the closure of 351 units is assembled whole.
- A cold build costs 0.37 s per MiB of image produced, about 3 MB/s, from 239 KiB (0.27 s) to 32 MiB (12.08 s); a warm read of the same image is 0.18 s. A trivial door importing the planner's 12 units alone costs 7.63 s. The synthetic 179 KB unit with one import lowers in 0.76 s, so the lowerer is not slow per byte of one unit; the cost follows the closure.
- The cold build is the Form lowerer run by `fk_cell_run_door` in the cold walker: BML text -> grammar templates (`bmf-grammar.fk` `template-emit`, strings) -> Form text -> C parse (about 17% of the beats) -> node rows -> a flat image written at 9 bytes a value (`fk_fkb_write_signed`).
- Strings are the seams between stages and also the cache keys, so one edit invalidates a whole closure while the unit of meaning is the recipe.

Where it goes: BML parses to nodes, each recipe is content-addressed by its own NodeID, the kernel JITs a recipe on first call and keeps the page under that id. An edit changes one NodeID, cold is the re-JIT of what changed, warm is reading pages by id (bandwidth).

Steps, each measured against the planner door's 12.94 s cold and 7.63 s trivial-door cold, each landed through the sibling-validation path:

1. A kernel native that appends one code node row (op and operands) so a Form producer can build nodes without the text hop; the grammar's templates then build nodes, and the Form text becomes a lens, not the pipe. Check: the door's digests equal the text route's.
2. The cache keyed on each recipe's content id, not the file text.
3. Lazy linking: a first call pulls the recipe's node and page; unit top-level lets run on first use, not at load.
4. A compact integer encoding for what is still written.
