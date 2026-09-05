# ear-commit-carrier.py — the ear organ's commit lane, its own process. Woken by the segment bell, it
# takes each segment the live lane handed over, lets the open reference (large-v3-turbo, ~400 ms
# warm) commit the line with its tongue and its own no-speech doubt, then offers the two tongues
# not heard through the local 3B (~150 ms a line), writing a frame after each step and ringing
# the door's bell. argv: spool run_seconds. Nothing leaves this Mac.
import sys, os, time, glob
import numpy as np
spool, run_s = sys.argv[1], float(sys.argv[2])
bell, stopf, seglist, segbell = spool + ".bell", spool + ".stop", spool + ".segments", spool + ".seg.bell"
snap = lambda name: glob.glob(os.path.expanduser('~/.cache/huggingface/hub/models--mlx-community--' + name + '/snapshots/*'))[0]
turbo, llama = snap('whisper-large-v3-turbo'), snap('Llama-3.2-3B-Instruct-4bit')
import mlx_whisper
from mlx_lm import load, generate
model, tok = load(llama)
NAMES = {"en": "English", "fa": "Persian", "pt": "Brazilian Portuguese"}
def ring(path):
    for _ in range(12):
        try:
            fd = os.open(path, os.O_WRONLY | os.O_NONBLOCK); os.write(fd, b"x"); os.close(fd); return
        except OSError:
            time.sleep(0.005)
def emit(**fields):
    body = "".join("%s=%s\n" % (k, str(v).replace("\n", " ")) for k, v in fields.items())
    with open(spool, "a") as f:
        f.write("<|ear:frame|>\nt=%d\n%s<|/ear:frame|>\n" % (int(time.time() * 1000), body)); f.flush()
    ring(bell)
def tr(text, target):
    msgs = [{"role": "user", "content": "Translate to %s. Answer with the translation only, one line.\n\n%s" % (NAMES[target], text)}]
    return generate(model, tok, prompt=tok.apply_chat_template(msgs, add_generation_prompt=True), max_tokens=80, verbose=False).strip().split("\n")[0]
warm = (np.random.randn(16000) * 0.001).astype(np.float32); mlx_whisper.transcribe(warm, path_or_hf_repo=turbo, fp16=True); tr("hello", "fa")
mark, t_start = (os.path.getsize(seglist) if os.path.exists(seglist) else 0), time.time()
S = dict(heard="", lang="", nsp=100, heardms=0, en="", fa="", pt="", saidms=0)
while time.time() - t_start < run_s + 10 and not os.path.exists(stopf):
    size = os.path.getsize(seglist) if os.path.exists(seglist) else 0
    if size <= mark:
        try:
            with open(segbell) as b: b.read()  # blocks at the bell until the live lane rings
        except OSError:
            time.sleep(0.05)
        continue
    with open(seglist) as f:
        f.seek(mark); lines = f.read(size - mark); mark = size
    for line in lines.strip().split("\n"):
        path, t_end = line.split(" "); t_end = int(t_end) / 1000.0
        r = mlx_whisper.transcribe(path, path_or_hf_repo=turbo, fp16=True, condition_on_previous_text=False, temperature=0.0, compression_ratio_threshold=None, logprob_threshold=None, no_speech_threshold=None)  # greedy, no retries: one pass is the whole cost
        segs = r.get("segments", []); nsp = (sum(s.get("no_speech_prob", 0.0) for s in segs) / len(segs)) if segs else 1.0
        text = r.get("text", "").strip()
        if not text or nsp > 0.6:
            os.remove(path); continue
        S.update(heard=text, lang=r.get("language", ""), nsp=int(round(nsp * 100)), heardms=int((time.time() - t_end) * 1000)); emit(kind="heard", **S)
        src = S["lang"] if S["lang"] in NAMES else ""
        for t in NAMES: S[t] = text if t == src else tr(text, t)
        S["saidms"] = int((time.time() - t_end) * 1000); emit(kind="said", **S)
        os.remove(path)
