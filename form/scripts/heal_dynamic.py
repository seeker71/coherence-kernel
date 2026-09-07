"""Observe a live child and apply the decision returned by Form."""
import json
from pathlib import Path
import subprocess
import time


class Watch:
    def __init__(self, root, session, stage, event, append):
        self.root, self.session, self.stage = Path(root), session, stage
        self.event, self.append = event, append
        self.progress_ms, self.gap_ms, self.next_ms, self.identifier = 0, 1, 1000, 0

    def progress(self, elapsed):
        self.gap_ms = max(1, elapsed - self.progress_ms)
        self.progress_ms = elapsed

    def observe(self, elapsed, pid):
        control_path = self.session / (self.stage + ".control")
        control = control_path.read_text().strip() if control_path.exists() else ""
        if elapsed < self.next_ms and control != "stop":
            return "continue"
        self.identifier += 1
        request = dict(id=self.identifier, quiet_ms=elapsed - self.progress_ms,
                       progress_gap_ms=self.gap_ms, control=control)
        self.event("dynamic-observation", observation=request)
        result = subprocess.run([str(self.root / "fkwu"), "observe/form-cli-heal-dynamic-run.fk"],
                                cwd=self.root, input=json.dumps(request) + "\n", text=True,
                                capture_output=True, timeout=30)
        rows = [json.loads(line) for line in result.stdout.splitlines() if line.startswith("{")]
        if result.returncode or len(rows) != 1 or rows[0].get("id") != self.identifier or rows[0].get("correlated") != 1:
            self.event("dynamic-control-refused", status=result.returncode)
            # A broken instrument cannot silently become an unlimited permit.
            return "control-error"
        decision = rows[0]
        action = decision["action"]
        if action not in ("continue", "inspect", "stop"):
            return "control-error"
        row = dict(stage=self.stage, id=f"dynamic-{self.identifier}", actor="form-policy",
                   offered=["continue", "inspect", "stop"], selected=action,
                   reason="observed-progress-and-explicit-control", observation=request,
                   stamp_ms=time.time_ns() // 1000000)
        self.append(self.session / "choices.jsonl", dict(row, state="selected", result="awaiting-action"))
        evidence = "reading-continues"
        if action == "inspect":
            probe = subprocess.run(["ps", "-p", str(pid), "-o", "pid=,stat=,time=,rss="],
                                   capture_output=True, text=True, timeout=2)
            evidence = probe.stdout.strip() or "process-exited-before-inspection"
            self.event("dynamic-process-inspection", process=evidence,
                       basis="pid-state-cumulative-cpu-time-rss;not-semantic-progress")
        if action != "stop":
            self.append(self.session / "choices.jsonl", dict(row, state="applied", result=evidence))
        else:
            self.stop_row = row  # Transport records application after signalling.
        self.next_ms = elapsed + decision["next_observation_ms"]
        return action

    def stopped(self, result):
        if hasattr(self, "stop_row"):
            self.append(self.session / "choices.jsonl", dict(self.stop_row, state="applied", result=result,
                        stamp_ms=time.time_ns() // 1000000))
