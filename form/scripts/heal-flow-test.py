#!/usr/bin/env python3
"""Simulated token producer; real incremental file/timeout observation transport."""
import json
from pathlib import Path
import sys
import tempfile
import threading
import time
import heal_flow
import heal_trace


def main():
    row = dict(phase="decode", route="test-only", model="simulated", model_path="fixture",
               base_seal_sha256="", adapter_path="", adapter_sha256="", adapter_state="base-only",
               input_tokens=4, output_tokens=1, position=4, stamp_ms=1)
    assert heal_flow.parse(heal_flow.PREFIX + json.dumps(row).encode()) == row
    assert heal_flow.parse(heal_flow.PREFIX + json.dumps(dict(row, private="must-not-pass")).encode()) is None
    assert heal_flow.parse(heal_flow.PREFIX + json.dumps(dict(row, output_tokens=-1)).encode()) is None
    with tempfile.TemporaryDirectory(prefix="heal-flow-test-") as directory:
        job = Path(directory)
        code = 'import json,time; row=' + repr(row) + '; print("private-answer-sentinel",flush=True)\nfor n in range(1,4):\n row.update(output_tokens=n,position=3+n); print("form-heal flow "+json.dumps(row),flush=True);time.sleep(.08)\ntime.sleep(3)'
        result = []
        thread = threading.Thread(target=lambda: result.append(heal_trace.execute(job, "stream", job, "-", [sys.executable, "-c", code], .7)))
        thread.start()
        deadline = time.monotonic() + .5
        path = job / "stream.flow.jsonl"
        while time.monotonic() < deadline and (not path.exists() or len(path.read_text().splitlines()) < 3):
            time.sleep(.01)
        assert thread.is_alive(), "stream became visible only after completion"
        rows = [json.loads(line) for line in path.read_text().splitlines()]
        assert [r["output_tokens"] for r in rows] == [1, 2, 3], rows
        assert heal_flow.totals(rows)[0]["output_tokens"] == 3
        thread.join(timeout=3)
        assert result == [124] and not thread.is_alive(), result
        assert "private-answer-sentinel" not in path.read_text()
        assert "private-answer-sentinel" in (job / "stream.out").read_text()
        print(json.dumps(dict(passed=5, simulated_model=True, output_tokens_observed=3,
                             checks=["strict public event schema", "negative count refusal", "events visible before timeout", "cumulative counter not double-counted", "private text retained outside diagnostic stream"])))


if __name__ == "__main__":
    main()
