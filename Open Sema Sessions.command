#!/bin/sh
set -eu

SEMA_COMMAND_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
/usr/bin/open "$SEMA_COMMAND_ROOT/Sema Sessions.app"
