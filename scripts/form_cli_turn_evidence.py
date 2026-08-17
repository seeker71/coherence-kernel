#!/usr/bin/env python3
"""Emit privacy-safe measurements for the latest completed Codex turn.

The carrier reads Codex's local thread index and rollout event stream, but it
never emits prompt, reasoning, answer, or tool-output content.  It emits only
identifiers already used for correlation plus counts, byte lengths, durations,
exit status tallies, and provider-reported token usage.  Form owns validation,
kind selection, normalization, and the decision made from this row.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import sqlite3
import sys
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path
from typing import Any, Iterable


SOURCE_KIND = "codex-rollout-jsonl+state-sqlite"
FORM_ROW = re.compile(r"^@form ([^ ]+) (-?[0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)$")


@dataclass(frozen=True)
class ThreadRef:
    thread_id: str
    rollout_path: Path
    model_provider: str
    model: str


@dataclass(frozen=True)
class TurnEvidence:
    tag: str
    thread_id: str
    turn_id: str
    model_provider: str
    model: str
    started_ms: int
    ended_ms: int
    duration_ms: int
    time_to_first_token_ms: int
    final_output_bytes: int
    model_calls: int
    input_tokens: int
    cached_input_tokens: int
    uncached_input_tokens: int
    cache_write_input_tokens: int
    output_tokens: int
    reasoning_output_tokens: int
    total_tokens: int
    tool_calls: int
    tool_outputs: int
    tool_wall_ms: int
    tool_output_bytes: int
    form_commands: int
    form_failures: int
    form_stdout_bytes: int
    form_stderr_bytes: int
    form_shown_bytes: int
    native_commands: int
    native_failures: int
    local_events: int
    remote_events: int
    total_events: int
    patch_events: int
    source_kind: str
    complete: int

    def row(self) -> str:
        values = list(asdict(self).values())
        for value in values:
            if isinstance(value, str) and any(ch in value for ch in "|\r\n"):
                raise ValueError("row text fields must not contain delimiters")
        return "|".join(str(value) for value in values)


def iso_ms(value: str) -> int:
    return int(datetime.fromisoformat(value.replace("Z", "+00:00")).timestamp() * 1000)


def json_bytes(value: Any) -> int:
    if isinstance(value, str):
        return len(value.encode("utf-8"))
    return len(json.dumps(value, ensure_ascii=False, separators=(",", ":")).encode("utf-8"))


def strings_in(value: Any) -> Iterable[str]:
    if isinstance(value, str):
        yield value
    elif isinstance(value, list):
        for item in value:
            yield from strings_in(item)
    elif isinstance(value, dict):
        for item in value.values():
            yield from strings_in(item)


def form_rows(value: Any) -> list[tuple[str, int, int, int, int]]:
    rows: list[tuple[str, int, int, int, int]] = []
    for text in strings_in(value):
        for line in text.splitlines():
            match = FORM_ROW.fullmatch(line)
            if match:
                rows.append(
                    (
                        match.group(1),
                        int(match.group(2)),
                        int(match.group(3)),
                        int(match.group(4)),
                        int(match.group(5)),
                    )
                )
    return rows


def open_readonly(path: Path) -> sqlite3.Connection:
    return sqlite3.connect(f"file:{path}?mode=ro", uri=True)


def find_thread(state_db: Path, cwd: Path, thread_id: str | None) -> ThreadRef:
    with open_readonly(state_db) as db:
        if thread_id:
            row = db.execute(
                "SELECT id, rollout_path, model_provider, COALESCE(model, '') "
                "FROM threads WHERE id = ?",
                (thread_id,),
            ).fetchone()
        else:
            row = db.execute(
                "SELECT id, rollout_path, model_provider, COALESCE(model, '') "
                "FROM threads WHERE cwd = ? AND archived = 0 "
                "ORDER BY updated_at_ms DESC LIMIT 1",
                (str(cwd.resolve()),),
            ).fetchone()
    if not row:
        raise RuntimeError("no active Codex thread found for this workspace")
    return ThreadRef(row[0], Path(row[1]), row[2], row[3])


def read_events(path: Path) -> list[dict[str, Any]]:
    events: list[dict[str, Any]] = []
    with path.open("r", encoding="utf-8") as stream:
        for line_number, line in enumerate(stream, 1):
            try:
                value = json.loads(line)
            except json.JSONDecodeError as exc:
                raise RuntimeError(f"invalid rollout JSON at line {line_number}") from exc
            if isinstance(value, dict):
                events.append(value)
    return events


def latest_completed_segment(
    events: list[dict[str, Any]],
) -> tuple[dict[str, Any], dict[str, Any], list[dict[str, Any]]]:
    starts: dict[str, tuple[int, dict[str, Any]]] = {}
    completed: list[tuple[int, int, dict[str, Any], dict[str, Any]]] = []
    for index, event in enumerate(events):
        payload = event.get("payload", {})
        if event.get("type") != "event_msg" or not isinstance(payload, dict):
            continue
        event_type = payload.get("type")
        turn_id = payload.get("turn_id")
        if event_type == "task_started" and isinstance(turn_id, str):
            starts[turn_id] = (index, event)
        elif event_type == "task_complete" and isinstance(turn_id, str) and turn_id in starts:
            start_index, start_event = starts[turn_id]
            completed.append((start_index, index, start_event, event))
    if not completed:
        raise RuntimeError("no completed turn is available yet")
    start_index, end_index, start_event, complete_event = completed[-1]
    return start_event, complete_event, events[start_index : end_index + 1]


def collect(thread: ThreadRef) -> TurnEvidence:
    start, complete, events = latest_completed_segment(read_events(thread.rollout_path))
    complete_payload = complete["payload"]

    usages: list[dict[str, Any]] = []
    calls: dict[str, dict[str, Any]] = {}
    outputs: dict[str, dict[str, Any]] = {}
    patch_events = 0
    for event in events:
        payload = event.get("payload", {})
        if not isinstance(payload, dict):
            continue
        if event.get("type") == "event_msg":
            if payload.get("type") == "token_count":
                usage = payload.get("info", {}).get("last_token_usage")
                if isinstance(usage, dict):
                    usages.append(usage)
            elif payload.get("type") == "patch_apply_end":
                patch_events += 1
        elif event.get("type") == "response_item":
            payload_type = payload.get("type")
            call_id = payload.get("call_id") or payload.get("id")
            if not isinstance(call_id, str):
                continue
            if payload_type in ("custom_tool_call", "function_call"):
                calls[call_id] = event
            elif payload_type in ("custom_tool_call_output", "function_call_output"):
                outputs[call_id] = event

    token_fields = (
        "input_tokens",
        "cached_input_tokens",
        "cache_write_input_tokens",
        "output_tokens",
        "reasoning_output_tokens",
        "total_tokens",
    )
    token_totals = {field: sum(int(usage.get(field, 0)) for usage in usages) for field in token_fields}
    uncached = token_totals["input_tokens"] - token_totals["cached_input_tokens"]
    if uncached < 0:
        raise RuntimeError("provider token usage reports cached input above total input")

    tool_wall_ms = 0
    tool_output_bytes = 0
    parsed_rows: list[tuple[str, int, int, int, int]] = []
    local_events = 0
    for call_id, call in calls.items():
        output = outputs.get(call_id)
        if output is None:
            continue
        tool_wall_ms += max(0, iso_ms(output["timestamp"]) - iso_ms(call["timestamp"]))
        output_value = output.get("payload", {}).get("output", "")
        tool_output_bytes += json_bytes(output_value)
        rows = form_rows(output_value)
        if rows:
            parsed_rows.extend(rows)
            local_events += sum(1 for kind, *_ in rows if kind != "fkwu")
        else:
            local_events += 1

    form_commands = len(parsed_rows)
    form_failures = sum(1 for _, exit_code, *_ in parsed_rows if exit_code != 0)
    form_stdout = sum(stdout for _, _, stdout, _, _ in parsed_rows)
    form_stderr = sum(stderr for _, _, _, stderr, _ in parsed_rows)
    form_shown = sum(shown for _, _, _, _, shown in parsed_rows)
    native_rows = [row for row in parsed_rows if row[0] == "fkwu"]
    native_commands = len(native_rows)
    native_failures = sum(1 for _, exit_code, *_ in native_rows if exit_code != 0)
    remote_events = len(usages)
    total_events = native_commands + local_events + remote_events

    started_ms = iso_ms(start["timestamp"])
    ended_ms = iso_ms(complete["timestamp"])
    duration_ms = int(round(float(complete_payload.get("duration_ms", ended_ms - started_ms))))
    ttft_ms = int(round(float(complete_payload.get("time_to_first_token_ms", 0))))
    final_output = complete_payload.get("last_agent_message", "")

    return TurnEvidence(
        tag="turn-evidence-v1",
        thread_id=thread.thread_id,
        turn_id=str(complete_payload["turn_id"]),
        model_provider=thread.model_provider,
        model=thread.model,
        started_ms=started_ms,
        ended_ms=ended_ms,
        duration_ms=duration_ms,
        time_to_first_token_ms=ttft_ms,
        final_output_bytes=json_bytes(final_output),
        model_calls=len(usages),
        input_tokens=token_totals["input_tokens"],
        cached_input_tokens=token_totals["cached_input_tokens"],
        uncached_input_tokens=uncached,
        cache_write_input_tokens=token_totals["cache_write_input_tokens"],
        output_tokens=token_totals["output_tokens"],
        reasoning_output_tokens=token_totals["reasoning_output_tokens"],
        total_tokens=token_totals["total_tokens"],
        tool_calls=len(calls),
        tool_outputs=sum(1 for call_id in calls if call_id in outputs),
        tool_wall_ms=tool_wall_ms,
        tool_output_bytes=tool_output_bytes,
        form_commands=form_commands,
        form_failures=form_failures,
        form_stdout_bytes=form_stdout,
        form_stderr_bytes=form_stderr,
        form_shown_bytes=form_shown,
        native_commands=native_commands,
        native_failures=native_failures,
        local_events=local_events,
        remote_events=remote_events,
        total_events=total_events,
        patch_events=patch_events,
        source_kind=SOURCE_KIND,
        complete=1,
    )


def default_codex_home() -> Path:
    configured = os.environ.get("CODEX_HOME")
    return Path(configured) if configured else Path.home() / ".codex"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--codex-home", type=Path, default=default_codex_home())
    parser.add_argument("--cwd", type=Path, default=Path.cwd())
    parser.add_argument("--thread-id")
    parser.add_argument("--format", choices=("row", "json"), default="row")
    args = parser.parse_args()
    try:
        thread = find_thread(args.codex_home / "state_5.sqlite", args.cwd, args.thread_id)
        evidence = collect(thread)
    except (OSError, sqlite3.Error, RuntimeError, ValueError, KeyError) as exc:
        print(f"form-cli-turn-evidence: {exc}", file=sys.stderr)
        return 1
    if args.format == "json":
        sys.stdout.write(json.dumps(asdict(evidence), sort_keys=True, separators=(",", ":")))
    else:
        sys.stdout.write(evidence.row())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
