#!/bin/sh
set -u

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
manifest="$repo_root/presence/fixtures/concept-audio-human-13-source.tsv"
model=${SEMA_WHISPER_MODEL:-"$repo_root/.cache/whisper.cpp/ggml-large-v3-turbo.bin"}
model_expected=1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69
model_actual=$(shasum -a 256 "$model" 2>/dev/null | awk '{print $1}')
if [ "$model_actual" != "$model_expected" ]; then
  printf 'Whisper model missing or wrong hash: %s\n' "$model" >&2
  exit 2
fi
for command_name in whisper-cli ffmpeg shasum awk sed; do
  if ! command -v "$command_name" >/dev/null 2>&1; then
    printf 'missing command: %s\n' "$command_name" >&2
    exit 2
  fi
done
if [ ! -x /usr/bin/curl ]; then
  printf '%s\n' '/usr/bin/curl is required' >&2
  exit 2
fi

run_dir=$(mktemp -d /tmp/sema-human-audio.XXXXXX)
if [ "${CAH13_KEEP_AUDIO:-0}" = 1 ]; then
  printf 'retaining run directory: %s\n' "$run_dir" >&2
else
  trap 'rm -rf "$run_dir"' EXIT HUP INT TERM
fi
evidence=${CAH13_EVIDENCE_FILE:-"$run_dir/evidence.log"}
: > "$evidence"

total=0
success=0
miss=0
unavailable=0
world_admitted=0
download_delay=${CAH13_DOWNLOAD_DELAY_SECONDS:-3}

tail -n +2 "$manifest" |
while IFS="$(printf '\t')" read -r index code concept_id transcription speaker speaker_id language_q recorded license license_url media_id page_url raw_url api_sha1 api_bytes duration_ms api_timestamp snapshot_retrieved first_acquisition observed_sha256; do
  total=$((total + 1))
  retrieved=$(date -u +%Y-%m-%dT%H:%M:%SZ)
  source_file="$run_dir/sample-$index.source"
  wav_file="$run_dir/sample-$index.wav"
  curl_error="$run_dir/sample-$index.curl-error"
  source_file_fk="$run_dir/sample-$index.fk"

  if ! /usr/bin/curl -L --fail --silent --show-error \
      --retry 12 --retry-delay 5 --retry-all-errors --retry-max-time 90 \
      --user-agent 'coherence-kernel-human-audio/0.1 (public corpus verification)' \
      "$raw_url?download=1" -o "$source_file" 2>"$curl_error"; then
    unavailable=$((unavailable + 1))
    sed -n 'p' "$repo_root/presence/concept-audio-human-13-live.fk" > "$source_file_fk"
    printf '\n(cah13-execute-unavailable %s "download-failed" "%s")\n' \
      "$index" "$retrieved" >> "$source_file_fk"
    output=$(cd "$repo_root" && ./fkwu --src "$source_file_fk" 2>>"$evidence")
    printf '%s\n' "$output" >> "$evidence"
    printf '%s\n' "$output" | sed -n '/^human-sample=/p' | sed -n '1p'
    sleep "$download_delay"
    continue
  fi

  actual_sha1=$(shasum -a 1 "$source_file" | awk '{print $1}')
  actual_bytes=$(wc -c < "$source_file" | tr -d ' ')
  actual_sha256=$(shasum -a 256 "$source_file" | awk '{print $1}')
  if [ "$actual_sha1" != "$api_sha1" ] || [ "$actual_sha256" != "$observed_sha256" ] || [ "$actual_bytes" != "$api_bytes" ]; then
    unavailable=$((unavailable + 1))
    sed -n 'p' "$repo_root/presence/concept-audio-human-13-live.fk" > "$source_file_fk"
    printf '\n(cah13-execute-unavailable %s "source-hash-or-size-mismatch" "%s")\n' \
      "$index" "$retrieved" >> "$source_file_fk"
    output=$(cd "$repo_root" && ./fkwu --src "$source_file_fk" 2>>"$evidence")
    printf '%s\n' "$output" >> "$evidence"
    printf '%s\n' "$output" | sed -n '/^human-sample=/p' | sed -n '1p'
    sleep "$download_delay"
    continue
  fi

  if ! ffmpeg -loglevel error -y -i "$source_file" -ar 16000 -ac 1 \
      -c:a pcm_s16le "$wav_file"; then
    unavailable=$((unavailable + 1))
    sed -n 'p' "$repo_root/presence/concept-audio-human-13-live.fk" > "$source_file_fk"
    printf '\n(cah13-execute-unavailable %s "audio-decode-failed" "%s")\n' \
      "$index" "$retrieved" >> "$source_file_fk"
    output=$(cd "$repo_root" && ./fkwu --src "$source_file_fk" 2>>"$evidence")
    printf '%s\n' "$output" >> "$evidence"
    printf '%s\n' "$output" | sed -n '/^human-sample=/p' | sed -n '1p'
    sleep "$download_delay"
    continue
  fi

  stats=$(ffmpeg -hide_banner -nostats -i "$source_file" \
    -af astats=metadata=1:reset=0 -f null - 2>&1)
  rms_db=$(printf '%s\n' "$stats" | awk -F': ' '/RMS level dB/{v=$2} END{print v}')
  peak_db=$(printf '%s\n' "$stats" | awk -F': ' '/Peak level dB/{v=$2} END{print v}')
  noise_floor_db=$(printf '%s\n' "$stats" | awk -F': ' '/Noise floor dB/{v=$2} END{print v}')
  test -n "$rms_db" || rms_db=unavailable
  test -n "$peak_db" || peak_db=unavailable
  test -n "$noise_floor_db" || noise_floor_db=unavailable

  # The basename is numeric and carries no source transcription or concept id.
  # No Whisper prompt flag is supplied.  Form receives only the waveform path,
  # measured source integrity, and acoustic statistics; its detector receives
  # only the resulting transcript and locale.
  sed -n 'p' "$repo_root/presence/concept-audio-human-13-live.fk" > "$source_file_fk"
  printf '\n(cah13-execute-file %s "%s" "%s" "%s" %s "%s" "%s" "%s" "%s")\n' \
    "$index" "$source_file" "$wav_file" "$actual_sha256" "$actual_bytes" "$retrieved" \
    "$rms_db" "$peak_db" "$noise_floor_db" >> "$source_file_fk"
  output=$(cd "$repo_root" && SEMA_WHISPER_MODEL="$model" ./fkwu --src "$source_file_fk" 2>>"$evidence")
  printf '%s\n' "$output" >> "$evidence"
  summary=$(printf '%s\n' "$output" | sed -n '/^human-sample=/p' | sed -n '1p')
  printf '%s\n' "$summary"
  case "$summary" in
    *' status=success '*) success=$((success + 1)) ;;
    *' status=miss '*) miss=$((miss + 1)) ;;
    *) unavailable=$((unavailable + 1)) ;;
  esac
  case "$summary" in
    *' world-admitted=1 '*) world_admitted=$((world_admitted + 1)) ;;
  esac
  sleep "$download_delay"
done

# POSIX pipeline loops run in a subshell.  Recompute authoritative counts from
# the evidence summaries instead of trusting shell-variable scope.
total=$(sed -n '/^human-sample=/p' "$evidence" | wc -l | tr -d ' ')
success=$(sed -n '/^human-sample=.* status=success /p' "$evidence" | wc -l | tr -d ' ')
miss=$(sed -n '/^human-sample=.* status=miss /p' "$evidence" | wc -l | tr -d ' ')
unavailable=$(sed -n '/^human-sample=.* status=unavailable /p' "$evidence" | wc -l | tr -d ' ')
world_admitted=$(sed -n '/^human-sample=.* world-admitted=1 /p' "$evidence" | wc -l | tr -d ' ')
offline_exact=$(sed -n '/^human-sample=.* offline-exact=1 /p' "$evidence" | wc -l | tr -d ' ')
printf 'human-recordings=%s locales=13 speakers=13 success=%s miss=%s unavailable=%s world-admitted=%s offline-exact=%s detector-limit=10000 tts=0 prompt=0 evidence=%s\n' \
  "$total" "$success" "$miss" "$unavailable" "$world_admitted" "$offline_exact" "$evidence"
test "$total" -eq 13
test $((success + miss + unavailable)) -eq 13
test $((offline_exact + unavailable)) -eq 13
