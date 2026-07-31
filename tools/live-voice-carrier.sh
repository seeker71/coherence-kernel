#!/usr/bin/env zsh
# live-voice-carrier.sh — the two transport lines the live loop does NOT own.
# The mind is form-stdlib/live-loop.fk running on fkwu; this script is eardrum and speaker cone.
#   usage: live-voice-carrier.sh <dir> [seconds] [device]
# Writes <dir>/stream.raw from the microphone; plays reply-*.wav as they appear;
# hands turn-*-utter.wav (the unknown utterances) to whisper-cli, leaving *-oracle.txt beside them.
set -euo pipefail
DIR=${1:?dir}; SECS=${2:-30}; DEV=${3:-1}
MODEL=${WHISPER_MODEL:-$HOME/.cache/whisper.cpp/ggml-small.bin}
: > "$DIR/stream.raw"
ffmpeg -v error -f avfoundation -i ":$DEV" -t "$SECS" -ar 8000 -ac 1 -f s16le - >> "$DIR/stream.raw" &
CAP=$!
typeset -A seen
while kill -0 $CAP 2>/dev/null; do
    for f in "$DIR"/reply-*.wav(N); do
        [[ -n "${seen[$f]:-}" ]] && continue; seen[$f]=1
        afplay -v 0.8 "$f" &
    done
    for u in "$DIR"/turn-*-utter.wav(N); do
        [[ -n "${seen[$u]:-}" ]] && continue; seen[$u]=1
        ( ffmpeg -v error -y -i "$u" -ar 16000 "${u%.wav}-16k.wav" && \
          whisper-cli -m "$MODEL" -f "${u%.wav}-16k.wav" -np 2>/dev/null | tail -1 > "${u%.wav}-oracle.txt" ) &
    done
    sleep 0.1
done
wait
