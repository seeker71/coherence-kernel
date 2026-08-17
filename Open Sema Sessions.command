#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"

if [ ! -x ./fkwu ]; then
  if [ "$(uname -s)" = Darwin ] && [ -f form/native/metal/fk-metal-carrier.m ]; then
    cc -O2 -o fkwu runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
      -framework Metal -framework Foundation -fobjc-arc
  else
    cc -O2 -o fkwu runtime/fkwu-uni.c
  fi
fi

SESSION_TEMP=$(mktemp -d /tmp/sema-sessions.XXXXXX)
SESSION_SOURCE="$SESSION_TEMP/session-app.fk"
SESSION_IMAGE="$SESSION_TEMP/session-app"
trap 'rm -f "$SESSION_SOURCE" "$SESSION_IMAGE.fkb" "$SESSION_IMAGE.sym"; rmdir "$SESSION_TEMP"' EXIT
cat form/form-stdlib/core.fk \
    form/form-stdlib/session-evidence.fk \
    form/form-stdlib/session-app.fk \
    form/form-stdlib/session-app-live.fk \
    form/form-stdlib/session-app-run.fk > "$SESSION_SOURCE"

(sleep 1; open http://127.0.0.1:8765/) &
./fkwu "$SESSION_SOURCE"
