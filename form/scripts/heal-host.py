#!/usr/bin/env python3
"""OS transport for the Form healer: snapshots, bounded argv, compare-and-swap.

No diagnosis, proposal selection, band truth, routing, or learning policy here.
This is a replaceable host carrier, not a second healing runtime.
"""
import fcntl
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
import heal_learning


def digest(path):
    if path.is_symlink():
        return "link:" + os.readlink(path)
    if not path.is_file():
        return "absent"
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def relative(root, text):
    path = Path(text)
    if path.is_absolute() or not text or any(p in ("..", ".git", ".form-heal") for p in path.parts):
        raise ValueError("invalid-relative-source")
    if not (root / path).resolve().is_relative_to(root) or any(p.is_symlink() for p in [root / path, *(root / path).parents] if p != root):
        raise ValueError("source-symlink-refused")
    return path


def prepare(target, band):
    root = Path.cwd().resolve()
    target, band = relative(root, target), relative(root, band)
    if target == band or target.suffix not in (".fk", ".bml") or "tests" in target.parts:
        raise ValueError("repair-needs-production-source-and-separate-band")
    if not (root / target).is_file() or not (root / band).is_file():
        raise ValueError("source-or-band-absent")
    spool = root / ".form-heal"
    spool.mkdir(mode=0o700, exist_ok=True)
    session = Path(tempfile.mkdtemp(prefix="attempt-", dir=spool))
    work = session / "work"
    work.mkdir()
    paths = subprocess.check_output(["git", "ls-files", "-z", "--cached", "--others", "--exclude-standard"]).split(b"\0")
    names = sorted({os.fsdecode(p) for p in paths if p} | {str(target), str(band), "fkwu"})
    manifest = {}
    for name in names:
        src = root / name
        if name.startswith(".form-heal/") or not src.exists():
            continue
        if src.is_dir():
            raise ValueError("submodule-needs-explicit-snapshot:" + name)
        dst = work / name
        dst.parent.mkdir(parents=True, exist_ok=True)
        if src.is_symlink():
            resolved = src.resolve()
            if not resolved.is_relative_to(root):
                raise ValueError("external-symlink-needs-explicit-resource:" + name)
            os.symlink(os.readlink(src), dst)
        else:
            shutil.copy2(src, dst)
        manifest[name] = digest(src)
    data = {"root": str(root), "target": str(target), "band": str(band), "manifest": manifest}
    (session / "snapshot.json").write_text(json.dumps(data))
    shutil.copy2(root / target, session / "original")
    shutil.copy2(root / band, session / "checker")
    (session / "target").write_text(str(target))
    (session / "band").write_text(str(band))
    (session / "attempts.tsv").touch()
    (session / "empty").mkdir()
    print(session)


def load(session):
    session = Path(session).resolve()
    data = json.loads((session / "snapshot.json").read_text())
    root = Path(data["root"])
    if session.parent != root / ".form-heal":
        raise ValueError("session-outside-spool")
    return session, data


def changed(base, manifest, except_path=""):
    return [p for p, value in manifest.items() if p != except_path and digest(base / p) != value]


def reset(session):
    session, data = load(session)
    work = session / "work"
    # Fresh original dependency bytes before EVERY candidate. Tests may have effects.
    for name, value in data["manifest"].items():
        dst, src = work / name, Path(data["root"]) / name
        if digest(src) != value:
            raise ValueError("stale-input:" + name)
        if digest(dst) != value:
            if dst.exists() or dst.is_symlink():
                dst.unlink()
            shutil.copy2(src, dst, follow_symlinks=False)
    for pattern in ("*.fkb", "*.sym", "*.dylib"):
        for path in work.rglob(pattern):
            if str(path.relative_to(work)) not in data["manifest"]:
                path.unlink()
    print("ready")


def run(session, name, seconds, directory, stdin_path, *argv):
    session, data = load(session)
    if not name.replace("-", "").isalnum():
        raise ValueError("invalid-stage-name")
    cwd = {"work": session / "work", "root": Path(data["root"]), "empty": session / "empty"}[directory]
    limit = 0 if float(seconds) == 0 else min(max(float(seconds), 0.1), 600)
    print(heal_trace.execute(session, name, cwd, stdin_path, argv, limit))


def choice(session, stage, identifier, state, offered, selected, reason, result):
    session, _ = load(session)
    heal_trace.decision(session, stage, identifier, state, offered, selected, reason, result)
    print("recorded")


def report(session):
    # Archived evidence is read without reviving its deleted snapshot root.
    session = Path(session).resolve()
    print(heal_trace.report(session, Path.cwd()))


def commit(session, proposal):
    session, data = load(session)
    root, work = Path(data["root"]), session / "work"
    with (root / ".form-heal" / "commit.lock").open("a") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        dirty = changed(root, data["manifest"]) + changed(work, data["manifest"], data["target"])
        if dirty:
            raise ValueError("stale-or-mutated-dependency:" + dirty[0])
        candidate, target = work / data["target"], root / data["target"]
        if candidate.is_symlink() or not candidate.is_file():
            raise ValueError("candidate-not-regular-source")
        if candidate.read_bytes() == (session / "original").read_bytes():
            raise ValueError("candidate-unchanged")
        offered = Path(proposal)
        if offered.parent != session or offered.is_symlink() or candidate.read_bytes() != offered.read_bytes():
            raise ValueError("candidate-changed-during-check")
        fd, tmp = tempfile.mkstemp(prefix=".heal-", dir=target.parent)
        try:
            with os.fdopen(fd, "wb") as stream:
                stream.write(candidate.read_bytes())
                stream.flush()
                os.fsync(stream.fileno())
            os.chmod(tmp, target.stat().st_mode)
            os.replace(tmp, target)
        finally:
            if os.path.exists(tmp):
                os.unlink(tmp)
    print("applied")


def guard(session):
    session, data = load(session)
    dirty = changed(session / "work", data["manifest"], data["target"])
    print("clean" if not dirty else "changed:" + dirty[0])


def hash_file(path):
    print(digest(Path(path)))


def append_bytes(path):
    with open(path, "ab") as stream:
        fcntl.flock(stream, fcntl.LOCK_EX)
        stream.write(sys.stdin.buffer.read(1048576))
    print("appended")


def claim(path):
    with open(path, "x") as stream:
        stream.write("claimed\n")
    print("claimed")


def finish(session):
    session, data = load(session)
    work = session / "work"
    if work.is_symlink():
        raise ValueError("snapshot-root-symlink-refused")
    shutil.rmtree(work)
    print("snapshot-released;evidence-retained")


def learn_prepare(job, route, model, seed, validation):
    round_path, parent = heal_learning.prepare(job, route, model, seed, validation,
                                               sys.stdin.read(24577))
    print(str(round_path) + "\n" + str(parent))


def learn_complete(round_path, status):
    print(heal_learning.complete(round_path, status))


if __name__ == "__main__":
    try:
        {"prepare": prepare, "reset": reset, "run": run, "commit": commit, "guard": guard, "hash": hash_file, "append": append_bytes, "claim": claim, "finish": finish, "choice": choice, "report": report, "learn-prepare": learn_prepare, "learn-complete": learn_complete}[sys.argv[1]](*sys.argv[2:])
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as exc:
        print("refused:" + str(exc))
        sys.exit(1)
