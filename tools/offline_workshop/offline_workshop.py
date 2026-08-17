#!/usr/bin/env python3
"""Local, consentful session capture, transcription, and source-led authoring.

This carrier deliberately uses only the Python standard library.  It moves host
audio and local process results into an inspectable bundle; Form remains the
authority that decides whether the evidence is trustworthy.
"""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import os
import platform
import re
import shutil
import signal
import subprocess
import sys
import tempfile
import time
from pathlib import Path
from typing import Any, Iterable


SCHEMA = "coherence.offline-workshop/v1"
VERSION = "1.0.0"


class WorkshopError(RuntimeError):
    def __init__(self, code: str, message: str, resolution: str):
        super().__init__(message)
        self.code = code
        self.resolution = resolution


def now_utc() -> str:
    return dt.datetime.now(dt.timezone.utc).replace(microsecond=0).isoformat()


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(path.name + ".tmp")
    temporary.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    temporary.replace(path)


def write_text(path: Path, value: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(path.name + ".tmp")
    temporary.write_text(value, encoding="utf-8")
    temporary.replace(path)


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def executable(value: str) -> str | None:
    if os.path.sep in value:
        candidate = Path(value).expanduser()
        return str(candidate.resolve()) if candidate.is_file() and os.access(candidate, os.X_OK) else None
    return shutil.which(value)


def discover_model(explicit: str | None = None) -> Path | None:
    if explicit:
        candidate = Path(explicit).expanduser()
        return candidate.resolve() if candidate.is_file() else None
    candidates: list[Path] = []
    configured = os.environ.get("COHERENCE_WHISPER_MODEL")
    if configured:
        candidates.append(Path(configured).expanduser())
    candidates.extend(
        [
            Path.home() / ".cache/whisper.cpp/ggml-large-v3-turbo.bin",
            Path.home() / ".cache/whisper.cpp/ggml-medium.bin",
            Path.home() / ".cache/whisper.cpp/ggml-small.bin",
            Path.cwd() / "models/ggml-large-v3-turbo.bin",
            Path.cwd() / "models/ggml-small.bin",
        ]
    )
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    return None


def command_version(command: str, args: list[str]) -> str | None:
    resolved = executable(command)
    if not resolved:
        return None
    try:
        result = subprocess.run(
            [resolved, *args], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, timeout=10, check=False,
        )
    except (OSError, subprocess.SubprocessError):
        return "present"
    first = next((line.strip() for line in result.stdout.splitlines() if line.strip()), "present")
    return first[:240]


def doctor_report(args: argparse.Namespace) -> dict[str, Any]:
    model = discover_model(args.model)
    ffmpeg = executable(args.ffmpeg_bin)
    whisper = executable(args.whisper_bin)
    fkwu = executable(args.fkwu_bin)
    if not fkwu:
        repo_fkwu = Path(__file__).resolve().parents[2] / "fkwu"
        fkwu = str(repo_fkwu) if repo_fkwu.is_file() and os.access(repo_fkwu, os.X_OK) else None
    required = {
        "ffmpeg": bool(ffmpeg),
        "whisper_cli": bool(whisper),
        "whisper_model": bool(model),
        "form_gate": bool(fkwu),
    }
    return {
        "schema": SCHEMA,
        "command": "doctor",
        "ready": all(required.values()),
        "required": required,
        "optional": {
            "ollama": bool(executable("ollama")),
            "llama_cli": bool(executable("llama-cli")),
        },
        "resolved": {
            "ffmpeg": ffmpeg,
            "whisper_cli": whisper,
            "whisper_model": str(model) if model else None,
            "fkwu": fkwu,
        },
        "versions": {
            "carrier": VERSION,
            "python": platform.python_version(),
            "ffmpeg": command_version(args.ffmpeg_bin, ["-version"]),
            "whisper_cli": command_version(args.whisper_bin, ["--version"]),
        },
        "resolution": [] if all(required.values()) else [
            "Install ffmpeg and whisper-cli, then set COHERENCE_WHISPER_MODEL to a local ggml model file.",
            "Build ./fkwu from the repository bootstrap so each session crosses the Form trust gate.",
            "Run ./offline-workshop doctor again; no network is used by this command.",
        ],
    }


def require_consent(consent: str) -> None:
    if consent != "explicit":
        raise WorkshopError(
            "consent-not-explicit",
            "Capture and transcription require --consent explicit.",
            "Confirm the participants agreed, then repeat the command with --consent explicit.",
        )


def begin_session(session: Path, origin: str) -> None:
    if session.exists():
        raise WorkshopError(
            "session-exists", f"Session path already exists: {session}",
            "Choose a new session directory. Existing evidence is never overwritten.",
        )
    for child in ("audio", "transcript", "drafts", "evidence"):
        (session / child).mkdir(parents=True, exist_ok=True)
    write_json(session / "status.json", {
        "schema": SCHEMA, "state": "working", "origin": origin, "updated_at": now_utc(),
    })


def mark_failure(session: Path, error: WorkshopError) -> None:
    if not session.exists():
        return
    failure = {
        "schema": SCHEMA,
        "state": "failed",
        "code": error.code,
        "message": str(error),
        "resolution": error.resolution,
        "updated_at": now_utc(),
    }
    write_json(session / "status.json", failure)
    write_json(session / "evidence/failure.json", failure)


def run_quiet(command: list[str], timeout: int | None = None) -> subprocess.CompletedProcess[bytes]:
    try:
        return subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE,
            timeout=timeout, check=False,
        )
    except subprocess.TimeoutExpired as exc:
        return subprocess.CompletedProcess(command, 124, b"", exc.stderr or b"")
    except OSError as exc:
        raise WorkshopError(
            "process-start-failed", f"Could not start {command[0]}: {exc}",
            "Run ./offline-workshop doctor and resolve the missing or inaccessible executable.",
        ) from exc


def normalize_audio(source: Path, target: Path, ffmpeg_bin: str) -> dict[str, Any]:
    ffmpeg = executable(ffmpeg_bin)
    if not ffmpeg:
        raise WorkshopError(
            "ffmpeg-missing", "ffmpeg is required to normalize the session audio.",
            "Install ffmpeg, then rerun ./offline-workshop doctor.",
        )
    if not source.is_file():
        raise WorkshopError(
            "audio-missing", f"Audio file does not exist: {source}",
            "Point --audio at a readable local recording.",
        )
    started = time.monotonic()
    result = run_quiet([
        ffmpeg, "-nostdin", "-hide_banner", "-loglevel", "error", "-y",
        "-i", str(source), "-vn", "-ar", "16000", "-ac", "1",
        "-c:a", "pcm_s16le", str(target),
    ])
    if result.returncode != 0 or not target.is_file() or target.stat().st_size <= 44:
        raise WorkshopError(
            "audio-normalization-failed",
            f"ffmpeg could not normalize the audio (exit {result.returncode}).",
            "Check that the recording is readable and rerun. The failure bundle preserves the evidence path.",
        )
    return {
        "kind": "observed", "exit_code": result.returncode,
        "seconds": round(time.monotonic() - started, 3),
        "sample_rate_hz": 16000, "channels": 1, "encoding": "pcm_s16le",
    }


def parse_whisper_json(path: Path) -> tuple[list[dict[str, Any]], dict[str, Any]]:
    try:
        raw = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise WorkshopError(
            "asr-output-invalid", "whisper-cli did not produce valid JSON evidence.",
            "Inspect the model and audio with ./offline-workshop doctor, then retry on CPU.",
        ) from exc
    transcription = raw.get("transcription")
    if not isinstance(transcription, list):
        raise WorkshopError(
            "asr-output-invalid", "whisper-cli JSON has no transcription list.",
            "Verify the installed whisper-cli version and retry.",
        )
    segments: list[dict[str, Any]] = []
    for index, item in enumerate(transcription, 1):
        if not isinstance(item, dict):
            continue
        offsets = item.get("offsets") or {}
        text = str(item.get("text", "")).strip()
        if not text:
            continue
        probabilities: list[float] = []
        for token in item.get("tokens") or []:
            token_text = str(token.get("text", ""))
            probability = token.get("p")
            if not token_text.startswith("[_") and isinstance(probability, (int, float)):
                probabilities.append(float(probability))
        segments.append({
            "schema": SCHEMA,
            "segment_id": f"seg-{index:06d}",
            "start_ms": int(offsets.get("from", 0)),
            "end_ms": int(offsets.get("to", 0)),
            "speaker": "unknown",
            "speaker_source": "not-observed",
            "overlap": "unknown",
            "text": text,
            "token_probability_mean": round(sum(probabilities) / len(probabilities), 6) if probabilities else None,
            "source_ref": f"audio/source.wav#t={int(offsets.get('from', 0))},{int(offsets.get('to', 0))}",
            "claim_kind": "observed",
        })
    if not segments:
        raise WorkshopError(
            "asr-empty", "whisper-cli returned no spoken segments.",
            "Check the recording level and input device; preserve silence as silence rather than inventing text.",
        )
    meta = {
        "language": (raw.get("result") or {}).get("language"),
        "model_type": (raw.get("model") or {}).get("type"),
        "systeminfo": raw.get("systeminfo"),
    }
    return segments, meta


def transcribe_whisper(
    audio: Path, output_stem: Path, whisper_bin: str, model: Path,
    language: str, cpu_only: bool, timeout: int,
) -> tuple[list[dict[str, Any]], dict[str, Any], list[dict[str, Any]]]:
    whisper = executable(whisper_bin)
    if not whisper:
        raise WorkshopError(
            "whisper-cli-missing", "whisper-cli is required for local transcription.",
            "Install whisper.cpp's whisper-cli and rerun ./offline-workshop doctor.",
        )
    attempts: list[dict[str, Any]] = []
    modes = ["cpu"] if cpu_only else ["accelerated", "cpu-fallback"]
    for mode in modes:
        output_json = output_stem.with_suffix(".json")
        if output_json.exists():
            output_json.unlink()
        command = [
            whisper, "-m", str(model), "-f", str(audio), "-np", "-ojf",
            "-of", str(output_stem), "-l", language,
        ]
        if mode != "accelerated":
            command.append("-ng")
        started = time.monotonic()
        result = run_quiet(command, timeout=timeout)
        elapsed = round(time.monotonic() - started, 3)
        attempt = {
            "mode": mode,
            "kind": "observed",
            "exit_code": result.returncode,
            "seconds": elapsed,
            "result": "resolved" if result.returncode == 0 and output_json.is_file() else "failed-attention",
        }
        attempts.append(attempt)
        if attempt["result"] == "resolved":
            segments, metadata = parse_whisper_json(output_json)
            return segments, metadata, attempts
    write_json(
        output_stem.parent.parent / "evidence/asr-attempts.json",
        {"schema": SCHEMA, "attempts": attempts},
    )
    raise WorkshopError(
        "asr-all-attempts-failed",
        "Both the requested Whisper lane and its CPU resolution attempt failed.",
        "Review evidence/asr-attempts.json, confirm the model fits memory, and retry with --cpu or a smaller model.",
    )


def timestamp(milliseconds: int) -> str:
    total_seconds, millis = divmod(max(0, milliseconds), 1000)
    hours, remainder = divmod(total_seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    return f"{hours:02d}:{minutes:02d}:{seconds:02d}.{millis:03d}"


def markdown_text(value: str) -> str:
    text = " ".join(value.splitlines())
    text = text.replace("\\", "\\\\")
    for character in ("`", "*", "_", "{", "}", "[", "]", "<", ">", "|", "#"):
        text = text.replace(character, "\\" + character)
    return text


def write_transcript(session: Path, segments: list[dict[str, Any]]) -> None:
    jsonl = "".join(json.dumps(segment, sort_keys=True) + "\n" for segment in segments)
    write_text(session / "transcript/segments.jsonl", jsonl)
    lines = [
        "# Session transcript\n",
        "> Observed ASR text. Speaker names remain `unknown` until a person reviews them.\n",
    ]
    for segment in segments:
        segment_id = segment["segment_id"]
        lines.extend([
            f'<a id="{segment_id}"></a>\n',
            f"**[{timestamp(segment['start_ms'])}–{timestamp(segment['end_ms'])}] "
            f"{markdown_text(segment['speaker'])}** (`{segment_id}`)\n",
            f"{markdown_text(segment['text'])}\n",
        ])
    write_text(session / "transcript/transcript.md", "\n".join(lines))


def draft_documents(session: Path, segments: list[dict[str, Any]]) -> int:
    excerpt_count = min(20, len(segments))
    manual = [
        "# Workshop manual — source-grounded draft\n",
        "> Kind: declared draft over observed transcript segments. Every excerpt links back to its source. "
        "No speaker identity, interpretation, or group agreement is inferred.\n",
        "## Session ground\n",
        f"- Observed transcript segments: {len(segments)}",
        f"- Human-reviewed speaker labels: {sum(s['speaker_source'] == 'human-reviewed' for s in segments)}",
        f"- Speaker labels still unknown: {sum(s['speaker'] == 'unknown' for s in segments)}",
        "\n## Source excerpts\n",
    ]
    for segment in segments[:excerpt_count]:
        link = f"../transcript/transcript.md#{segment['segment_id']}"
        manual.extend([
            f"### [{timestamp(segment['start_ms'])} · {markdown_text(segment['speaker'])}]({link})\n",
            f"> {markdown_text(segment['text'])}\n",
            "Reflection: What did this mean in the room, and what would participants change or clarify?\n",
        ])
    manual.extend([
        "## Human attention before publication\n",
        "- Review speaker labels with the participants; do not infer identity from voice alone.",
        "- Mark overlap, inaudible passages, corrections, and consent withdrawals.",
        "- Separate verbatim excerpts, facilitator interpretation, and newly written prose.",
        "- Obtain explicit approval before sharing anything beyond the local session boundary.\n",
    ])
    write_text(session / "drafts/workshop-manual.md", "\n".join(manual))

    ledger = [
        "# Book source ledger\n",
        "> A local source index, not a book claim. Each row remains traceable to observed session evidence.\n",
        "| Segment | Time | Speaker status | Source text |",
        "|---|---:|---|---|",
    ]
    for segment in segments:
        safe_text = markdown_text(segment["text"])
        safe_speaker = markdown_text(segment["speaker"])
        link = f"../transcript/transcript.md#{segment['segment_id']}"
        ledger.append(
            f"| [{segment['segment_id']}]({link}) | {timestamp(segment['start_ms'])} | "
            f"{safe_speaker} ({segment['speaker_source']}) | {safe_text} |"
        )
    ledger.append("")
    write_text(session / "drafts/book-source-ledger.md", "\n".join(ledger))
    return excerpt_count


def segment_metrics(segments: list[dict[str, Any]], attempts: list[dict[str, Any]]) -> dict[str, Any]:
    known = sum(segment["speaker"] != "unknown" for segment in segments)
    unknown = len(segments) - known
    reviewed = sum(segment["speaker_source"] == "human-reviewed" for segment in segments)
    end_ms = max((segment["end_ms"] for segment in segments), default=0)
    probabilities = [
        segment["token_probability_mean"] for segment in segments
        if segment.get("token_probability_mean") is not None
    ]
    return {
        "schema": SCHEMA,
        "kind": "observed",
        "segments": len(segments),
        "known_speaker_segments": known,
        "unknown_speaker_segments": unknown,
        "human_reviewed_speaker_segments": reviewed,
        "speaker_attribution_coverage": round(known / len(segments), 6) if segments else 0.0,
        "overlap_observed_segments": sum(segment["overlap"] is True for segment in segments),
        "overlap_unknown_segments": sum(segment["overlap"] == "unknown" for segment in segments),
        "source_linked_segments": sum(bool(segment.get("source_ref")) for segment in segments),
        "duration_ms": end_ms,
        "word_count": sum(len(segment["text"].split()) for segment in segments),
        "token_probability_mean": round(sum(probabilities) / len(probabilities), 6) if probabilities else None,
        "asr_attempts": attempts,
        "wer": None,
        "wer_reason": "No human reference transcript was supplied.",
        "der": None,
        "der_reason": "No time-aligned human speaker reference was supplied.",
    }


def normalized_words(text: str) -> list[str]:
    return re.findall(r"[\w']+", text.casefold(), flags=re.UNICODE)


def edit_distance(reference: list[str], hypothesis: list[str]) -> int:
    previous = list(range(len(hypothesis) + 1))
    for ref_index, ref_word in enumerate(reference, 1):
        current = [ref_index]
        for hyp_index, hyp_word in enumerate(hypothesis, 1):
            current.append(min(
                current[-1] + 1,
                previous[hyp_index] + 1,
                previous[hyp_index - 1] + (ref_word != hyp_word),
            ))
        previous = current
    return previous[-1]


def read_speaker_reference(path: Path) -> list[dict[str, Any]]:
    intervals: list[dict[str, Any]] = []
    for line_number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        if not line.strip() or line.startswith("#"):
            continue
        fields = line.split("\t")
        if len(fields) != 3:
            raise WorkshopError(
                "speaker-reference-invalid", f"Invalid speaker reference row {line_number}.",
                "Use start_ms<TAB>end_ms<TAB>participant-approved speaker label.",
            )
        try:
            start_ms, end_ms = int(fields[0]), int(fields[1])
        except ValueError as exc:
            raise WorkshopError(
                "speaker-reference-invalid", f"Non-integer time in speaker reference row {line_number}.",
                "Use integer millisecond boundaries.",
            ) from exc
        speaker = fields[2].strip()
        if start_ms < 0 or end_ms <= start_ms or not speaker:
            raise WorkshopError(
                "speaker-reference-invalid", f"Invalid interval in speaker reference row {line_number}.",
                "Each reviewed interval needs start_ms >= 0, end_ms > start_ms, and a speaker label.",
            )
        intervals.append({"start_ms": start_ms, "end_ms": end_ms, "speaker": speaker})
    if not intervals:
        raise WorkshopError(
            "speaker-reference-empty", "The time-aligned speaker reference has no rows.",
            "Add at least one participant-reviewed interval or omit --speaker-reference.",
        )
    return intervals


def active_label(intervals: list[dict[str, Any]], point: float) -> str | None:
    active = [row["speaker"] for row in intervals if row["start_ms"] <= point < row["end_ms"]]
    if len(active) > 1:
        return "__overlap__"
    return active[0] if active else None


def label_error_measure(
    segments: list[dict[str, Any]], reference: list[dict[str, Any]],
) -> dict[str, Any]:
    hypothesis = [
        {"start_ms": int(segment["start_ms"]), "end_ms": int(segment["end_ms"]),
         "speaker": segment["speaker"]}
        for segment in segments
    ]
    boundaries = sorted({
        int(value)
        for row in [*reference, *hypothesis]
        for value in (row["start_ms"], row["end_ms"])
    })
    miss = false_alarm = speaker_error = reference_ms = overlap_ms = 0
    for start_ms, end_ms in zip(boundaries, boundaries[1:]):
        if end_ms <= start_ms:
            continue
        duration = end_ms - start_ms
        point = start_ms + duration / 2
        ref_label = active_label(reference, point)
        hyp_label = active_label(hypothesis, point)
        if ref_label == "__overlap__":
            overlap_ms += duration
            continue
        if ref_label is not None:
            reference_ms += duration
        if ref_label is None and hyp_label is not None:
            false_alarm += duration
        elif ref_label is not None and hyp_label is None:
            miss += duration
        elif ref_label is not None and hyp_label != ref_label:
            speaker_error += duration
    if reference_ms <= 0:
        raise WorkshopError(
            "speaker-reference-empty", "No non-overlap reference duration remained for evaluation.",
            "Provide at least one participant-reviewed single-speaker interval.",
        )
    numerator = miss + false_alarm + speaker_error
    return {
        "der": round(numerator / reference_ms, 6),
        "method": "time-weighted-label-error/no-collar/exact-reviewed-labels",
        "reference_ms": reference_ms,
        "miss_ms": miss,
        "false_alarm_ms": false_alarm,
        "speaker_error_ms": speaker_error,
        "reference_overlap_excluded_ms": overlap_ms,
    }


def quality_evaluation(
    segments: list[dict[str, Any]], reference_text: str,
    speaker_reference: list[dict[str, Any]] | None,
) -> dict[str, Any]:
    reference_words = normalized_words(reference_text)
    hypothesis_words = normalized_words(" ".join(segment["text"] for segment in segments))
    if not reference_words:
        raise WorkshopError(
            "reference-transcript-empty", "The human reference transcript has no comparable words.",
            "Provide a reviewed plain-text transcript for this same audio.",
        )
    edits = edit_distance(reference_words, hypothesis_words)
    evaluation: dict[str, Any] = {
        "schema": SCHEMA,
        "kind": "observed-against-human-reference",
        "wer": round(edits / len(reference_words), 6),
        "wer_counts": {
            "word_edits": edits,
            "reference_words": len(reference_words),
            "hypothesis_words": len(hypothesis_words),
        },
        "wer_method": "unicode-casefold-word-levenshtein",
        "der": None,
        "der_reason": "No time-aligned human speaker reference was supplied.",
    }
    if speaker_reference is not None:
        evaluation.update(label_error_measure(segments, speaker_reference))
        evaluation["der_reason"] = None
    return evaluation


def artifact_hashes(session: Path) -> dict[str, dict[str, Any]]:
    relative_paths = [
        "audio/capture.wav",
        "audio/source.wav",
        "transcript/whisper.json",
        "transcript/segments.jsonl",
        "transcript/transcript.md",
        "drafts/workshop-manual.md",
        "drafts/book-source-ledger.md",
        "evidence/asr-attempts.json",
        "evidence/metrics.json",
        "evidence/evaluation.json",
        "evidence/reference-transcript.txt",
        "evidence/reference-speakers.tsv",
    ]
    result: dict[str, dict[str, Any]] = {}
    for relative in relative_paths:
        path = session / relative
        if path.is_file():
            result[relative] = {"sha256": sha256_file(path), "bytes": path.stat().st_size}
    return result


def finalize_session(
    session: Path, audio_evidence: dict[str, Any], model: Path,
    whisper_meta: dict[str, Any], segments: list[dict[str, Any]], attempts: list[dict[str, Any]],
) -> None:
    write_transcript(session, segments)
    quote_count = draft_documents(session, segments)
    metrics = segment_metrics(segments, attempts)
    write_json(session / "evidence/asr-attempts.json", {"schema": SCHEMA, "attempts": attempts})
    write_json(session / "evidence/metrics.json", metrics)
    manifest = {
        "schema": SCHEMA,
        "carrier_version": VERSION,
        "state": "complete",
        "created_at": now_utc(),
        "consent": {"capture": "explicit", "sharing": "not-granted"},
        "storage": {"kind": "local-files", "network_used": False},
        "claim_kinds": {
            "audio": "observed", "transcript": "observed",
            "speaker_labels": "observed" if metrics["human_reviewed_speaker_segments"] else "unknown",
            "workshop_manual": "declared-source-grounded-draft",
        },
        "audio": audio_evidence,
        "asr": {
            "engine": "whisper-cli", "model_path": str(model),
            "model_bytes": model.stat().st_size, "metadata": whisper_meta,
            "resolved_by": attempts[-1]["mode"],
        },
        "counts": {
            "segments": metrics["segments"],
            "known_speaker_segments": metrics["known_speaker_segments"],
            "unknown_speaker_segments": metrics["unknown_speaker_segments"],
            "source_linked_segments": metrics["source_linked_segments"],
            "draft_excerpts": quote_count,
        },
        "artifacts": artifact_hashes(session),
        "revisions": [],
    }
    write_json(session / "manifest.json", manifest)
    write_json(session / "status.json", {
        "schema": SCHEMA, "state": "complete", "updated_at": now_utc(),
        "next_attention": "Review unknown speakers, overlap, transcript corrections, and sharing consent.",
    })


def transcribe_into_session(
    args: argparse.Namespace, source: Path, session: Path, origin: str,
    session_started: bool = False,
) -> None:
    require_consent(args.consent)
    model = discover_model(args.model)
    if not model:
        raise WorkshopError(
            "whisper-model-missing", "No local Whisper ggml model was found.",
            "Set COHERENCE_WHISPER_MODEL to a local model file, then run ./offline-workshop doctor.",
        )
    if not session_started:
        begin_session(session, origin)
    try:
        audio_target = session / "audio/source.wav"
        audio_evidence = normalize_audio(source, audio_target, args.ffmpeg_bin)
        audio_evidence.update({"sha256": sha256_file(audio_target), "bytes": audio_target.stat().st_size})
        output_stem = session / "transcript/whisper"
        segments, whisper_meta, attempts = transcribe_whisper(
            audio_target, output_stem, args.whisper_bin, model,
            args.language, args.cpu, args.timeout,
        )
        write_json(session / "evidence/asr-attempts.json", {"schema": SCHEMA, "attempts": attempts})
        finalize_session(session, audio_evidence, model, whisper_meta, segments, attempts)
    except WorkshopError as error:
        mark_failure(session, error)
        raise


def read_segments(session: Path) -> list[dict[str, Any]]:
    path = session / "transcript/segments.jsonl"
    try:
        return [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines() if line.strip()]
    except (OSError, json.JSONDecodeError) as exc:
        raise WorkshopError(
            "segments-invalid", f"Could not read transcript evidence at {path}.",
            "Restore the file from the original session evidence or transcribe into a new session.",
        ) from exc


def form_gate(values: list[int], fkwu_bin: str) -> dict[str, Any]:
    repo = Path(__file__).resolve().parents[2]
    core = repo / "form/form-stdlib/core.fk"
    contract = repo / "form/form-stdlib/offline-workshop-session.fk"
    fkwu = executable(fkwu_bin)
    if not fkwu:
        local = repo / "fkwu"
        fkwu = str(local) if local.is_file() and os.access(local, os.X_OK) else None
    if not fkwu or not core.is_file() or not contract.is_file():
        return {"available": False, "kind": "unknown", "verdict": None, "expected": 16383}
    source = core.read_text(encoding="utf-8") + "\n" + contract.read_text(encoding="utf-8")
    source += "\n(ows-session-verdict " + " ".join(str(value) for value in values) + ")\n"
    temporary_name = ""
    try:
        with tempfile.NamedTemporaryFile("w", suffix=".fk", encoding="utf-8", delete=False) as temporary:
            temporary.write(source)
            temporary_name = temporary.name
        result = subprocess.run(
            [fkwu, temporary_name], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
            text=True, timeout=30, check=False,
        )
        output = result.stdout.strip().splitlines()
        verdict = int(output[-1]) if result.returncode == 0 and output else None
        return {
            "available": True, "kind": "observed", "exit_code": result.returncode,
            "verdict": verdict, "expected": 16383, "passed": verdict == 16383 and result.returncode == 0,
        }
    except (OSError, ValueError, subprocess.SubprocessError):
        return {"available": True, "kind": "observed", "verdict": None, "expected": 16383, "passed": False}
    finally:
        if temporary_name:
            try:
                Path(temporary_name).unlink()
            except OSError:
                pass


def verify_session(session: Path, fkwu_bin: str, require_form: bool = True) -> dict[str, Any]:
    errors: list[str] = []
    try:
        manifest = json.loads((session / "manifest.json").read_text(encoding="utf-8"))
        status = json.loads((session / "status.json").read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return {"schema": SCHEMA, "session": str(session), "passed": False, "errors": ["manifest-or-status-invalid"]}
    if manifest.get("schema") != SCHEMA:
        errors.append("schema-mismatch")
    if manifest.get("state") != "complete" or status.get("state") != "complete":
        errors.append("session-not-complete")
    artifacts = manifest.get("artifacts") or {}
    required_artifacts = {
        "audio/source.wav", "transcript/whisper.json", "transcript/segments.jsonl",
        "transcript/transcript.md", "drafts/workshop-manual.md",
        "drafts/book-source-ledger.md", "evidence/asr-attempts.json", "evidence/metrics.json",
    }
    allowed_artifacts = required_artifacts | {
        "audio/capture.wav", "evidence/evaluation.json",
        "evidence/reference-transcript.txt", "evidence/reference-speakers.tsv",
    }
    for relative in sorted(required_artifacts - set(artifacts)):
        errors.append(f"artifact-unlisted:{relative}")
    for relative, expected in artifacts.items():
        if relative not in allowed_artifacts:
            errors.append(f"artifact-path-rejected:{relative}")
            continue
        path = session / relative
        if not path.is_file():
            errors.append(f"artifact-missing:{relative}")
        elif sha256_file(path) != expected.get("sha256"):
            errors.append(f"artifact-hash-mismatch:{relative}")
    try:
        segments = read_segments(session)
    except WorkshopError as error:
        errors.append(error.code)
        segments = []
    counts = manifest.get("counts") or {}
    known = sum(segment.get("speaker") != "unknown" for segment in segments)
    unknown = len(segments) - known
    linked = sum(bool(segment.get("source_ref")) for segment in segments)
    if len(segments) != counts.get("segments"):
        errors.append("segment-count-mismatch")
    if known != counts.get("known_speaker_segments") or unknown != counts.get("unknown_speaker_segments"):
        errors.append("speaker-count-mismatch")
    if linked != counts.get("source_linked_segments"):
        errors.append("source-link-count-mismatch")
    transcript_md = (session / "transcript/transcript.md").read_text(encoding="utf-8") if (session / "transcript/transcript.md").is_file() else ""
    manual_md = (session / "drafts/workshop-manual.md").read_text(encoding="utf-8") if (session / "drafts/workshop-manual.md").is_file() else ""
    ledger_md = (session / "drafts/book-source-ledger.md").read_text(encoding="utf-8") if (session / "drafts/book-source-ledger.md").is_file() else ""
    for segment in segments:
        if f'id="{segment.get("segment_id")}"' not in transcript_md:
            errors.append(f"source-anchor-missing:{segment.get('segment_id')}")
        if f"#{segment.get('segment_id')}" not in ledger_md:
            errors.append(f"ledger-source-link-missing:{segment.get('segment_id')}")
    quote_count = int(counts.get("draft_excerpts") or 0)
    for segment in segments[:quote_count]:
        if f"#{segment.get('segment_id')}" not in manual_md:
            errors.append(f"manual-source-link-missing:{segment.get('segment_id')}")
    audio = session / "audio/source.wav"
    audio_bytes = audio.stat().st_size if audio.is_file() else 0
    if audio.is_file() and manifest.get("audio", {}).get("sha256") != sha256_file(audio):
        errors.append("audio-evidence-hash-mismatch")
    evaluation: dict[str, Any] | None = None
    evaluation_path = session / "evidence/evaluation.json"
    reference_path = session / "evidence/reference-transcript.txt"
    if evaluation_path.is_file():
        try:
            evaluation = json.loads(evaluation_path.read_text(encoding="utf-8"))
            reference_text = reference_path.read_text(encoding="utf-8")
            speaker_path = session / "evidence/reference-speakers.tsv"
            speaker_rows = read_speaker_reference(speaker_path) if speaker_path.is_file() else None
            recomputed = quality_evaluation(segments, reference_text, speaker_rows)
            if evaluation.get("wer") != recomputed.get("wer"):
                errors.append("wer-recompute-mismatch")
            if evaluation.get("wer_counts") != recomputed.get("wer_counts"):
                errors.append("wer-counts-recompute-mismatch")
            if evaluation.get("der") != recomputed.get("der"):
                errors.append("der-recompute-mismatch")
        except (OSError, json.JSONDecodeError, WorkshopError):
            errors.append("quality-evaluation-invalid")
    integrity = 0 if errors else 1
    form_values = [
        1 if manifest.get("consent", {}).get("capture") == "explicit" else 0,
        1 if manifest.get("storage", {}).get("kind") == "local-files" else 0,
        1 if manifest.get("storage", {}).get("network_used") is False else 0,
        1 if manifest.get("state") == "complete" else 0,
        len(segments), known, unknown,
        sum(segment.get("overlap") is True for segment in segments),
        linked, quote_count, audio_bytes,
        min((int(segment.get("start_ms", 0)) for segment in segments), default=0),
        max((int(segment.get("end_ms", 0)) for segment in segments), default=0),
        integrity, 1 if manifest.get("claim_kinds", {}).get("transcript") == "observed" else 0,
        sum(segment.get("speaker_source") == "human-reviewed" for segment in segments),
    ]
    gate = form_gate(form_values, fkwu_bin)
    if require_form and not gate.get("available"):
        errors.append("form-gate-unavailable")
    elif gate.get("available") and not gate.get("passed"):
        errors.append("form-gate-rejected")
    report = {
        "schema": SCHEMA,
        "session": str(session.resolve()),
        "passed": not errors,
        "kind": "observed",
        "errors": errors,
        "counts": {
            "segments": len(segments), "known_speaker_segments": known,
            "unknown_speaker_segments": unknown, "source_linked_segments": linked,
        },
        "form_gate": gate,
        "quality": {
            "wer": evaluation.get("wer") if evaluation else None,
            "der": evaluation.get("der") if evaluation else None,
            "kind": evaluation.get("kind") if evaluation else "unknown",
        },
        "unknowns": {
            "wer": None if evaluation else "unknown until a human reference transcript is supplied",
            "der": (
                None if evaluation and evaluation.get("der") is not None
                else "unknown until time-aligned human speaker labels are supplied"
            ),
            "speaker_segments": unknown,
        },
    }
    write_json(session / "evidence/verification.json", report)
    return report


def relabel_session(args: argparse.Namespace) -> dict[str, Any]:
    require_consent(args.consent)
    session = Path(args.session).expanduser().resolve()
    labels_path = Path(args.labels).expanduser().resolve()
    if not labels_path.is_file():
        raise WorkshopError(
            "labels-missing", f"Speaker label file does not exist: {labels_path}",
            "Create a TSV with segment_id, a tab, and the participant-approved speaker label.",
        )
    labels: dict[str, str] = {}
    for line_number, line in enumerate(labels_path.read_text(encoding="utf-8").splitlines(), 1):
        if not line.strip() or line.startswith("#"):
            continue
        fields = line.split("\t")
        if len(fields) != 2 or not fields[0].strip() or not fields[1].strip():
            raise WorkshopError(
                "labels-invalid", f"Invalid TSV row {line_number}.",
                "Use exactly: segment_id<TAB>speaker label. Comments may begin with #.",
            )
        labels[fields[0].strip()] = fields[1].strip()
        if fields[1].strip().casefold() == "unknown":
            raise WorkshopError(
                "labels-invalid", f"Row {line_number} uses the reserved label 'unknown'.",
                "Omit the row until a participant-approved speaker label is known.",
            )
    segments = read_segments(session)
    valid_ids = {segment["segment_id"] for segment in segments}
    extra = sorted(set(labels) - valid_ids)
    if extra:
        raise WorkshopError(
            "labels-unknown-segment", f"Labels name unknown segments: {', '.join(extra)}",
            "Use segment ids from transcript/segments.jsonl.",
        )
    for segment in segments:
        if segment["segment_id"] in labels:
            segment["speaker"] = labels[segment["segment_id"]]
            segment["speaker_source"] = "human-reviewed"
    manifest_path = session / "manifest.json"
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    attempts = (json.loads((session / "evidence/asr-attempts.json").read_text(encoding="utf-8"))).get("attempts", [])
    write_transcript(session, segments)
    quote_count = draft_documents(session, segments)
    metrics = segment_metrics(segments, attempts)
    write_json(session / "evidence/metrics.json", metrics)
    manifest["claim_kinds"]["speaker_labels"] = "observed" if metrics["human_reviewed_speaker_segments"] else "unknown"
    manifest["counts"] = {
        "segments": metrics["segments"],
        "known_speaker_segments": metrics["known_speaker_segments"],
        "unknown_speaker_segments": metrics["unknown_speaker_segments"],
        "source_linked_segments": metrics["source_linked_segments"],
        "draft_excerpts": quote_count,
    }
    manifest.setdefault("revisions", []).append({
        "kind": "human-reviewed-speaker-labels", "at": now_utc(),
        "labels_sha256": sha256_file(labels_path), "labels_applied": len(labels),
    })
    manifest["artifacts"] = artifact_hashes(session)
    write_json(manifest_path, manifest)
    return verify_session(session, args.fkwu_bin, require_form=not args.no_form)


def evaluate_session(args: argparse.Namespace) -> dict[str, Any]:
    require_consent(args.consent)
    session = Path(args.session).expanduser().resolve()
    reference_path = Path(args.reference_transcript).expanduser().resolve()
    if not reference_path.is_file():
        raise WorkshopError(
            "reference-transcript-missing", f"Reference transcript does not exist: {reference_path}",
            "Provide a participant-reviewed plain-text transcript of the same recording.",
        )
    before = verify_session(session, args.fkwu_bin, require_form=not args.no_form)
    if not before["passed"]:
        raise WorkshopError(
            "session-verification-failed", "The session did not verify before quality evaluation.",
            "Resolve the reported integrity or Form-gate failures before attaching human reference truth.",
        )
    reference_text = reference_path.read_text(encoding="utf-8")
    speaker_rows = None
    speaker_text = None
    if args.speaker_reference:
        speaker_path = Path(args.speaker_reference).expanduser().resolve()
        if not speaker_path.is_file():
            raise WorkshopError(
                "speaker-reference-missing", f"Speaker reference does not exist: {speaker_path}",
                "Provide reviewed start_ms<TAB>end_ms<TAB>speaker rows, or omit --speaker-reference.",
            )
        speaker_text = speaker_path.read_text(encoding="utf-8")
        speaker_rows = read_speaker_reference(speaker_path)
    segments = read_segments(session)
    evaluation = quality_evaluation(segments, reference_text, speaker_rows)
    local_reference = session / "evidence/reference-transcript.txt"
    write_text(local_reference, reference_text)
    evaluation["reference_transcript_sha256"] = sha256_file(local_reference)
    if speaker_text is not None:
        local_speakers = session / "evidence/reference-speakers.tsv"
        write_text(local_speakers, speaker_text)
        evaluation["reference_speakers_sha256"] = sha256_file(local_speakers)
    write_json(session / "evidence/evaluation.json", evaluation)
    metrics_path = session / "evidence/metrics.json"
    metrics = json.loads(metrics_path.read_text(encoding="utf-8"))
    metrics["wer"] = evaluation["wer"]
    metrics["wer_reason"] = None
    metrics["wer_counts"] = evaluation["wer_counts"]
    metrics["der"] = evaluation["der"]
    metrics["der_reason"] = evaluation["der_reason"]
    if evaluation["der"] is not None:
        metrics["der_measure"] = {
            key: evaluation[key]
            for key in (
                "method", "reference_ms", "miss_ms", "false_alarm_ms",
                "speaker_error_ms", "reference_overlap_excluded_ms",
            )
        }
    write_json(metrics_path, metrics)
    manifest_path = session / "manifest.json"
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    manifest["claim_kinds"]["quality_evaluation"] = "observed-against-human-reference"
    manifest.setdefault("revisions", []).append({
        "kind": "human-reference-quality-evaluation", "at": now_utc(),
        "wer": evaluation["wer"], "der": evaluation["der"],
    })
    manifest["artifacts"] = artifact_hashes(session)
    write_json(manifest_path, manifest)
    return verify_session(session, args.fkwu_bin, require_form=not args.no_form)


def capture_audio(args: argparse.Namespace, target: Path) -> dict[str, Any]:
    ffmpeg = executable(args.ffmpeg_bin)
    if not ffmpeg:
        raise WorkshopError(
            "ffmpeg-missing", "ffmpeg is required for microphone capture.",
            "Install ffmpeg and rerun ./offline-workshop doctor.",
        )
    input_format = args.input_format or ("avfoundation" if sys.platform == "darwin" else "pulse")
    input_device = args.input_device or (":0" if sys.platform == "darwin" else "default")
    command = [ffmpeg, "-hide_banner", "-loglevel", "error", "-y", "-f", input_format, "-i", input_device]
    if args.seconds:
        command.extend(["-t", str(args.seconds)])
    command.extend(["-vn", "-ar", "16000", "-ac", "1", "-c:a", "pcm_s16le", str(target)])
    started = time.monotonic()
    try:
        process = subprocess.Popen(command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
        _, _ = process.communicate()
    except KeyboardInterrupt:
        process.send_signal(signal.SIGINT)
        process.wait(timeout=10)
    except OSError as exc:
        raise WorkshopError(
            "capture-start-failed", f"Could not start microphone capture: {exc}",
            "Check ffmpeg microphone permissions and the --input-format/--input-device values.",
        ) from exc
    if process.returncode not in (0, 255) or not target.is_file() or target.stat().st_size <= 44:
        raise WorkshopError(
            "capture-failed", f"Microphone capture failed (exit {process.returncode}).",
            "Grant microphone permission, list ffmpeg input devices, and retry with the correct device.",
        )
    return {
        "kind": "observed", "seconds": round(time.monotonic() - started, 3),
        "input_format": input_format, "input_device": input_device,
    }


def add_common_runtime_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--model", help="Local ggml Whisper model; also read from COHERENCE_WHISPER_MODEL")
    parser.add_argument("--whisper-bin", default="whisper-cli")
    parser.add_argument("--ffmpeg-bin", default="ffmpeg")
    parser.add_argument("--fkwu-bin", default="fkwu")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="offline-workshop",
        description="Consentful, fully local session transcription with source-linked workshop drafts.",
    )
    parser.add_argument("--version", action="version", version=VERSION)
    subparsers = parser.add_subparsers(dest="command", required=True)

    doctor = subparsers.add_parser("doctor", help="Observe local readiness without using the network")
    add_common_runtime_arguments(doctor)

    transcribe = subparsers.add_parser("transcribe", help="Transcribe an existing local recording")
    transcribe.add_argument("--audio", required=True)
    transcribe.add_argument("--session", required=True)
    transcribe.add_argument("--consent", required=True)
    transcribe.add_argument("--language", default="auto")
    transcribe.add_argument("--cpu", action="store_true", help="Skip acceleration and use CPU directly")
    transcribe.add_argument("--timeout", type=int, default=7200)
    add_common_runtime_arguments(transcribe)

    capture = subparsers.add_parser("capture", help="Capture a microphone session, then transcribe locally")
    capture.add_argument("--session", required=True)
    capture.add_argument("--consent", required=True)
    capture.add_argument("--seconds", type=int, help="Capture duration; omit to stop with Ctrl-C")
    capture.add_argument("--input-format", help="ffmpeg input format (macOS: avfoundation; Linux: pulse)")
    capture.add_argument("--input-device", help="ffmpeg input device (macOS example: :0; Linux: default)")
    capture.add_argument("--language", default="auto")
    capture.add_argument("--cpu", action="store_true")
    capture.add_argument("--timeout", type=int, default=7200)
    add_common_runtime_arguments(capture)

    verify = subparsers.add_parser("verify", help="Recompute hashes, counts, links, and the Form trust gate")
    verify.add_argument("--session", required=True)
    verify.add_argument("--fkwu-bin", default="fkwu")
    verify.add_argument("--no-form", action="store_true", help="Allow carrier-only verification outside this repo")

    relabel = subparsers.add_parser("relabel", help="Apply participant-reviewed speaker labels from TSV")
    relabel.add_argument("--session", required=True)
    relabel.add_argument("--labels", required=True)
    relabel.add_argument("--consent", required=True)
    relabel.add_argument("--fkwu-bin", default="fkwu")
    relabel.add_argument("--no-form", action="store_true")

    evaluate = subparsers.add_parser(
        "evaluate", help="Measure WER and optional time-aligned speaker error against human truth",
    )
    evaluate.add_argument("--session", required=True)
    evaluate.add_argument("--reference-transcript", required=True)
    evaluate.add_argument("--speaker-reference")
    evaluate.add_argument("--consent", required=True)
    evaluate.add_argument("--fkwu-bin", default="fkwu")
    evaluate.add_argument("--no-form", action="store_true")
    return parser


def main(argv: Iterable[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(list(argv) if argv is not None else None)
    try:
        if args.command == "doctor":
            report = doctor_report(args)
            print(json.dumps(report, indent=2, sort_keys=True))
            return 0 if report["ready"] else 1
        if args.command == "transcribe":
            session = Path(args.session).expanduser().resolve()
            transcribe_into_session(args, Path(args.audio).expanduser().resolve(), session, "existing-audio")
            report = verify_session(session, args.fkwu_bin, require_form=True)
            print(json.dumps(report, indent=2, sort_keys=True))
            return 0 if report["passed"] else 1
        if args.command == "capture":
            require_consent(args.consent)
            session = Path(args.session).expanduser().resolve()
            begin_session(session, "microphone-capture")
            try:
                capture_path = session / "audio/capture.wav"
                capture_audio(args, capture_path)
                transcribe_into_session(
                    args, capture_path, session, "microphone-capture", session_started=True,
                )
            except WorkshopError as error:
                mark_failure(session, error)
                raise
            report = verify_session(session, args.fkwu_bin, require_form=True)
            print(json.dumps(report, indent=2, sort_keys=True))
            return 0 if report["passed"] else 1
        if args.command == "verify":
            report = verify_session(
                Path(args.session).expanduser().resolve(), args.fkwu_bin,
                require_form=not args.no_form,
            )
            print(json.dumps(report, indent=2, sort_keys=True))
            return 0 if report["passed"] else 1
        if args.command == "relabel":
            report = relabel_session(args)
            print(json.dumps(report, indent=2, sort_keys=True))
            return 0 if report["passed"] else 1
        if args.command == "evaluate":
            report = evaluate_session(args)
            print(json.dumps(report, indent=2, sort_keys=True))
            return 0 if report["passed"] else 1
    except WorkshopError as error:
        print(json.dumps({
            "schema": SCHEMA, "passed": False, "kind": "observed-failure",
            "code": error.code, "message": str(error), "resolution": error.resolution,
        }, indent=2, sort_keys=True), file=sys.stderr)
        return 2
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
