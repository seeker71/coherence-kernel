"""Process observation transport. Form owns intervals and recovery decisions."""
import json
import os
from pathlib import Path
import re
import selectors
import signal
import subprocess
import time
import heal_flow
import heal_dynamic

STAGE = re.compile(rb"^form-peer stage route=([a-zA-Z0-9-]{1,96}) phase=([a-zA-Z0-9-]{1,128}) status=([a-zA-Z0-9-]{1,32}) stamp-ms=([0-9]{1,16})$")


def process_identity(pid):
    result = subprocess.run(["ps", "-p", str(pid), "-o", "lstart="],
                            capture_output=True, text=True, timeout=2)
    return result.stdout.strip() if result.returncode == 0 else ""


def group_members(group):
    rows = subprocess.check_output(["ps", "-eo", "pid,pgid"], text=True, timeout=2).splitlines()[1:]
    return [int(row.split()[0]) for row in rows if len(row.split()) == 2 and int(row.split()[1]) == group]


def append(path, row):
    with path.open("a") as stream:
        stream.write(json.dumps(row, separators=(",", ":")) + "\n")
        stream.flush()


def decision(session, stage, identifier, state, offered, selected, reason, result):
    """Persist a decision already selected by the Form caller, never infer one."""
    options = json.loads(offered)
    if not isinstance(options, list) or selected not in options:
        raise ValueError("selected-action-not-offered")
    if state not in ("selected", "applied"):
        raise ValueError("invalid-decision-state")
    append(session / "choices.jsonl", dict(stage=stage, id=identifier, state=state,
           offered=options, selected=selected, reason=reason, result=result,
           stamp_ms=time.time_ns() // 1000000, actor="form-policy"))


def execute(session, name, cwd, stdin_path, argv, limit):
    start, wall = time.monotonic_ns(), time.time_ns() // 1000000
    events, actions, pending = [], [], {"stdout": b"", "stderr": b""}
    total, reason, status, proc = 0, "completed", 127, None
    code, cleanup = None, {"state": "not-spawned"}
    adaptive = limit == 0

    def elapsed():
        return (time.monotonic_ns() - start) // 1000000

    def event(phase, **fields):
        row = dict(phase=phase, elapsed_ms=elapsed(), **fields)
        events.append(row)
        append(session / (name + ".events.jsonl"), row)

    def send(sig):
        row = dict(signal=sig.name, elapsed_ms=elapsed())
        try:
            if proc.poll() is not None and not group_members(proc.pid):
                result = "group-already-exited"
            else:
                os.killpg(proc.pid, sig)
                result = "sent"
        except ProcessLookupError:
            result = "group-already-exited"
        except OSError as exc:
            # A signal error is evidence. It must not erase the child's exit
            # or abort the final durable record (macOS can return EPERM here).
            result = "signal-error"
            row.update(errno=exc.errno, error=str(exc))
        row["result"] = result
        actions.append(row)
        event("process-signal-observed", **{k: v for k, v in row.items() if k != "elapsed_ms"})
        return result

    watch = heal_dynamic.Watch(Path(__file__).resolve().parents[2], session, name, event, append) if adaptive else None

    # Names belong to one invocation. Never mix a prior invocation's events.
    (session / (name + ".events.jsonl")).write_text("")
    (session / (name + ".flow.jsonl")).write_text("")
    with open(stdin_path if stdin_path != "-" else os.devnull, "rb") as inp, \
            (session / (name + ".out")).open("wb") as out, \
            (session / (name + ".err")).open("wb") as err:
        try:
            event("process-spawn-begin")
            proc = subprocess.Popen(argv, cwd=cwd, stdin=inp, stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE, start_new_session=True)
            # Survives an outer coordinator timeout, before this function can return.
            running = dict(version="observed-process-v1", state="running", argv=list(argv),
                           pid=proc.pid, identity=process_identity(proc.pid),
                           started_monotonic_ns=start, started_unix_ms=wall,
                           budget_ms=round(limit * 1000))
            running["supervision"] = "dynamic-progress" if adaptive else "explicit-deadline"
            (session / (name + ".process.json")).write_text(json.dumps(running) + "\n")
            event("process-spawned", pid=proc.pid)
            stop_at = None
            with selectors.DefaultSelector() as selector:
                selector.register(proc.stdout, selectors.EVENT_READ, ("stdout", out))
                selector.register(proc.stderr, selectors.EVENT_READ, ("stderr", err))
                while selector.get_map() or proc.poll() is None:
                    action = watch.observe(elapsed(), proc.pid) if watch and reason == "completed" and proc.poll() is None else "continue"
                    fixed_stop = not adaptive and (elapsed() >= limit * 1000 or total > 1048576)
                    if reason == "completed" and (fixed_stop or action in ("stop", "control-error")):
                        reason = ("cancelled" if action == "stop" else "dynamic-control-error" if action == "control-error" else "timeout" if elapsed() >= limit * 1000 else "output-limit")
                        event("process-termination-begin", reason=reason)
                        sent = send(signal.SIGTERM)
                        if watch:
                            watch.stopped("SIGTERM:" + sent)
                        stop_at = time.monotonic() + 0.5
                    if stop_at is not None and time.monotonic() >= stop_at:
                        send(signal.SIGKILL)
                        stop_at = None
                    for key, _ in selector.select(0.02):
                        chunk = os.read(key.fd, 65536)
                        label, stream = key.data
                        if not chunk:
                            selector.unregister(key.fileobj)
                            key.fileobj.close()
                            continue
                        total += len(chunk)
                        stream.write(chunk if adaptive else chunk[:max(0, 1048576 - stream.tell())])
                        stream.flush()
                        pending[label] += chunk
                        while b"\n" in pending[label]:
                            line, pending[label] = pending[label].split(b"\n", 1)
                            match = STAGE.fullmatch(line.strip())
                            if match:
                                route, phase, state, stamp = match.groups()
                                event(phase.decode(), route=route.decode(), state=state.decode(),
                                      child_stamp_ms=int(stamp), stream=label)
                                if watch:
                                    watch.progress(elapsed())
                            flow = heal_flow.parse(line)
                            if flow is not None:
                                append(session / (name + ".flow.jsonl"), dict(flow, elapsed_ms=elapsed()))
                                if watch:
                                    watch.progress(elapsed())
                        if len(pending[label]) > 65536:
                            pending[label] = b""  # No private unterminated content in events.
                    if proc.poll() is not None:
                        # A completed leader cannot leave a pipe-holding writer alive.
                        send(signal.SIGKILL)
                        stop_at = None
                code = proc.wait()
                status = (128 - code if code < 0 else code) if reason == "completed" else 130 if reason == "cancelled" else 125 if reason == "dynamic-control-error" else 124
                event("process-reaped", returncode=code)
        except (OSError, ValueError, subprocess.SubprocessError) as exc:
            err.write(str(exc).encode())
            reason = "spawn-or-transport-error"
            status = 125 if adaptive else 127
            event("process-transport-error", error_type=type(exc).__name__)
        finally:
            if proc is not None:
                try:
                    members = group_members(proc.pid)
                    if proc.poll() is None or members:
                        send(signal.SIGKILL)
                    code = proc.wait(timeout=2)
                    until = time.monotonic() + 2
                    members = group_members(proc.pid)
                    while members and time.monotonic() < until:
                        time.sleep(0.02)
                        members = group_members(proc.pid)
                    cleanup = dict(state="released" if not members else "members-remain",
                                   leader_returncode=code, group_members=members)
                except (OSError, subprocess.SubprocessError) as exc:
                    cleanup = dict(state="inspection-or-release-failed", error_type=type(exc).__name__,
                                   error=str(exc), leader_returncode=proc.poll())
                if cleanup["state"] != "released":
                    reason, status = "cleanup-incomplete", 125
                event("process-cleanup-observed", cleanup=cleanup)
    end = elapsed()
    record = dict(version="observed-process-v1", state="finished", argv=list(argv), status=status,
                  reason=reason, seconds=end / 1000, elapsed_ms=end,
                  started_unix_ms=wall, ended_unix_ms=time.time_ns() // 1000000,
                  budget_ms=round(limit * 1000), events=events, termination=actions,
                  supervision="dynamic-progress" if adaptive else "explicit-deadline",
                  timing_basis="parent-monotonic-observation-ms", output_bytes=total,
                  pid=proc.pid if proc is not None else None, returncode=code, cleanup=cleanup)
    (session / (name + ".status")).write_text(str(status))
    (session / (name + ".process.json")).write_text(json.dumps(record, indent=2) + "\n")
    return status


def stop_unfinished(session, reason="outer-evaluation-timeout"):
    """Release recorded children after their coordinator has been stopped.

    PID, process group and start identity all have to match before signalling.
    This is an OS ownership check, not a model-selected recovery policy.
    """
    stopped = []
    for path in session.glob("*.process.json"):
        row = json.loads(path.read_text())
        if row.get("state") != "running":
            continue
        pid = row["pid"]
        identity = process_identity(pid)
        try:
            group = os.getpgid(pid)
        except ProcessLookupError:
            identity, group = "", pid
        if identity and (not row["identity"] or identity != row["identity"] or group != pid):
            raise RuntimeError("unfinished-process-ownership-changed:" + str(pid))
        name = path.name.removesuffix(".process.json")
        event_path = session / (name + ".events.jsonl")
        elapsed = lambda: (time.monotonic_ns() - row["started_monotonic_ns"]) // 1000000
        append(event_path, dict(phase="process-termination-begin", elapsed_ms=elapsed(), reason=reason))
        control = dict(stage=name, id=str(pid) + "-outer-stop", actor="evaluation-process-carrier",
                       offered=["terminate-owned-process-group"], selected="terminate-owned-process-group",
                       reason=reason + ";owned-child-still-unfinished")
        append(session / "choices.jsonl", dict(control, state="selected", result="awaiting-owned-process-exit", stamp_ms=time.time_ns() // 1000000))
        actions = []
        for sig in (signal.SIGTERM, signal.SIGKILL):
            try:
                os.killpg(pid, sig)
                result = "sent"
            except ProcessLookupError:
                result = "group-already-exited"
            actions.append(dict(signal=sig.name, result=result, elapsed_ms=elapsed()))
            if sig == signal.SIGTERM:
                time.sleep(0.1)
        deadline = time.monotonic() + 2
        while group_members(pid) and time.monotonic() < deadline:
            time.sleep(0.02)
        gone = not group_members(pid)
        if not gone:
            raise RuntimeError("unfinished-process-still-present:" + str(pid))
        append(event_path, dict(phase="process-exit-observed", elapsed_ms=elapsed()))
        end = elapsed()
        status = 124 if reason == "outer-evaluation-timeout" else 1
        row.update(state="externally-stopped", status=status, reason=reason, status_basis="coordinator-termination-policy",
                   seconds=end / 1000, elapsed_ms=end, ended_unix_ms=time.time_ns() // 1000000,
                   events=[json.loads(line) for line in event_path.read_text().splitlines()],
                   termination=actions, timing_basis="parent-monotonic-observation-ms",
                   owned_process_absent=True, owned_group_empty=True,
                   output_bytes=sum(p.stat().st_size for p in (session / (name + ".out"), session / (name + ".err")) if p.exists()))
        path.write_text(json.dumps(row, indent=2) + "\n")
        (session / (name + ".status")).write_text(str(status))
        append(session / "choices.jsonl", dict(control, state="applied", result="owned-process-exit-observed", stamp_ms=time.time_ns() // 1000000))
        stopped.append(pid)
    return stopped


def report(session, root):
    """Read retained evidence; use the executable BML interval authority."""
    records, all_choices = [], []
    choices = session / "choices.jsonl"
    if choices.exists():
        all_choices = [json.loads(line) for line in choices.read_text().splitlines() if line]
    for path in sorted(session.glob("*.process.json")):
        record = json.loads(path.read_text())
        stage = path.name.removesuffix(".process.json")
        if record.get("version") != "observed-process-v1":
            continue  # Legacy evidence is never represented as a fresh measured trace.
        if record.get("state") == "running":
            raise ValueError("process-still-running:" + stage)
        request = dict(elapsed_ms=record["elapsed_ms"],
                       events=[[e["phase"], e["elapsed_ms"]] for e in record["events"]])
        result = subprocess.run([str(root / "fkwu"), "observe/form-cli-heal-timing-run.fk"],
                                cwd=root, input=json.dumps(request) + "\n", text=True,
                                capture_output=True, timeout=30, check=True)
        lines = [line for line in result.stdout.splitlines() if line.startswith("{")]
        if len(lines) != 1:
            raise ValueError("timing-authority-did-not-return-one-report")
        timing = json.loads(lines[0])
        if timing["valid"] != 1 or timing["accounted_ms"] != record["elapsed_ms"]:
            raise ValueError("process-time-did-not-reconcile:" + stage)
        public = {key: value for key, value in record.items() if key != "argv"}
        public["program"] = Path(record["argv"][0]).name
        flow_path = session / (stage + ".flow.jsonl")
        flows = [json.loads(line) for line in flow_path.read_text().splitlines()] if flow_path.exists() else []
        records.append(dict(stage=stage, process=public, timing=timing,
                            token_flows=heal_flow.totals(flows), token_events=len(flows)))
    records.sort(key=lambda r: r["process"]["started_unix_ms"])
    if not records:
        raise ValueError("recorded-process-timeline-absent;fresh-run-required")
    summary = dict(version="heal-observation-v1", generated_unix_ms=time.time_ns() // 1000000,
                   evidence=str(session), processes=records, choices=all_choices)
    learning_path = session / "learning.jsonl"
    summary["learning_rounds"] = [json.loads(line) for line in learning_path.read_text().splitlines()] if learning_path.exists() else []
    summary["partial_outputs"] = [dict(path=str(path), bytes=path.stat().st_size) for path in sorted(session.glob("*.reply.partial"))]
    (session / "timing-report.json").write_text(json.dumps(summary, indent=2) + "\n")
    lines = ["# Observed healing run", "", "Times are elapsed wall time between parent-observed events, including waiting; they are not CPU measurements.", ""]
    for row in records:
        p = row["process"]
        supervision = "dynamic progress supervision" if p.get("supervision") == "dynamic-progress" else f"deadline {p['budget_ms']} ms"
        lines.append(f"- {row['stage']}: {p['elapsed_ms']} ms; {p['reason']}; exit {p['status']}; {supervision}.")
        for flow in row["token_flows"]:
            lines.append(f"  Token flow: {flow['route']} / {flow['model']}; input={flow['input_tokens']}; output={flow['output_tokens']}; adapter={flow['adapter_state']}; last event={flow['phase']} at {flow['elapsed_ms']} ms.")
        if p["reason"] in ("timeout", "outer-evaluation-timeout"):
            lines.append("  Stage totals: " + "; ".join(f"{name}={ms} ms" for name, ms in row["timing"]["families"] if ms))
            lines.extend(["", "| Last observed stage | Start ms | End ms | Elapsed ms |", "|---|---:|---:|---:|"])
            lines.extend(f"| {name} | {start} | {end} | {duration} |" for name, start, end, duration in row["timing"]["spans"])
            lines.append("")
    lines.extend(["", "## Recorded policy choices", ""])
    for c in all_choices:
        lines.append(f"- {c['stage']} [{c['id']}] {c['state']}: offered {', '.join(c['offered'])}; selected {c['selected']}; reason {c['reason']}; result {c['result']}.")
    lines.extend(["", "## Observed learning rounds", ""])
    for learning in summary["learning_rounds"]:
        lines.append(f"- {learning['round']}: {learning['state']}; model={learning['model']}; trained tokens={learning['trained_tokens']}; changed tensors={len(learning['changed_tensors'])}; {learning['serving_state']}.")
    (session / "timing-report.md").write_text("\n".join(lines) + "\n")
    timeouts = [r for r in records if r["process"]["reason"] in ("timeout", "outer-evaluation-timeout")]
    notice = [f"timing-report={session / 'timing-report.json'}; processes={len(records)}; timeouts={len(timeouts)}; choice-events={len(all_choices)}"]
    for row in timeouts:
        notice.append(f"timeout {row['stage']}: elapsed={row['process']['elapsed_ms']}ms; " +
                      "; ".join(f"{name}={duration}ms" for name, duration in row["timing"]["families"] if duration))
        notice.extend(f"choice {c['state']}: offered={','.join(c['offered'])}; selected={c['selected']}; result={c['result']}"
                      for c in all_choices if c["stage"] in (row["stage"], "evaluation-outcome"))
    return "\n".join(notice)
