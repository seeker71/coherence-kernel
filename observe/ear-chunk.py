# ear-chunk.py — the ear organ's one crossing: a long-lived recorder that hears the room in
# short chunks (sox `rec`, 16 kHz mono), transcribes each with the open reference (mlx-whisper
# large-v3-turbo, language auto), translates the two other tongues among en/fa/pt-br with the
# local 3B (mlx_lm, model kept loaded), measures dBFS, and appends one frame per chunk to the
# ear spool the Form door drains. Nothing leaves this Mac. argv: spool chunk_seconds max_chunks
import sys, os, time, glob, math, subprocess, wave, struct
spool, secs, maxn = sys.argv[1], float(sys.argv[2]), int(sys.argv[3])
wrepo = glob.glob(os.path.expanduser('~/.cache/huggingface/hub/models--mlx-community--whisper-large-v3-turbo/snapshots/*'))[0]
lrepo = glob.glob(os.path.expanduser('~/.cache/huggingface/hub/models--mlx-community--Llama-3.2-3B-Instruct-4bit/snapshots/*'))[0]
import mlx_whisper
from mlx_lm import load, generate
model, tok = load(lrepo)
NAMES = {"en": "English", "fa": "Persian", "pt": "Brazilian Portuguese"}
def tr(text, target):
    if not text: return ""
    msgs = [{"role": "user", "content": f"Translate to {NAMES[target]}. Answer with the translation only, one line.\n\n{text}"}]
    p = tok.apply_chat_template(msgs, add_generation_prompt=True)
    return generate(model, tok, prompt=p, max_tokens=80, verbose=False).strip().split("\n")[0]
def dbfs(path):
    with wave.open(path) as w:
        n = w.getnframes(); raw = w.readframes(n)
    if n == 0: return -120
    s = struct.unpack("<%dh" % n, raw); rms = math.sqrt(sum(x * x for x in s) / n)
    return int(round(20 * math.log10(rms / 32768))) if rms >= 1 else -120
os.makedirs(os.path.dirname(spool) or ".", exist_ok=True)
with open(spool, "a") as f:
    f.write("<|ear:frame|>\nt=%d\nkind=born\nms=0\ndb=-120\nnsp=100\nlang=\nen=\nfa=\npt=\n<|/ear:frame|>\n" % int(time.time() * 1000)); f.flush()
n = 0
while n < maxn and not os.path.exists(spool + ".stop"):
    wav = spool + ".chunk.wav"; t0 = time.time()
    subprocess.run(["rec", "-q", "-r", "16000", "-c", "1", "-b", "16", wav, "trim", "0", str(secs)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    db = dbfs(wav)
    r = mlx_whisper.transcribe(wav, path_or_hf_repo=wrepo, fp16=True, condition_on_previous_text=False, no_speech_threshold=0.5)
    lang = r.get("language", ""); text = r.get("text", "").strip().replace("\n", " ")
    segs = r.get("segments", [])
    nsp = (sum(s.get("no_speech_prob", 0.0) for s in segs) / len(segs)) if segs else 1.0
    if db < -55 or nsp > 0.6: text = ""; lang = ""  # near-silence, or the reference itself unsure: say nothing
    out = {"en": "", "fa": "", "pt": ""}
    src = lang if lang in out else ""  # a fourth tongue is heard as itself and offered in all three
    if src: out[src] = text
    for t in ("en", "fa", "pt"):
        if t != src: out[t] = tr(text, t)
    ms = int((time.time() - t0) * 1000)
    with open(spool, "a") as f:
        f.write("<|ear:frame|>\nt=%d\nkind=heard\nms=%d\ndb=%d\nnsp=%d\nlang=%s\nsrc=%s\nen=%s\nfa=%s\npt=%s\n<|/ear:frame|>\n" % (int(time.time() * 1000), ms, db, int(round(nsp * 100)), lang, text, out["en"], out["fa"], out["pt"])); f.flush()
    n += 1
