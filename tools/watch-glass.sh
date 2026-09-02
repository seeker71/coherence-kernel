#!/bin/zsh
# watch-glass.sh — foreground-TTY carrier for the Form-owned Glass supervisor.
# Form owns dependency identity, selfmolt/abstain state, and restart choice. This
# zsh membrane only keeps the terminal attached and obeys that choice.
cd "$(dirname "$0")/.." || exit 1
trap 'exit 130' INT TERM HUP
while :; do
  # Paint the small Form-owned truthful frame before any full-graph admission.
  ./fkwu observe/form-glass-staged-startup-run.fk || { sleep 1; continue; }
  ./fkwu form/form-stdlib/form-glass-launch.bml || { sleep 1; continue; }
  ./fkwu --src observe/form-glass-launch-run.fk || { sleep 1; continue; }
  ./fkwu observe/form-resource-governor-glass-current-run.fk || { sleep 1; continue; }
  ./fkwu observe/form-glass-live-run.fk
  restart=$(./fkwu observe/form-glass-supervisor-restart-run.fk)
  if [[ "$restart" == "0" ]]; then exit 0; fi
  print -r -- "Form Glass supervisor: ${restart:-unavailable}; rebirthing after 250ms"
  sleep 0.25
done
