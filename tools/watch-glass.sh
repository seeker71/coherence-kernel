#!/bin/zsh
# watch-glass.sh — the hearth's living dashboard, supervised: stays
# connected, reborn within a second whenever the glass sources change
# (selfmolt) or anything kills it. Run from anywhere:
#   ./tools/watch-glass.sh
cd "$(dirname "$0")/.."
while :; do printf '1\n' | ./fkwu observe/hearth-glass-live.fk; sleep 1; done
