#!/usr/bin/env python3
"""Real subprocess liveness, adaptive Form decisions, and explicit cancellation."""
import json
from pathlib import Path
import sys
import tempfile
import threading
import time
import heal_trace


def main():
    with tempfile.TemporaryDirectory(prefix="heal-dynamic-test-") as directory:
        job = Path(directory)
        assert heal_trace.execute(job, "quick", job, "-", [sys.executable, "-c", "print('done')"], 0) == 0
        quick = json.loads((job / "quick.process.json").read_text())
        assert quick["elapsed_ms"] < 1000
        code = 'import time\nfor n in range(15):\n print("form-peer stage route=test phase=progress-"+str(n)+" status=value stamp-ms="+str(time.time_ns()//1000000),flush=True);time.sleep(.12)'
        assert heal_trace.execute(job, "progress", job, "-", [sys.executable, "-c", code], 0) == 0
        long = json.loads((job / "progress.process.json").read_text())
        assert long["elapsed_ms"] > 1500 and long["reason"] == "completed"
        assert long["supervision"] == "dynamic-progress"
        choices = [json.loads(line) for line in (job / "choices.jsonl").read_text().splitlines()]
        assert any(c["selected"] == "continue" for c in choices), choices
        result = []
        thread = threading.Thread(target=lambda: result.append(heal_trace.execute(job, "cancel", job, "-", [sys.executable, "-c", "import time;time.sleep(10)"], 0)))
        thread.start()
        time.sleep(.25)
        (job / "cancel.control").write_text("stop\n")
        thread.join(timeout=5)
        assert result == [130] and not thread.is_alive(), result
        cancelled = json.loads((job / "cancel.process.json").read_text())
        assert cancelled["reason"] == "cancelled"
        choices = [json.loads(line) for line in (job / "choices.jsonl").read_text().splitlines()]
        stop = [c for c in choices if c["stage"] == "cancel"]
        assert [c["state"] for c in stop] == ["selected", "applied"] and all(c["selected"] == "stop" for c in stop)
        print(json.dumps(dict(passed=4, quick_ms=quick["elapsed_ms"], progressing_ms=long["elapsed_ms"],
                             cancelled_ms=cancelled["elapsed_ms"], checks=["quick work ends immediately", "progress continues past observation interval", "Form choices are correlated and applied", "explicit cancellation signals and reaps owned process"])))


if __name__ == "__main__":
    main()
