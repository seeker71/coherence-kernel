# stt-oracle.py — the one host crossing of the STT lane: the open-source reference
# (mlx-whisper, whisper-large-v3-turbo, weights cached on this Mac) transcribes one WAV
# and answers name=value lines the Form door reads: language, text, segments, latency.
# Everything around it — capture, level, translation through the hearth, glass, rows — is Form.
import sys, time, json, glob
wav = sys.argv[1]; lang = sys.argv[2] if len(sys.argv) > 2 and sys.argv[2] else None
repo = glob.glob('/Users/ursmuff/.cache/huggingface/hub/models--mlx-community--whisper-large-v3-turbo/snapshots/*')[0]
import mlx_whisper
t0 = time.time()
r = mlx_whisper.transcribe(wav, path_or_hf_repo=repo, language=lang, fp16=True, condition_on_previous_text=False)
ms = int((time.time() - t0) * 1000)
print("language=" + str(r.get("language")))
print("latency-ms=" + str(ms))
print("segments=" + str(len(r.get("segments", []))))
print("text=" + r.get("text", "").strip().replace("\n", " "))
