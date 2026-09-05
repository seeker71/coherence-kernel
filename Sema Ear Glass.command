#!/bin/sh
# Sema Ear Glass — double-click (or bind a Shortcut to this file for a hot key): a full-screen
# terminal transcribing the room live in English, Persian and Brazilian Portuguese.
cd "$(dirname "$0")" || exit 1
printf '\033[?1049h'; printf '600\n\n' | ./fkwu observe/ear-glass-live.fk; printf '\033[?1049l'
