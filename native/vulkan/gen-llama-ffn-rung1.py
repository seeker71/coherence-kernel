import gguf, numpy as np, struct, sys
BLOB="/Users/ursmuff/.ollama/models/blobs/sha256-74701a8c35f6c8d9a4b91f3f3497643001d63e0c7a84e085bed452548fa88d45"
r=gguf.GGUFReader(BLOB)
T={t.name:t for t in r.tensors}
def deq(name):
    t=T[name]
    a=gguf.quants.dequantize(t.data, t.tensor_type).astype(np.float32)
    # gguf shape is reversed: ne=[in,out]; numpy data is [out,in]
    return a.reshape(list(reversed([int(x) for x in t.shape])))
gate=deq('blk.0.ffn_gate.weight'); up=deq('blk.0.ffn_up.weight'); down=deq('blk.0.ffn_down.weight')
fnorm=deq('blk.0.ffn_norm.weight').reshape(-1)
print("shapes gate",gate.shape,"up",up.shape,"down",down.shape,"fnorm",fnorm.shape)
D=2048; F=8192; EPS=1e-5
rng=np.random.default_rng(7)
x=(rng.standard_normal(D)*0.5).astype(np.float32)
# RMSNorm (f32)
ms=np.float32(np.mean(x.astype(np.float32)**2))
n=(x/np.sqrt(ms+np.float32(EPS))*fnorm).astype(np.float32)
g=(gate@n).astype(np.float32)          # [F]
u=(up@n).astype(np.float32)            # [F]
silu=(g/(np.float32(1.0)+np.exp(-g))).astype(np.float32)
h=(silu*u).astype(np.float32)
y=(down@h).astype(np.float32)          # [D]
def wf(nm,arr): open(nm,'wb').write(arr.astype(np.float32).tobytes())
# row-major: gate[F][D], up[F][D], down[D][F]
wf('r1_gate.bin',gate); wf('r1_up.bin',up); wf('r1_down.bin',down); wf('r1_fnorm.bin',fnorm)
wf('r1_x.bin',x); wf('r1_yref.bin',y)
print("y[0:5]",y[:5],"|y|max",np.abs(y).max())
print("bytes gate",gate.nbytes,"down",down.nbytes)
