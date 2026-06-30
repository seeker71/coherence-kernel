import struct, random
random.seed(31)
D,S,H,F=8,4,2,16; Dh=D//H; EPS=1e-5; INV=1.0/(Dh**0.5)
def rf(x): return struct.unpack('<f',struct.pack('<f',x))[0]
def FM(a,b):return rf(a*b)
def FA(a,b):return rf(a+b)
def FS(a,b):return rf(a-b)
def FD(a,b):return rf(a/b)
def FQ(a):return rf(struct.unpack('<f',struct.pack('<f',a**0.5))[0])
def fexp(v):
    s=v;k=0
    while abs(s)>0.5:s=FM(s,0.5);k+=1
    n=1.0;t=1.0;acc=1.0
    for q in range(14):t=FM(t,FD(s,n));acc=FA(acc,t);n=FA(n,1.0)
    while k>0:acc=FM(acc,acc);k-=1
    return acc
C8=rf(0.7978845608028654);C4=rf(0.044715)
def gelu(v):
    z=FM(C8,FA(v,FM(C4,FM(v,FM(v,v)))));e=fexp(FM(2.0,z));th=FD(FS(e,1.0),FA(e,1.0));return FM(FM(0.5,v),FA(1.0,th))
def rnd(lo=-1.0,hi=1.0):return rf(round(random.uniform(lo,hi),3))
# weights
g1=[rnd(0.5,1.5) for _ in range(D)]; b1=[rnd(-0.3,0.3) for _ in range(D)]
Wq=[[rnd() for _ in range(D)] for _ in range(D)]; Wk=[[rnd() for _ in range(D)] for _ in range(D)]
Wv=[[rnd() for _ in range(D)] for _ in range(D)]; Wo=[[rnd() for _ in range(D)] for _ in range(D)]
g2=[rnd(0.5,1.5) for _ in range(D)]; b2=[rnd(-0.3,0.3) for _ in range(D)]
W1=[[rnd() for _ in range(D)] for _ in range(F)]; B1f=[rnd(-0.3,0.3) for _ in range(F)]
W2=[[rnd() for _ in range(F)] for _ in range(D)]; B2f=[rnd(-0.3,0.3) for _ in range(D)]
X=[[rnd() for _ in range(D)] for _ in range(S)]
def w(name,vals): open(name,'wb').write(b''.join(struct.pack('<f',rf(v)) for v in vals))
def flat(m): return [x for r in m for x in r]
w('bx.bin',flat(X)); w('bg1.bin',g1); w('bb1.bin',b1)
w('bwq.bin',flat(Wq)); w('bwk.bin',flat(Wk)); w('bwv.bin',flat(Wv)); w('bwo.bin',flat(Wo))
w('bg2.bin',g2); w('bb2.bin',b2)
w('bw1.bin',flat(W1)); w('bb1f.bin',B1f); w('bw2.bin',flat(W2)); w('bb2f.bin',B2f)
def ln(row,g,b):
    s=0.0
    for i in range(D-1,-1,-1): s=FA(row[i],s)
    mean=FD(s,rf(float(D)))
    vs=0.0
    for i in range(D-1,-1,-1):
        d=FS(row[i],mean); vs=FA(FM(d,d),vs)
    inv=FD(rf(1.0),FQ(FA(FD(vs,rf(float(D))),rf(EPS))))
    return [FA(FM(FM(FS(row[i],mean),inv),g[i]),b[i]) for i in range(D)]
def matmul(Xr,W):  # Xr[S][in], W[out][in] -> [S][out]
    out=[]
    for s in range(len(Xr)):
        row=[]
        for o in range(len(W)):
            acc=0.0
            for t in range(len(W[o])-1,-1,-1): acc=FA(FM(Xr[s][t],W[o][t]),acc)
            row.append(acc)
        out.append(row)
    return out
def attn(Q,K,V):
    Y=[[0.0]*D for _ in range(S)]
    for h in range(H):
        for i in range(S):
            sc=[0.0]*S
            for j in range(S):
                acc=0.0
                for t in range(Dh-1,-1,-1): acc=FA(FM(Q[i][h*Dh+t],K[j][h*Dh+t]),acc)
                sc[j]=FM(acc,rf(INV))
            m=sc[0]
            for j in range(1,S):
                if sc[j]>m:m=sc[j]
            ss=0.0
            for j in range(S-1,-1,-1): ss=FA(fexp(FS(sc[j],m)),ss)
            for t in range(Dh):
                o=0.0
                for j in range(S-1,-1,-1): o=FA(FM(FD(fexp(FS(sc[j],m)),ss),V[j][h*Dh+t]),o)
                Y[i][h*Dh+t]=o
    return Y
def ffn(Xr):
    out=[]
    for s in range(S):
        a=[gelu(FA(sum_down([FM(W1[k][j],Xr[s][j]) for j in range(D)]),B1f[k])) for k in range(F)]
        y=[FA(sum_down([FM(W2[i][j],a[j]) for j in range(F)]),B2f[i]) for i in range(D)]
        out.append(y)
    return out
def sum_down(terms):
    acc=0.0
    for t in reversed(terms): acc=FA(t,acc)
    return acc
n1=[ln(X[s],g1,b1) for s in range(S)]
Q=matmul(n1,Wq);K=matmul(n1,Wk);Vv=matmul(n1,Wv)
at=attn(Q,K,Vv); ao=matmul(at,Wo)
r1=[[FA(X[s][i],ao[s][i]) for i in range(D)] for s in range(S)]
n2=[ln(r1[s],g2,b2) for s in range(S)]
ff=ffn(n2)
out=[[FA(r1[s][i],ff[s][i]) for i in range(D)] for s in range(S)]
def bts(x):return struct.unpack('<I',struct.pack('<f',rf(x)))[0]
NLAYERS=4
XX=X
for _L in range(NLAYERS):
    n1=[ln(XX[s],g1,b1) for s in range(S)]
    Q=matmul(n1,Wq);K=matmul(n1,Wk);Vv=matmul(n1,Wv)
    at=attn(Q,K,Vv); ao=matmul(at,Wo)
    r1=[[FA(XX[s][i],ao[s][i]) for i in range(D)] for s in range(S)]
    n2=[ln(r1[s],g2,b2) for s in range(S)]
    ff=ffn(n2)
    XX=[[FA(r1[s][i],ff[s][i]) for i in range(D)] for s in range(S)]
import struct as _st
open("/private/tmp/claude-501/bout4.bin","wb").write(b"".join(_st.pack("<I",_st.unpack("<I",_st.pack("<f",rf(XX[s][i]))[0:4])[0]) for s in range(S) for i in range(D)))
print("DEPTH4 written")
