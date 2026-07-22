#!/usr/bin/env bash
# grade.sh — the objective half of Stone 44. Nothing here is a judgement of quality;
# every verdict is a string equality against an answer already in the repo, or the
# exit of the body's own compiler on the model's bytes.
#
# selfgauge (834) — THE DENOMINATOR, said out loud. Each task is graded against a
# fact this repo already holds, not against a preference:
#   t1  the prelude that actually makes tiny-matvec-band.fk print 1 instead of 0.
#       Reproduced: form/form-stdlib/transformer-block.fk:27 defines tb-matvec, and
#       adding that one file to the preludes line flips the verdict 0 -> 1.
#   t2  fkwu AND bin-go must both print 30. Not "looks like Form" — compiles and
#       computes. A plausible cell that does not compile is a fail.
#   t2b snugcause: the same shape with a filter and a cube, answer 91. A model that
#       passes t2 by echoing the example prints 30 here and is caught by the number.
#   t3  ds4.c:10318 `float max_score = sinks[h];` — the sink IS in the max-shift.
#   t4  the 12-step table in receipts/2026-07-22-mla-recipe.md, reduced to seven
#       checkpoints each decided by a tensor in the prompt. Machine-scored on
#       keyword evidence, and every hit prints the line it fired on so the mark can
#       be overruled by a reader.
#
# usage: tools/local-authoring-capability/grade.sh <out-dir>
set -u
OUT="${1:?usage: grade.sh <out-dir>}"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

verdict () { printf '  %-5s %-6s %s\n' "$1" "$2" "$3"; }

for j in "$OUT"/*__t1.json; do
  [ -e "$j" ] || continue
  SLUG="$(basename "$j" __t1.json)"
  echo "=== $SLUG ==="

  # zerobirth / edgedrop: an empty or one-token-looping answer is a FAILED RUN and is
  # named as such, never scored as a wrong answer.
  for t in t1 t2 t2b t3 t4; do
    f="$OUT/${SLUG}__${t}.json"; [ -e "$f" ] || continue
    if grep -q '"empty": true' "$f"; then verdict "$t" "NORUN" "empty response (edgedrop)"; fi
    if grep -q '"degenerate": true' "$f"; then verdict "$t" "NORUN" "degenerate repetition (zerobirth)"; fi
  done

  # t1 — one file name, exact.
  a="$OUT/${SLUG}__t1.txt"
  if [ -e "$a" ]; then
    if grep -qi 'form-stdlib/transformer-block\.fk' "$a"; then verdict t1 PASS "named transformer-block.fk"
    else verdict t1 FAIL "$(tr -d '\n' < "$a" | head -c 90)"; fi
  fi

  # t2 / t2b — the body's own compilers decide. Two arms, both must agree.
  for pair in "t2 30" "t2b 91"; do
    set -- $pair; t="$1"; want="$2"
    a="$OUT/${SLUG}__${t}.txt"; [ -e "$a" ] || continue
    # strip markdown fences if the model added them despite being told not to
    sed -e 's/^```.*$//' "$a" > "$OUT/${SLUG}__${t}.fk"
    fk="$(./fkwu --src "$OUT/${SLUG}__${t}.fk" 2>&1 | grep -v 'stale\|dylib' | tail -1)"
    cat form/form-stdlib/core.fk "$OUT/${SLUG}__${t}.fk" > "$OUT/${SLUG}__${t}.go.fk"
    go="$(./form/form-kernel-go/bin-go "$OUT/${SLUG}__${t}.go.fk" 2>&1 | tail -1)"
    if [ "$fk" = "$want" ] && [ "$go" = "$want" ]; then verdict "$t" PASS "fkwu=$fk bin-go=$go"
    else verdict "$t" FAIL "fkwu=$fk bin-go=$go (want $want)"; fi
  done

  # t3 — the answer line, exact, and the deciding line quoted.
  a="$OUT/${SLUG}__t3.txt"
  if [ -e "$a" ]; then
    if grep -qi 'SINK IS INCLUDED IN THE MAX' "$a" && ! grep -qi 'SINK IS NOT INCLUDED' "$a"
    then verdict t3 PASS "$(grep -i 'max_score = sinks' "$a" | head -1 | tr -d '\n' | head -c 60)"
    else verdict t3 FAIL "$(grep -iE 'ANSWER|SINK IS' "$a" | head -1 | tr -d '\n' | head -c 90)"; fi
  fi

  # t4 — seven checkpoints, each printed with its evidence line.
  a="$OUT/${SLUG}__t4.txt"
  if [ -e "$a" ]; then
    hits=0
    chk () {  # chk <label> <ere>
      if grep -qiE "$2" "$a"; then hits=$((hits+1)); printf '        + %-22s %s\n' "$1" \
        "$(grep -iEm1 "$2" "$a" | tr -d '\n' | head -c 78)"
      else printf '        - %-22s (absent)\n' "$1"; fi
    }
    chk "q_a>norm>q_b"     'q_a_norm.*(then|to|before).*q_b|q_a_normali[sz]ed.*q_b'
    chk "per-head q rms"   'per[- ]head.*(rms|norm)|head_rms|unweighted.*norm'
    chk "rope tail 64"     '(tail|last|trailing|final).{0,24}64|64.{0,24}(tail|trailing|last)'
    chk "K == V same row"  '(K and V are|k *= *v|same row|K *is *(also *)?V|V *= *K|identical row)'
    chk "sink denominator" 'denominator|no value vector|contributes no value'
    chk "inverse rope out" 'inverse (rope|rotar)|un-?rope|rope.{0,20}(inverse|-1)'
    chk "grouped out 8x"   '(8|eight) *(groups|group)|grouped.{0,30}(projection|output)'
    verdict t4 "$hits/7" "checkpoints from receipts/2026-07-22-mla-recipe.md"
  fi
done
