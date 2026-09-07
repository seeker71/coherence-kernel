#!/usr/bin/env python3
"""Real bounded child, live Form decisions, and conserved observation intervals."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
from unittest.mock import patch
import heal_trace

ROOT = Path(__file__).resolve().parents[2]


def main():
    with tempfile.TemporaryDirectory(prefix="heal-trace-") as directory:
        root = Path(directory).resolve()
        job = root / ".form-heal" / "attempt-trace"
        job.mkdir(parents=True)
        (job / "snapshot.json").write_text(json.dumps({"root": str(root)}))
        driver = ROOT / ".form-heal" / "trace-driver.fk"
        code = 'import time; print("private-content-sentinel", flush=True); print("form-peer stage route=test phase=work-begin status=begin stamp-ms="+str(time.time_ns()//1000000), flush=True); time.sleep(5)'
        driver.write_text('; preludes: form-stdlib/bml/form-cli-heal.bml\n' +
            '(do (let job (read_line)) (fh-run job "timed" 1 "root" "-" (list "python3" "-c" ' + json.dumps(code) + ')))\n')
        result = subprocess.run(["./fkwu", str(driver)], cwd=ROOT, input=str(job) + "\n",
                                text=True, capture_output=True, timeout=30)
        assert result.returncode == 0 and result.stdout.strip().splitlines()[-1] == "124", result.stdout + result.stderr
        assert "timeout timed:" in result.stdout and "choice applied:" in result.stdout
        p = json.loads((job / "timed.process.json").read_text())
        assert p["reason"] == "timeout" and 1000 <= p["elapsed_ms"] < 2500, p
        assert any(e["phase"] == "work-begin" for e in p["events"])
        assert any(a["signal"] == "SIGTERM" and a["result"] == "sent" for a in p["termination"])
        assert any(e["phase"] == "process-reaped" for e in p["events"])
        assert p["events"][-1]["phase"] == "process-cleanup-observed"
        assert p["cleanup"]["state"] == "released" and p["cleanup"]["group_members"] == []
        rendered = heal_trace.report(job, ROOT)
        report = json.loads((job / "timing-report.json").read_text())
        row = report["processes"][0]
        assert row["timing"]["accounted_ms"] == p["elapsed_ms"]
        assert sum(s[3] for s in row["timing"]["spans"]) == p["elapsed_ms"]
        assert [c["state"] for c in report["choices"]] == ["selected", "applied"]
        assert report["choices"][0]["id"] == report["choices"][1]["id"]
        assert "exit=124" == report["choices"][1]["result"]
        assert "private-content-sentinel" not in (job / "timing-report.json").read_text()
        assert "private-content-sentinel" not in (job / "timed.events.jsonl").read_text()
        assert "private-content-sentinel" not in (job / "timing-report.md").read_text()
        # A child clock moving backwards cannot corrupt parent-monotonic intervals.
        argv = ["python3", "-c", 'print("form-peer stage route=test phase=one status=value stamp-ms=9999999999999"); print("form-peer stage route=test phase=two status=value stamp-ms=1")']
        assert heal_trace.execute(job, "clock", root, "-", argv, 1) == 0
        heal_trace.report(job, ROOT)
        report = json.loads((job / "timing-report.json").read_text())
        assert all(r["timing"]["accounted_ms"] == r["process"]["elapsed_ms"] for r in report["processes"])
        # The outer coordinator dies; its child started a separate process group.
        # Durable ownership metadata allows bounded cleanup without that parent.
        orphan = root / "orphan"
        orphan.mkdir()
        child_code = 'import sys,time;sys.path.insert(0,' + repr(str(ROOT / "form/scripts")) + ');import heal_trace;from pathlib import Path;heal_trace.execute(Path(' + repr(str(orphan)) + '),"inner",Path.cwd(),"-",[sys.executable,"-c","import time;time.sleep(30)"],20)'
        assert heal_trace.execute(job, "outer", root, "-", [sys.executable, "-c", child_code], 1) == 124
        started = json.loads((orphan / "inner.process.json").read_text())
        assert started["state"] == "running" and heal_trace.process_identity(started["pid"])
        stopped = heal_trace.stop_unfinished(orphan)
        assert stopped == [started["pid"]] and not heal_trace.group_members(started["pid"])
        settled = json.loads((orphan / "inner.process.json").read_text())
        assert settled["owned_process_absent"] and settled["reason"] == "outer-evaluation-timeout"
        heal_trace.report(orphan, ROOT)
        outer_choices = [json.loads(line) for line in (orphan / "choices.jsonl").read_text().splitlines()]
        assert [c["state"] for c in outer_choices] == ["selected", "applied"]
        assert all(c["actor"] == "evaluation-process-carrier" for c in outer_choices)
        # A real child outlives a denied signal and exits naturally. The OS
        # denial is injected; the exit, files and cleanup inspection are real.
        with patch.object(heal_trace.os, "killpg", side_effect=PermissionError(1, "Operation not permitted")):
            assert heal_trace.execute(job, "denied", root, "-", [sys.executable, "-c", "import time;time.sleep(.4)"], .1) == 124
        denied = json.loads((job / "denied.process.json").read_text())
        assert denied["returncode"] == 0 and denied["cleanup"]["state"] == "released", denied
        assert any(a.get("errno") == 1 for a in denied["termination"]), denied
        assert (job / "denied.status").read_text() == "124"
        print(json.dumps({"passed": 7, "timeout_ms": p["elapsed_ms"], "checks": ["real deadline and process reaping", "Form offered/selected/applied decision correlation", "complete nonoverlapping wall-time accounting", "private output excluded from event and prose reports", "child clock cannot change monotonic accounting", "outer timeout stops the recorded detached child and preserves its timeline", "denied signal preserves actual exit and durable cleanup record"]}))


if __name__ == "__main__":
    main()
