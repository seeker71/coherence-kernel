# ear-carrier.py — the ear organ's live lane, a stream. A continuous recorder (sox `rec`, 16 kHz
# mono, raw on a pipe) delivers 20 ms blocks; each arrival wakes the live decoder (whisper-tiny,
# ~20 ms warm) which offers the current line — audio since the last commit, at most 4 s — and
# writes a frame, ringing the door's bell. No hop, no sleep: the lane paces at the decoder's own
# speed. When the speaker pauses (600 ms under the level line) or the line reaches 4 s, the
# segment's audio is handed to the commit lane (observe/ear-commit-carrier.py, its own process, so the
# reference and the 3B never stand on this path). argv: spool run_seconds. Nothing leaves this Mac.
import sys, os, time, glob, math, subprocess, threading, collections, wave
import numpy as np
spool, run_s = sys.argv[1], float(sys.argv[2])
bell, stopf, segdir, seglist, segbell = spool + ".bell", spool + ".stop", spool + ".seg", spool + ".segments", spool + ".seg.bell"
snap = lambda name: glob.glob(os.path.expanduser('~/.cache/huggingface/hub/models--mlx-community--' + name + '/snapshots/*'))[0]
tiny = snap('whisper-tiny')
import mlx_whisper
LEVEL, PAUSE, SEGMAX, RATE = -55, 0.5, 4.0, 16000
def ring(path):
    for _ in range(12):  # the door may be painting; catch it at the bell within ~60 ms
        try:
            fd = os.open(path, os.O_WRONLY | os.O_NONBLOCK); os.write(fd, b"x"); os.close(fd); return
        except OSError:
            time.sleep(0.005)
def emit(**fields):
    body = "".join("%s=%s\n" % (k, str(v).replace("\n", " ")) for k, v in fields.items())
    with open(spool, "a") as f:
        f.write("<|ear:frame|>\nt=%d\n%s<|/ear:frame|>\n" % (int(time.time() * 1000), body)); f.flush()
    ring(bell)
def unloop(text):
    # the tiny model loops on faint room sound ("a little bit of a little bit of ..."): keep the line up to its first repeated four words
    w = text.split()
    for i in range(len(w) - 3):
        if w[i:i + 4] == w[i + 4:i + 8]: return " ".join(w[:i + 4]) + " …"
    return text
def dbfs(a):
    rms = float(np.sqrt(np.mean(a * a))) if a.size else 0.0
    return int(round(20 * math.log10(rms))) if rms > 1e-6 else -120
warm = (np.random.randn(RATE) * 0.001).astype(np.float32); mlx_whisper.transcribe(warm, path_or_hf_repo=tiny, fp16=True)
os.makedirs(segdir, exist_ok=True)
blocks, lock, arrived = collections.deque(), threading.Lock(), threading.Event()
rec = subprocess.Popen(["rec", "-q", "-r", str(RATE), "-c", "1", "-b", "16", "-e", "signed-integer", "-t", "raw", "-"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
def reader():
    while True:
        raw = rec.stdout.read(640)  # 20 ms
        if not raw: break
        with lock: blocks.append((np.frombuffer(raw, dtype=np.int16).astype(np.float32) / 32768.0, time.time()))
        arrived.set()
threading.Thread(target=reader, daemon=True).start()
emit(kind="born", db=-120, live="", livelang="", livems=0)
seg, seg_speech, last_voice, t_start, last_quiet, nseg = [], False, 0.0, time.time(), 0.0, 0
while time.time() - t_start < run_s and not os.path.exists(stopf):
    arrived.wait(1.0); arrived.clear()
    with lock:
        fresh = list(blocks); blocks.clear()
    if not fresh: continue
    seg.extend(fresh); t_end = fresh[-1][1]; now = time.time()
    db = dbfs(np.concatenate([b for b, _ in seg[-15:]]))
    if db > LEVEL: seg_speech, last_voice = True, now
    seg_len = len(seg) * 0.02
    if seg_speech and seg_len >= 0.3:
        audio = np.concatenate([b for b, _ in seg[-200:]])
        r = mlx_whisper.transcribe(audio, path_or_hf_repo=tiny, fp16=True, condition_on_previous_text=False, temperature=0.0, compression_ratio_threshold=None, logprob_threshold=None, no_speech_threshold=None)
        emit(kind="live", db=db, live=unloop(r.get("text", "").strip()), livelang=r.get("language", ""), livems=int((time.time() - t_end) * 1000))
        if (now - last_voice > PAUSE) or seg_len >= SEGMAX:
            nseg += 1; path = "%s/%d-%d.wav" % (segdir, int(t_end * 1000), nseg)
            with wave.open(path, "wb") as w:
                w.setnchannels(1); w.setsampwidth(2); w.setframerate(RATE); w.writeframes((audio * 32767).astype(np.int16).tobytes())
            with open(seglist, "a") as f: f.write("%s %d\n" % (path, int(t_end * 1000)))
            ring(segbell)
            seg, seg_speech = [], False
    elif not seg_speech:
        seg = seg[-50:]  # keep one second of room before speech starts
        if now - last_quiet > 1.0:
            last_quiet = now; emit(kind="quiet", db=db, live="", livelang="", livems=0)
emit(kind="done", db=-120, live="", livelang="", livems=0); rec.terminate()
