"""Read token-count events without copying private token bytes into diagnostics."""
import json

PREFIX = b"form-heal flow "
TEXT = {"phase", "route", "model", "model_path", "base_seal_sha256", "adapter_path", "adapter_sha256", "adapter_state"}
COUNTS = {"input_tokens", "output_tokens", "position", "stamp_ms"}


def parse(line):
    if not line.startswith(PREFIX):
        return None
    try:
        row = json.loads(line[len(PREFIX):])
    except (ValueError, UnicodeError):
        return None
    if not isinstance(row, dict) or set(row) != TEXT | COUNTS:
        return None
    if any(not isinstance(row[k], str) or len(row[k]) > 4096 for k in TEXT):
        return None
    if any(type(row[k]) is not int or row[k] < 0 for k in COUNTS):
        return None
    return row


def totals(rows):
    """Counters are cumulative within a generation; never sum every pulse."""
    latest = {}
    for row in rows:
        key = (row["route"], row["model"], row["model_path"], row["adapter_path"])
        latest[key] = row
    return list(latest.values())
