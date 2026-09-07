#!/usr/bin/env python3
"""Evaluation filesystem/process carrier. Form owns cases and repair verdicts.

Each run has a disposable repository, no imported repair memory, and retained
per-case evidence. This carrier neither trains nor issues adapter attestations.
"""
import hashlib
import json
import os
from pathlib import Path
import shutil
import signal
import subprocess
import sys
import tempfile
import time
import heal_trace

ROOT = Path(__file__).resolve().parents[2]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else None


def execute(report, name, cwd, argv, text="", seconds=30):
    input_path = report / (name + ".input")
    input_path.write_text(text)
    try:
        status = heal_trace.execute(report, name, cwd, str(input_path), argv, seconds)
    finally:
        input_path.unlink(missing_ok=True)
    observation = json.loads((report / (name + ".process.json")).read_text())
    observation["exit"] = status
    if status:
        raise RuntimeError(f"{name}: exit={status}; reason={observation['reason']}; evidence={report}")
    return (report / (name + ".out")).read_text(), observation


def manifest(report):
    output, _ = execute(report, "manifest", ROOT,
                        ["./fkwu", "observe/form-cli-heal-eval-manifest.fk"])
    lines = [line for line in output.splitlines() if line.strip() not in ("", "0")]
    if len(lines) != 1:
        raise ValueError("manifest-must-be-one-json-value")
    curriculum = json.loads(lines[0])
    rows = curriculum["cases"]
    if not rows or any(len(row) != 7 for row in rows):
        raise ValueError("invalid-form-curriculum")
    return curriculum


def copy_body(destination):
    names = subprocess.check_output(["git", "ls-files", "-z", "--cached", "--others", "--exclude-standard"], cwd=ROOT).split(b"\0")
    total = 0
    for raw in names:
        name = os.fsdecode(raw)
        source = ROOT / name
        if not name or name.startswith((".form-heal/", ".coherence-network/")):
            continue
        if source.suffix not in (".fk", ".bml", ".py", ".c") or not source.is_file() or source.is_symlink():
            continue
        total += source.stat().st_size
        if total > 268435456:
            raise ValueError("evaluation-source-snapshot-limit")
        target = destination / name
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
    shutil.copy2(ROOT / "fkwu", destination / "fkwu")
    shutil.copy2(ROOT / ".gitignore", destination / ".gitignore")
    subprocess.run(["git", "init", "-q"], cwd=destination, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, timeout=10)


def evaluate(args):
    # This repeats only the public transport shape; Form selects cases, routes,
    # and outcomes. No arbitrary source path or test verdict is an argument.
    mode, model, selected = "deterministic", "", ""
    if args == ["deterministic"] or not args:
        pass
    elif len(args) == 3 and args[0] == "native" and args[1] and args[2]:
        mode, model, selected = args
    else:
        raise ValueError("use deterministic or native MODEL CASE")
    spool = ROOT / ".form-heal" / "evaluations"
    spool.mkdir(parents=True, exist_ok=True)
    report = Path(tempfile.mkdtemp(prefix="eval-", dir=spool))
    memory = ROOT / ".form-heal" / "learned.tsv"
    before_memory = sha(memory)
    observations = []
    summary = {"status": "running", "mode": mode,
               "model": model, "training": "none", "remote_calls": 0,
               "split_scope": "Locally withheld from repair memory; no claim about model pretraining exposure.",
               "route_scope": "Candidate generation plus real Form preflight/check/commit; excludes RAG, git retrieval, and repair-memory replay.",
               "cases": observations}
    try:
        curriculum = manifest(report)
        summary["version"] = curriculum["version"]
        curriculum_path = report / "curriculum.json"
        curriculum_path.write_text(json.dumps(curriculum, indent=2) + "\n")
        summary["curriculum_sha256"] = sha(curriculum_path)
        target, checker = curriculum["target"], curriculum["checker"]
        rows = curriculum["cases"]
        if selected:
            rows = [row for row in rows if row[0] == selected]
            if not rows:
                raise ValueError("unknown-evaluation-case")
        with tempfile.TemporaryDirectory(prefix="body-", dir=report) as directory:
            body = Path(directory)
            copy_body(body)
            for row in rows:
                case = report / row[0]
                case.mkdir()
                (body / target).write_text(row[4])
                (body / checker).write_text(row[5])
                (case / "original.fk").write_text(row[4])
                (case / "checker.fk").write_text(row[5])
                try:
                    output, process = execute(case, "case", body,
                        ["./fkwu", "observe/form-cli-heal-eval-case.bml"],
                        "\n".join([row[0], mode, model]) + "\n", 0 if mode == "native" else 120)
                except Exception:
                    # Preserve unfinished stage events even when the outer bound fires.
                    for job in (body / ".form-heal").glob("attempt-*"):
                        try:
                            outer = json.loads((case / "case.process.json").read_text())
                            stopped = heal_trace.stop_unfinished(job, "outer-evaluation-timeout" if outer["reason"] == "timeout" else "outer-evaluation-failure")
                            (job / "outer-cleanup.json").write_text(json.dumps({"stopped_owned_pids": stopped}) + "\n")
                        finally:
                            shutil.copytree(job, case / job.name, ignore=shutil.ignore_patterns("work"))
                    raise
                refs = [line.split("=", 1)[1] for line in output.splitlines()
                        if line.startswith("evaluation-evidence=")]
                if len(refs) != 1:
                    raise RuntimeError(f"{row[0]}: evaluation did not finish; evidence={case}")
                job = Path(refs[0]).resolve()
                if job.parent != body.resolve() / ".form-heal" or job.is_symlink():
                    raise ValueError("evaluation-evidence-outside-owned-body")
                result = json.loads((job / "evaluation-result.json").read_text())
                if len(result) != 8 or result[:3] != row[:3]:
                    raise ValueError("evaluation-result-contract-mismatch")
                if (body / ".form-heal" / "learned.tsv").exists():
                    raise ValueError("evaluation-leaked-into-repair-memory")
                shutil.copytree(job, case / "evidence")
                # Render again with the permanent archive paths after releasing the body.
                print(heal_trace.report(case / "evidence", ROOT), flush=True)
                heal_trace.report(case, ROOT)
                observation = dict(zip(["id", "split", "family", "outcome", "mode", "model", "baseline_passed", "unchanged"], result))
                observation.update({"seconds": process["seconds"], "original_sha256": sha(case / "original.fk"),
                                    "checker_sha256": sha(case / "checker.fk"), "source_after_sha256": sha(body / target)})
                observations.append(observation)
                (case / "observation.json").write_text(json.dumps(observation, indent=2) + "\n")
                print(f"{row[0]} split={row[1]} outcome={result[3]}", flush=True)
                if result[3] in ("invalid-case", "regression", "refused-input-change"):
                    raise RuntimeError(f"{row[0]}: {result[3]}")
        summary["status"] = "measured"
    except Exception as exc:
        summary["status"], summary["failure"] = "incomplete", str(exc)
        raise
    finally:
        summary["repair_memory_unchanged"] = sha(memory) == before_memory
        if not summary["repair_memory_unchanged"]:
            summary["status"] = "incomplete"
            summary["failure"] = "live-repair-memory-changed-during-evaluation;cause-unattributed"
        summary["counts"] = {name: sum(row["outcome"] == name for row in observations)
                             for name in ("repaired", "unresolved", "preserved")}
        (report / "summary.json").write_text(json.dumps(summary, indent=2) + "\n")
        print("evaluation-report=" + str(report), flush=True)
    if not summary["repair_memory_unchanged"]:
        raise RuntimeError("live-repair-memory-changed-during-evaluation")
    print("heal evaluation measured; training=none; remote=0")


if __name__ == "__main__":
    try:
        evaluate(sys.argv[1:])
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as exc:
        print("heal evaluation incomplete: " + str(exc))
        sys.exit(1)
