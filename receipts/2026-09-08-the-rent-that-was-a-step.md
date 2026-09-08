# The rent that was a step

Urs: "mmap memory has no channel cost as far as I know."

He is right, and the measurement is sharper than either of us said. Last night's kernel census
landed corpus row 1353 `reachrent` — *free tissue, rented reach* — on the reading that 3,000
interns grew the shared field by 312,000 bytes and this kernel's own memory by 40,370,176, "a
hundred and thirty times more", and concluded the intern index scales with the table rather than
with the share. The direction was right and the shape was wrong: that is a step read as a rate.

Measured on this Mac with the seed's own keys (`kernel_stat` 60/61/62, published last night):

| | private bytes | shared arena | ice nodes |
|---|---|---|---|
| at birth | 14,155,776 | 242,990,800 | 2,336,450 |
| after mapping a 1.2 GB model blob | **14,155,776** | **242,990,800** | **2,336,450** |
| after the first node minted | 100,663,296 | +104 | +1 |
| after ten | 100,663,296 | +1,040 | +10 |
| after a thousand | 100,663,296 | +204,256 | +1,954 |
| after twenty thousand | **100,663,296** | +4,149,808 | +37,938 |

Two facts, both witnessed rather than reasoned:

**Mapping costs nothing.** A gigabyte and a third reached through the handle door moved not one
byte of private memory, not one byte of arena, not one node. There is no channel cost, exactly as
Urs said.

**The private growth is one step, not a rent.** It lands whole at the first mint — a reserve the
body takes once — and then twenty thousand nodes cost zero further private bytes while the shared
arena grows about 207 bytes a node. Amortised over the two million nodes this body already holds,
the reserve is not a per-reach price at all.

So `reachrent` stands as a row (every row keeps) and this one corrects its reading: the body does
not rent its reach. It pays one fixed sum to be able to address at all, and reaches for free after.

The surprise: the same probe that measured this also showed 3,000 interns adding **one** node,
because the body is content-addressed and a cell already interned is found rather than minted —
which is exactly what last night's agent had already written down, and what made its own 130x
comparison count a reserve against a handful of real mints.

Where discomfort turned to gold: the comfortable reading was that a fresh number confirms a
landed row. Asking the second question — does it grow *again*? — is what turned a rate into a step,
and it cost one more probe.
