import gguf,numpy as np,re,json
BLOB="/Users/ursmuff/.ollama/models/blobs/sha256-74701a8c35f6c8d9a4b91f3f3497643001d63e0c7a84e085bed452548fa88d45"
r=gguf.GGUFReader(BLOB);F={f:r.fields[f] for f in r.fields};T={t.name:t for t in r.tensors}
def deq(n):
    t=T[n];a=gguf.quants.dequantize(t.data,t.tensor_type).astype(np.float32);return a.reshape(list(reversed([int(x) for x in t.shape])))
# ---- tokenizer (byte-level BPE) ----
def rdstr_at(field,i):
    return bytes(field.parts[field.data[i]]).decode('utf-8',errors='replace')
toksf=F['tokenizer.ggml.tokens']; vocab=[rdstr_at(toksf,i) for i in range(len(toksf.data))]
tok2id={t:i for i,t in enumerate(vocab)}
mergesf=F['tokenizer.ggml.merges']; merges=[rdstr_at(mergesf,i) for i in range(len(mergesf.data))]
bpe_rank={tuple(m.split(' ')):i for i,m in enumerate(merges)}
# GPT2 bytes<->unicode
def bytes_to_unicode():
    bs=list(range(33,127))+list(range(161,173))+list(range(174,256)); cs=bs[:]; n=0
    for b in range(256):
        if b not in bs: bs.append(b); cs.append(256+n); n+=1
    return dict(zip(bs,[chr(c) for c in cs]))
b2u=bytes_to_unicode(); u2b={v:k for k,v in b2u.items()}
def bpe(token):
    word=list(token); 
    if len(word)<2: return word
    while True:
        pairs=[(word[i],word[i+1]) for i in range(len(word)-1)]
        rp=min(pairs,key=lambda p:bpe_rank.get(p,1e18))
        if rp not in bpe_rank: break
        a,b=rp; nw=[]; i=0
        while i<len(word):
            if i<len(word)-1 and word[i]==a and word[i+1]==b: nw.append(a+b); i+=2
            else: nw.append(word[i]); i+=1
        word=nw
        if len(word)==1: break
    return word
PAT=re.compile(r"'s|'t|'re|'ve|'m|'ll|'d| ?[A-Za-z]+| ?[0-9]+| ?[^\sA-Za-z0-9]+|\s+")
def encode(text,add_bos=True):
    ids=[128000] if add_bos else []
    for piece in PAT.findall(text):
        bb=piece.encode('utf-8'); chars=''.join(b2u[c] for c in bb)
        for w in bpe(chars): ids.append(tok2id[w])
    return ids
def decode(ids):
    s=''.join(vocab[i] for i in ids if i<128000)
    bb=bytes(u2b[c] for c in s if c in u2b); return bb.decode('utf-8',errors='replace')
# ---- model ----
D,NH,NKV,HD,NL,EPS,BASE=2048,32,8,64,16,np.float32(1e-5),500000.0
embd=deq('token_embd.weight'); onorm=deq('output_norm.weight').reshape(-1)
inv_freq=BASE**(-np.arange(0,HD,2)/HD)  # (32,)
def rope(x,pos): # x: (heads, HD) interleaved-pair rotation
    out=x.copy().astype(np.float32)
    ang=pos*inv_freq  # (32,)
    c=np.cos(ang).astype(np.float32); s=np.sin(ang).astype(np.float32)
    x0=x[:,0::2]; x1=x[:,1::2]
    out[:,0::2]=x0*c - x1*s; out[:,1::2]=x0*s + x1*c
    return out
def rms(v,w):
    ms=np.float32(np.mean(v.astype(np.float32)**2));return (v/np.sqrt(ms+EPS)*w).astype(np.float32)
def silu(g):return (g/(np.float32(1)+np.exp(-g))).astype(np.float32)
W={}
for L in range(NL):
    for nm in ['attn_norm','attn_q','attn_k','attn_v','attn_output','ffn_norm','ffn_gate','ffn_up','ffn_down']:
        W[(L,nm)]=deq(f'blk.{L}.{nm}.weight')
def gen(prompt,nnew=12):
    ids=encode(prompt); kc=[[] for _ in range(NL)]; kv=[[] for _ in range(NL)]; gen_ids=[]
    seq=ids[:]
    for step in range(len(ids)+nnew):
        pos=step; tid=seq[pos] if pos<len(seq) else gen_ids[-1]
        x=embd[tid].astype(np.float32).copy()
        for L in range(NL):
            an=W[(L,'attn_norm')].reshape(-1); n1=rms(x,an)
            q=(W[(L,'attn_q')]@n1).astype(np.float32).reshape(NH,HD)
            k=(W[(L,'attn_k')]@n1).astype(np.float32).reshape(NKV,HD)
            v=(W[(L,'attn_v')]@n1).astype(np.float32).reshape(NKV,HD)
            q=rope(q,pos); k=rope(k,pos)
            kc[L].append(k); kv[L].append(v)
            K=np.stack(kc[L]); V=np.stack(kv[L])  # (pos+1, NKV, HD)
            attn=np.zeros((NH,HD),np.float32)
            for h in range(NH):
                kvh=h//(NH//NKV)
                scores=(K[:,kvh,:]@q[h])/np.sqrt(HD)  # (pos+1,)
                scores=scores-scores.max(); w=np.exp(scores); w=w/w.sum()
                attn[h]=(w[:,None]*V[:,kvh,:]).sum(0)
            ao=(W[(L,'attn_output')]@attn.reshape(-1)).astype(np.float32)
            x=(x+ao).astype(np.float32)
            n2=rms(x,W[(L,'ffn_norm')].reshape(-1))
            x=(x+(W[(L,'ffn_down')]@(silu(W[(L,'ffn_gate')]@n2)*(W[(L,'ffn_up')]@n2)))).astype(np.float32)
        if pos>=len(ids)-1:
            xf=rms(x,onorm); logits=(embd@xf).astype(np.float32); nt=int(logits.argmax())
            gen_ids.append(nt); 
            if nt==128001 or nt==128009: break
    return ids,gen_ids
ids,g=gen("The capital of France is",12)
print("prompt token IDs:",ids)
print("generated IDs:",g)
print("generated text:",repr(decode(g)))
print("ollama oracle  :",repr(" Paris. The Eiffel Tower, a famous landmark in"))
json.dump({"prompt_ids":ids,"gen_ids":g},open("ref_tokens.json","w"))
