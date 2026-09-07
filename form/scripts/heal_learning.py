"""Artifact and process transport for the Form-selected local learner."""
import fcntl
import hashlib
import json
from pathlib import Path
import re
import struct
import tempfile
import time


def digest(path):
    with Path(path).open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def tensors(path):
    with Path(path).open("rb") as stream:
        length = struct.unpack("<Q", stream.read(8))[0]
        if length > 1048576:
            raise ValueError("adapter-header-limit")
        header = json.loads(stream.read(length))
        rows = {}
        for name, item in header.items():
            if name == "__metadata__":
                continue
            start, end = item["data_offsets"]
            if not 0 <= start < end <= Path(path).stat().st_size - length - 8:
                raise ValueError("adapter-tensor-bounds")
            stream.seek(8 + length + start)
            rows[name] = dict(shape=item["shape"], dtype=item["dtype"],
                              sha256=hashlib.sha256(stream.read(end - start)).hexdigest())
        return rows


def prepare(job, route, model, seed, validation, row):
    job, model, seed, validation = map(lambda p: Path(p).resolve(), (job, model, seed, validation))
    root = Path.cwd().resolve()
    if job.parent != root / ".form-heal" or not (job / "snapshot.json").is_file():
        raise ValueError("learning-job-outside-spool")
    if (job / "evaluation-only").exists():
        raise ValueError("evaluation-excluded-from-training")
    example = json.loads(row)
    if set(example) != {"messages"} or len(example["messages"]) != 2:
        raise ValueError("training-row-shape")
    seed_config = json.loads((seed / "adapter_config.json").read_text())
    if Path(seed_config["model"]).resolve() != model or seed_config.get("fine_tune_type") != "lora":
        raise ValueError("learner-base-adapter-mismatch")
    valid = validation.read_text()
    if not valid.strip() or row.strip() in {line.strip() for line in valid.splitlines()}:
        raise ValueError("training-validation-overlap-or-empty")
    home = root / ".form-heal" / "learning"
    home.mkdir(parents=True, exist_ok=True)
    current = home / "candidate.json"
    parent = seed / "adapters.safetensors"
    if current.exists():
        prior = json.loads(current.read_text())
        if prior["model"] != str(model) or digest(prior["adapter"]) != prior["sha256"]:
            raise ValueError("learner-parent-binding-changed")
        parent = Path(prior["adapter"])
    round_path = Path(tempfile.mkdtemp(prefix="round-", dir=home))
    data = round_path / "data"
    data.mkdir()
    (data / "train.jsonl").write_text(row + "\n")
    (data / "valid.jsonl").write_text(valid)
    # Same validation corpus before/after. This is not a frozen test score.
    (data / "test.jsonl").write_text(valid)
    manifest = dict(round=round_path.name, route=route, job=str(job), model=str(model),
                    parent=str(parent), parent_sha256=digest(parent),
                    seed=str(seed), training_sha256=digest(data / "train.jsonl"),
                    validation_sha256=digest(validation), started_unix_ms=time.time_ns() // 1000000,
                    base_files={p.name: digest(p) for p in sorted(model.glob("*.safetensors"))},
                    model_config_sha256=digest(model / "config.json"),
                    adapter_config_sha256=digest(seed / "adapter_config.json"),
                    objective="observed-round-outcome-next-token-distillation",
                    serving_state="candidate-only;independent-repair-evaluation-required")
    (round_path / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return round_path, parent


def complete(round_path, status):
    round_path = Path(round_path).resolve()
    record = json.loads((round_path / "manifest.json").read_text())
    job = Path(record["job"])
    candidate = round_path / "adapter" / "adapters.safetensors"
    log = (job / (round_path.name + ".out")).read_text(errors="replace")
    counts = re.findall(r"Trained Tokens (\d+)", log)
    process_file = job / (round_path.name + ".process.json")
    process = json.loads(process_file.read_text()) if process_file.is_file() else {}
    numeric_status = int(status) if re.fullmatch(r"[0-9]+", str(status)) else None
    completed = (numeric_status == 0 and process.get("state") == "finished"
                 and process.get("status") == 0 and process.get("returncode") == 0
                 and process.get("cleanup", {}).get("state") == "released")
    record.update(process_status=numeric_status, supervisor_response=str(status),
                  process_completion_verified=completed, ended_unix_ms=time.time_ns() // 1000000,
                  trained_tokens=int(counts[-1]) if counts else None,
                  validation_loss_before_reported=re.findall(r"Val loss ([0-9.eE+-]+)", log),
                  validation_loss_after_reported=re.findall(r"Test loss ([0-9.eE+-]+)", log),
                  loss_precision="trainer-printed-values", state="training-failed", changed_tensors=[])
    if completed and candidate.is_file() and counts and int(counts[-1]) > 0:
        if digest(record["parent"]) != record["parent_sha256"]:
            raise ValueError("learner-parent-weights-changed")
        if any(digest(Path(record["model"]) / name) != value for name, value in record["base_files"].items()):
            raise ValueError("learner-base-weights-changed")
        before, after = tensors(record["parent"]), tensors(candidate)
        if set(before) != set(after) or any(before[k]["shape"] != after[k]["shape"] for k in before):
            raise ValueError("trained-adapter-shape-changed")
        changed = [k for k in before if before[k]["sha256"] != after[k]["sha256"]]
        record.update(adapter=str(candidate), sha256=digest(candidate), changed_tensors=changed,
                      adapter_tensors=after, base_weights_unchanged=True,
                      state="candidate-updated" if changed else "weights-unchanged")
        if changed:
            home = round_path.parent
            with (home / "update.lock").open("a") as lock:
                fcntl.flock(lock, fcntl.LOCK_EX)
                current = home / "candidate.json"
                expected = digest(record["parent"])
                observed = json.loads(current.read_text())["sha256"] if current.exists() else expected
                if observed != record["parent_sha256"] or expected != record["parent_sha256"]:
                    record["state"] = "candidate-retained-parent-conflict"
                else:
                    staged = home / (round_path.name + ".pointer")
                    staged.write_text(json.dumps(record, indent=2) + "\n")
                    staged.replace(current)
    (round_path / "result.json").write_text(json.dumps(record, indent=2) + "\n")
    # Diagnostics carry identities and measured counts, never the training text.
    public = {k: v for k, v in record.items() if k != "adapter_tensors"}
    with (job / "learning.jsonl").open("a") as stream:
        stream.write(json.dumps(public, separators=(",", ":")) + "\n")
    return record["state"]
