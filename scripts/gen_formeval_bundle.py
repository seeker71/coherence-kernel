#!/usr/bin/env python3
# gen_formeval_bundle.py — emit runtime/fkwu-formeval.h from grammars/form-eval.fk.
# The --feval mode embeds the form-eval meta-evaluator (plus the char_at/ord helpers that
# its core.fk prelude would otherwise supply) so the C seed can bootstrap form-eval and let
# form-eval run the recipe. Regenerate after editing grammars/form-eval.fk; do not hand-edit
# the generated header.
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Helpers form-eval needs that the surface-grammar --src parser cannot lift from core.fk.
HELPERS = (
    "(defn char_at (s i) (substring s i (add i 1)))\n"
    "(defn ord (c) (str_byte_at c 0))\n"
)


def cesc(s):
    out = []
    for ch in s:
        o = ord(ch)
        if ch == "\\":
            out.append("\\\\")
        elif ch == '"':
            out.append('\\"')
        elif ch == "\n":
            out.append("\\n")
        elif ch == "\t":
            out.append("\\t")
        elif ch == "\r":
            pass
        elif o < 32:
            out.append("\\%03o" % o)
        else:
            out.append(ch)
    return "".join(out)


def main():
    with open(os.path.join(ROOT, "grammars", "form-eval.fk"), "r", encoding="utf-8") as f:
        fe = f.read()
    bundle = HELPERS + fe
    esc = cesc(bundle)
    # Chunk into C string fragments; never split inside an escape (don't end on an odd run of '\').
    frags = []
    i = 0
    while i < len(esc):
        j = min(i + 180, len(esc))
        while j > i:
            tail = esc[i:j]
            trailing = len(tail) - len(tail.rstrip("\\"))
            if trailing % 2 == 1:
                j -= 1
            else:
                break
        frags.append(esc[i:j])
        i = j
    out_path = os.path.join(ROOT, "runtime", "fkwu-formeval.h")
    with open(out_path, "w", encoding="utf-8", newline="\n") as o:
        o.write("/* fkwu-formeval.h — GENERATED from grammars/form-eval.fk (+ char_at/ord helpers).\n")
        o.write("   The form-eval meta-evaluator, embedded so `--feval` is self-contained (no runtime file dep).\n")
        o.write("   Regenerate: python3 scripts/gen_formeval_bundle.py  (do not hand-edit). */\n")
        o.write("static const char *fk_formeval_src =\n")
        for fr in frags:
            o.write('  "%s"\n' % fr)
        o.write(";\n")
    print("wrote %s, bundle bytes=%d, frags=%d" % (out_path, len(bundle), len(frags)))


if __name__ == "__main__":
    main()
