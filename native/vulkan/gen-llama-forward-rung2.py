import gguf, numpy as np, time, os
BLOB="/Users/ursmuff/.ollama/models/blobs/sha256-74701a8c35f6c8d9a4b91f3f3497643001d63e0c7a84e085bed452548fa88d45"
r=gguf.GGUFReader(BLOB); T={t.name:t for t in r.tensors}
OUT="/private/tmp/claude-501/r2"; os.makedirs(OUT,exist_ok=True)
def deq(name):
    t=T[name]; a=gguf.quants.dequantize(t.data,t.tensor_type).astype(np.float32)
    return a.reshape(list(reversed([int(x) for x in t.shape])))
D,F,NL=2048,8192,16; EPS=np.float32(1e-5)
embd=deq('token_embd.weight')   # [128256, 2048]
onorm=deq('output_norm.weight').reshape(-1)
BOS=128000
x=embd[BOS].astype(np.float32).copy()
def rms(v,w):
    ms=np.float32(np.mean(v.astype(np.float32)**2))
    return (v/np.sqrt(ms+EPS)*w).astype(np.float32)
def silu(g): return (g/(np.float32(1)+np.exp(-g))).astype(np.float32)
t0=time.time()
for L in range(NL):
    an=deq(f'blk.{L}.attn_norm.weight').reshape(-1)
    wv=deq(f'blk.{L}.attn_v.weight'); wo=deq(f'blk.{L}.attn_output.weight')
    fn=deq(f'blk.{L}.ffn_norm.weight').reshape(-1)
    wg=deq(f'blk.{L}.ffn_gate.weight'); wu=deq(f'blk.{L}.ffn_up.weight'); wd=deq(f'blk.{L}.ffn_down.weight')
    # save f32 for device (reused buffers per layer)
    for nm,arr in [('an',an),('wv',wv),('wo',wo),('fn',fn),('wg',wg),('wu',wu),('wd',wd)]:
        arr.astype(np.float32).tofile(f'{OUT}/L{L}_{nm}.bin')
    # numpy reference forward (seq=1, attn out = GQA-expanded V)
    n1=rms(x,an)
    v=(wv@n1).astype(np.float32)            # [512]  (8 kv heads * 64)
    attn=np.empty(D,np.float32)             # [2048] (32 q heads * 64)
    for h in range(32): attn[h*64:h*64+64]=v[(h//4)*64:(h//4)*64+64]
    ao=(wo@attn).astype(np.float32)
    x=(x+ao).astype(np.float32)
    n2=rms(x,fn)
    h_=(silu(wg@n2)*(wu@n2)).astype(np.float32)
    ff=(wd@h_).astype(np.float32)
    x=(x+ff).astype(np.float32)
xf=rms(x,onorm)
logits=(embd@xf).astype(np.float32)   # tied LM head [128256]
onorm.tofile(f'{OUT}/onorm.bin'); embd.astype(np.float32).tofile(f'{OUT}/embd.bin'); x.tofile(f'{OUT}/x0.bin')
np.array([BOS],np.int32).tofile(f'{OUT}/bos.bin')
top=np.argsort(-logits)[:10]
print("forward %.0fs  logits shape"%(time.time()-t0),logits.shape)
print("top-10 token ids:",top.tolist())
print("top-10 logits:",[round(float(logits[i]),3) for i in top])
np.array(top,np.int32).tofile(f'{OUT}/topk.bin'); logits.tofile(f'{OUT}/logits.bin')
print("embd f32 bytes:",embd.nbytes)
