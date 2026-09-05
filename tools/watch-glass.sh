#!/bin/zsh
# watch-glass.sh — foreground-TTY carrier for the Form-owned Glass supervisor.
# Form owns dependency identity, selfmolt/abstain state, and restart choice. This
# zsh membrane only keeps the terminal attached and obeys that choice. It also
# stands the sensor process beside the glass: the rows that fork or scan the
# filesystem are gathered there at their own cadence and given into shared
# memory, so the glass frame path holds only gift reads, render, and a native
# wait (observe/form-glass-sensors-live.fk). Ending the carrier ends the sensors.
cd "$(dirname "$0")/.." || exit 1
if [[ "${FORM_GLASS_NICE_APPLIED:-0}" != "1" ]]; then
  export FORM_GLASS_NICE_APPLIED=1
  exec /usr/bin/nice -n 19 "$0" "$@"
fi
./fkwu observe/form-glass-sensors-live.fk >/dev/null 2>&1 &
sensors=$!
trap 'kill "$sensors" 2>/dev/null; exit 130' INT TERM HUP
while :; do
  # Paint the small Form-owned truthful frame before any full-graph admission.
  ./fkwu observe/form-glass-staged-startup-run.fk || { sleep 1; continue; }
  ./fkwu form/form-stdlib/form-glass-launch.bml || { sleep 1; continue; }
  ./fkwu observe/form-glass-launch-run.fk || { sleep 1; continue; }
  ./fkwu observe/form-resource-governor-glass-current-run.fk || { sleep 1; continue; }
  ./fkwu observe/form-glass-live-run.fk
  restart=$(./fkwu observe/form-glass-supervisor-restart-run.fk)
  if [[ "$restart" == "0" ]]; then kill "$sensors" 2>/dev/null; exit 0; fi
  print -r -- "Form Glass supervisor: ${restart:-unavailable}; rebirthing after 250ms"
  sleep 0.25
done
